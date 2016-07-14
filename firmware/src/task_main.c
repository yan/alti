/**
 * Copyright 2016 Yan Ivnitskiy
 */

#include <stdio.h>
#include <string.h>

#include <rtos.h>
#include <config.h>
#include <globals.h>
#include <nrf8001.h>
#include <events.h>
#include <state.h>
#include <util.h>
#include <logger.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_alert.h>
#include <task_sensor.h>
#include <task_gps.h>

#include <services.h>
#include <settings.h>

const uint8_t kSensorPipe = PIPE_SENSOR_STREAM_SENSOR_DATA_TX;
const uint8_t kConfigPipe = PIPE_AERO_CONFIG_AERO_CONFIG_TX_1;

static void handle_config(struct config_packet_s *config_msg );

static void handle_config(struct config_packet_s *config_msg )
{
  struct event_s event;

  switch (config_msg->type) {
    case CONFIG_SETTING:
      // De-duplicate from the default case
      settings_apply(&config_msg->setting);
      break;

    case CONFIG_START_LOGGING:
      filter_init_state(&g.filter_state);
      logger_start_event(&event);
      config_msg->type = CONFIG_RESPONSE_OK;
      break;

    case CONFIG_STOP_LOGGING:
      config_msg->type = CONFIG_RESPONSE_OK;
      logger_end_event(&event);
      break;

    case CONFIG_SET_EVENT:
    {
      struct event_s *p_event = NULL;

      config_msg->type = CONFIG_RESPONSE_FAIL;

      while (logger_get_event(p_event, &event)) {
        // If we're looking for the most recent id, just return it
        if (config_msg->event.event_id == 0) {
          config_msg->event.event_id = event.header.event_id;
          config_msg->type = CONFIG_RESPONSE_OK;
          break;
        }

        // If we're searching for an id and we find it, return it
        if (config_msg->event.event_id == event.header.event_id) {
          config_msg->type = CONFIG_RESPONSE_OK;
          break;
        }

        p_event = &event;
      }
    }
    break;

    case CONFIG_FORMAT_STORAGE: 
      logger_format_storage();
    
    break;
    case CONFIG_LIST_EVENTS:
    {
      struct event_s *p_event = NULL;
      struct sensor_packet_s packet;

      if (PIPE_OPEN(kConfigPipe)) {
        // First, send the event header
        ble_tx(kConfigPipe, (void*)&event, EVENT_HEADER_SIZE);

        // Then, send the samples
        while (logger_get_event(p_event, &event)) {
          ble_tx(kConfigPipe, (void*)&packet, sizeof(packet));

          p_event = &event;
        }
      }
    }
    break;

    /**
     * Send all the samples of the most recent event via the config chan
     */
    case CONFIG_GET_EVENTDATA:

      if (PIPE_OPEN(kConfigPipe)) {
        uint32_t i = 0;
        struct sensor_packet_s packet;

        config_msg->event_data = event.header;
        while (logger_read_sample(&event, i, &packet)) {
          ble_tx(kConfigPipe, (void*)&packet, EVENT_HEADER_SIZE);
        }
      }
    break;

    /* default:
       TODO: Unsupported message type, handle this better */
  }
}

struct tx_packet_s {
    uint8_t header;
    struct sensor_packet_s body;
} __attribute__((packed));

void task_main(void *p)
{
  (void) p;

  portBASE_TYPE status;
  enum global_state_e state = GLOBAL_STATE_RESET;

  struct tx_packet_s tx_packet ={ 0xff, {0}} ;
  struct event_s event;// = {0};

  memset(&event, '\0', sizeof(event));
  // tx_packet.header = 0xff;

  // struct event_header_s current_event;

  for (;;) {
    struct global_event_s evt;
    status = xQueueReceive(g.main_queue_g, &evt, MAIN_EVENT_LOOP_TIMEOUT);


    /* We had no events, we're probably sleeping. Wake up, poll baro, and 
     * determine if we need to wake up.
     */
    if (status == pdFAIL) {
      if (state == GLOBAL_STATE_SLEEP) {
      }
      continue;
    }

    counter_add_event(COUNTER_EVENTS_RECEIVED);

    switch (evt.type) {
      case GLOBAL_EVT_RESET:
        state = GLOBAL_STATE_RESET;
        break;

#if CONFIG_USE_GPS
      case GLOBAL_EVT_SENSOR_GPS: {
        /* Capture the gps sample, and get the other sensors */

        tx_packet.body.ticks = xTaskGetTickCount();
        tx_packet.body.gps_sample = evt.payload.gps_sample;

        BaseType_t type = SENSOR_REQUEST_AIR_PRESSURE 
                         | SENSOR_REQUEST_ACCEL;
        xQueueSend(g.sensor_queue_g, &type, 0);
      }
      break;
#endif // CONFIG_USE_GPS

#if CONFIG_USE_ACCEL
      case GLOBAL_EVT_SENSOR_ACCEL: {
        tx_packet.body.accel_sample = evt.payload.accel_sample;
      }
      break;
#endif
      case GLOBAL_EVT_SENSOR_COMPLETE: {
        if (event.header.in_progress) {
          logger_write_sample(&event, &tx_packet.body);
        }

        filter_add_value(&g.filter_state, tx_packet.body.mbarc);

        if (PIPE_OPEN(kSensorPipe)) {
          ble_tx(kSensorPipe, (void*)&tx_packet, sizeof(tx_packet));
        }
      }
      break;

      case GLOBAL_EVT_SENSOR_BARO: {
        tx_packet.body.mbarc =  evt.payload.baro_sample.mbarc;
      }
      break;

      /* Bluetooth events */
      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {
        BaseType_t action = EVT_GPS_SLEEP;

        if (PIPE_OPEN(kSensorPipe)) {
          state = GLOBAL_STATE_STREAMING;

          logger_start_event(&event);

          action = EVT_GPS_START;
          xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
        } else {
          action = EVT_GPS_SLEEP;
          xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
        }
      }
      break;

      // case GLOBAL_EVT_NRF8001_RDY: {
      // }
      // break;

      case GLOBAL_EVT_NRF8001_DATA_RECEIVED: {
        struct config_packet_s *config_msg = (void*) &evt.payload.data;

        /** TODO: Clean up status returning */
        handle_config(config_msg);

        if (PIPE_OPEN(kConfigPipe)) {
          ble_tx(kConfigPipe, (void*)config_msg, sizeof(*config_msg));
        }
      }
      break;

      case GLOBAL_EVT_LAST:
        break;

      default:
        state = GLOBAL_STATE_RESET;
        break;
    }

    (void) state;

    counter_add_event(COUNTER_EVENTS_PROCESSED);
  }

  (void) state; // XXX Remove me once we start using this

}
