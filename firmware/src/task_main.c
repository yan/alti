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
const uint8_t kConfigPipeTx = PIPE_AERO_CONFIG_AERO_CONFIG_TX_1;
const uint8_t kConfigPipeRx = PIPE_AERO_CONFIG_AERO_CONFIG_RX_ACK_AUTO;


static void handle_config(struct config_packet_s *config_msg );
static void gps_start(void);
static void gps_stop(void);

static void gps_start(void)
{
  BaseType_t action = EVT_GPS_START;
  xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
}
static void gps_stop(void)
{
  BaseType_t action = EVT_GPS_SLEEP;
  xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
}
static void handle_config(struct config_packet_s *config_msg )
{

  if (config_msg == NULL) {
    return;
  }

  switch (config_msg->type) {
    case CONFIG_SETTING:
      // De-duplicate from the default case
      settings_apply(&config_msg->setting);
      break;

    case CONFIG_START_LOGGING:
      filter_init_state(&g.filter_state);
      gps_start();
      logger_start_event(&g.current_event_g);
      config_msg->type = CONFIG_RESPONSE_OK;
      break;

    case CONFIG_STOP_LOGGING:
      gps_stop();
      logger_end_event(&g.current_event_g);
      config_msg->type = CONFIG_RESPONSE_OK;
      break;

    case CONFIG_SET_EVENT:
    {
      struct event_s *p_event = NULL;
      struct event_s event;

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
      struct event_s event;
      struct event_s *p_event = NULL;

      /**
       * XXX: Should we go into GLOBAL_STATE_TRANSFERRING_DATA state?. Is that
       * state even necessary?
       */
      if (PIPE_OPEN(kConfigPipeTx)) {
        while (logger_get_event(p_event, &event)) {
          ble_tx_head(kConfigPipeTx, CONFIG_RESPONSE_EVENT,
              (uint8_t*)&event.header, sizeof(event.header));

          p_event = &event;
        }
      }

      config_msg->type = CONFIG_RESPONSE_OK;
    }
    break;

    /**
     * Send all the samples of the most recent event via the config chan
     *
     * TODO: Check to make sure we're not currently logging the event
     */
    case CONFIG_GET_EVENTDATA:

      if (PIPE_OPEN(kConfigPipeTx)) {
        uint32_t i = 0;
        struct event_s event;
        struct sensor_packet_s sensors;

        logger_get_event(NULL, &event);

        config_msg->event_data = event.header;
        while (logger_read_sample(&event, i, &sensors)) {
          ble_tx_head(kConfigPipeTx, CONFIG_RESPONSE_SAMPLE, (uint8_t*)&sensors,
              sizeof(sensors));
        }

        config_msg->type = CONFIG_RESPONSE_OK;
      }
    break;

    /* default:
       TODO: Unsupported message type, handle this better */
  }
}

void task_main(void *p)
{
  (void) p;

  portBASE_TYPE status;
  enum global_state_e state = GLOBAL_STATE_RESET;

  struct sensor_packet_s sensors;

  memset(&sensors, '\0', sizeof(sensors));

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

        /**
         * Following states until GLOBAL_EVT_SENSOR_COMPLETE are used for
         * aggregating the complete sensor state. After the last sensor, the 
         * sensor task sends a GLOBAL_EVT_SENSOR_COMPLETE message, at which
         * point we can either store, act on, or transmit the full sensor packet
         *
         * These are kicked off by the GLOBAL_EVT_SENSOR_GPS state. Eventually,
         * these can be kicked off by a timer.
         */

#if CONFIG_USE_GPS
      case GLOBAL_EVT_SENSOR_GPS: {
        /* Capture the gps sample, and get the other sensors */

        sensors.ticks = xTaskGetTickCount();
        sensors.gps_sample = evt.payload.gps_sample;

        BaseType_t type = SENSOR_REQUEST_AIR_PRESSURE 
                        | SENSOR_REQUEST_ACCEL;
        xQueueSend(g.sensor_queue_g, &type, 0);
      }
      break;
#endif // CONFIG_USE_GPS

#if CONFIG_USE_ACCEL
      case GLOBAL_EVT_SENSOR_ACCEL: {
        sensors.accel_sample = evt.payload.accel_sample;
      }
      break;
#endif

      case GLOBAL_EVT_SENSOR_BARO: {
        sensors.mbarc =  evt.payload.baro_sample.mbarc;
      }
      break;

      case GLOBAL_EVT_SENSOR_COMPLETE: {

        if (g.current_event_g.header.in_progress) {
          status = logger_write_sample(&g.current_event_g, &sensors);

          /* Writing failed because we filled up storage with just the current 
           * event. Finish logging.
           */
          if (!status) {
            gps_stop();
            logger_end_event(&g.current_event_g);
          }
        }

        filter_add_value(&g.filter_state, sensors.mbarc);

        if (PIPE_OPEN(kSensorPipe)) {
          ble_tx_head(kSensorPipe, CONFIG_RESPONSE_SAMPLE, (uint8_t*)&sensors,
              sizeof(sensors));
        }
      }
      break;

      /* Bluetooth events */
      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {

        if (PIPE_OPEN(kSensorPipe)) {
          state = GLOBAL_STATE_STREAMING;

          // gps_start();
        } else {
          // gps_stop();
        }
      }
      break;

      // case GLOBAL_EVT_NRF8001_RDY: {
      // }
      // break;

      case GLOBAL_EVT_NRF8001_DATA_RECEIVED: {
        struct nrf8001_datarx_s *rx_data =
          (struct nrf8001_datarx_s*) &evt.payload.nrf8001_cmd.data;

        /* TODO: Clean this up */
        struct config_packet_s *config_msg =
          (struct config_packet_s *) &rx_data->data;

        if (rx_data->pipe == kConfigPipeRx) {
          /** TODO: Clean up status returning */
          handle_config(config_msg);

          if (PIPE_OPEN(kConfigPipeTx)) {
            ble_tx_head(kConfigPipeTx, CONFIG_RESPONSE_MSG,
                (uint8_t*)config_msg, sizeof(*config_msg));
          }
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
