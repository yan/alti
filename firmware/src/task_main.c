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

static void handle_config(void);

static void handle_config(void)
{
}

void task_main(void *p)
{
  (void) p;

  portBASE_TYPE status;
  enum global_state_e state = GLOBAL_STATE_RESET;
  struct sensor_packet_s packet = {0};
  struct event_s event;// = {0};
  memset(&event, '\0', sizeof(event));

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

        packet.ticks = xTaskGetTickCount();
        packet.gps_sample = evt.payload.gps_sample;

        BaseType_t type = SENSOR_REQUEST_AIR_PRESSURE 
                         | SENSOR_REQUEST_ACCEL;
        xQueueSend(g.sensor_queue_g, &type, 0);
      }
      break;
#endif // CONFIG_USE_GPS

#if CONFIG_USE_ACCEL
      case GLOBAL_EVT_SENSOR_ACCEL: {
        packet.accel_sample = evt.payload.accel_sample;
      }
      break;
#endif
      case GLOBAL_EVT_SENSOR_COMPLETE: {
        if (event.header.in_progress) {
          logger_write_sample(&event, &packet);
        }

        filter_add_value(&g.filter_state, packet.mbarc);

        if (PIPE_OPEN(kSensorPipe)) {
          ble_tx(kSensorPipe, (void*)&packet, sizeof(packet));
        }
      }
      break;

      case GLOBAL_EVT_SENSOR_BARO: {
        packet.mbarc =  evt.payload.baro_sample.mbarc;
      }
      break;

      /* Bluetooth events */
      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {
        BaseType_t action = EVT_GPS_SLEEP;

        if (PIPE_OPEN(kSensorPipe)) {
          logger_start_event(&event);

          action = EVT_GPS_START;
          xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
        } 
        // xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
      }
      break;

      // case GLOBAL_EVT_NRF8001_RDY: {
      // }
      // break;

      case GLOBAL_EVT_NRF8001_DATA_RECEIVED: {
        // XXX Clean up the casting
        struct setting_packet_s *setting_msg = (void*) &evt.payload.data;

        switch (setting_msg->type) {
          case CONFIG_SETTING:
            break;
          case CONFIG_START_LOGGING:
            filter_init_state(&g.filter_state);
            logger_start_event(&event);
            setting_msg->event.status = 1;
            if (PIPE_OPEN(kConfigPipe)) {
              ble_tx(kConfigPipe, (void*)setting_msg, sizeof(*setting_msg));
            }
            break;

          case CONFIG_STOP_LOGGING:
            setting_msg->event.status = 1;
            if (PIPE_OPEN(kConfigPipe)) {
              ble_tx(kConfigPipe, (void*)setting_msg, sizeof(*setting_msg));
            }
            logger_end_event(&event);
            break;

          case CONFIG_SET_EVENT:
          {
            struct event_s *p_event = NULL;
            setting_msg->event.status = 0;

            while (logger_get_event(p_event, &event)) {
              // If we're searching for an id and we find it, return it
              if (event.header.event_id == setting_msg->event.event_id) {
                setting_msg->event.status = 1;
                break;
              }

              // If we're looking for the most recent id, just return it
              if (setting_msg->event.event_id == 0) {
                setting_msg->event.event_id = event.header.event_id;
                setting_msg->event.status = 1;
                break;
              }

              p_event = &event;

            }

            /* Return a response if the client is waiting for one */
            if (PIPE_OPEN(kConfigPipe)) {
              ble_tx(kConfigPipe, (void*)setting_msg, sizeof(*setting_msg));
            }

          }
          break;

          case CONFIG_FORMAT_STORAGE: 
            logger_format_storage();
          
          break;
          case CONFIG_LIST_EVENTS:
          {
            struct event_s *p_event = NULL;

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
              setting_msg->event_data = event.header;
              while (logger_read_sample(&event, i, &packet)) {
                ble_tx(kConfigPipe, (void*)&packet, EVENT_HEADER_SIZE);
              }
            }
          break;

          default:
            settings_apply(setting_msg);
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
