/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

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

#include <services.h>

int g_given = 0, g_events_received = 0, g_events_processed = 0;

void task_main(void *p)
{
  (void) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  enum global_state_e state = GLOBAL_STATE_RESET;

  for (;;) {
    status = xQueueReceive(g.main_queue_g, &evt, MAIN_EVENT_LOOP_TIMEOUT);


    /* We had no events, we're probably sleeping. Wake up, poll baro, and 
     * determine if we need to wake up.
     */
    if (status == pdFAIL) {
      if (state == GLOBAL_STATE_SLEEP) {
      }
      continue;
    }

    g_events_received++;

    switch (evt.type) {
      case GLOBAL_EVT_RESET:
        state = GLOBAL_STATE_RESET;
        break;

      case GLOBAL_EVT_SENSOR_GPS: {
        unsigned int accuracy = (unsigned int) evt.payload.gps_sample.accuracy;
        const uint8_t tx_pipe = PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX;

        if (PIPE_OPEN(tx_pipe)) {
          ble_tx(tx_pipe, (void*)&accuracy, sizeof(accuracy));
        }

      }
      break;
      case GLOBAL_EVT_SENSOR_BARO: {
        unsigned int pressure = (unsigned int) evt.payload.baro_sample.mbarc;
        const uint8_t tx_pipe = PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX;

        break;

        // log the data
        {
          struct sensor_packet_s packet = {
            .mbarc = (ms5611_mbarc_t) pressure,
            .ticks = xTaskGetTickCount()
          };
          write_sensor_packet(&packet);
        }

        // Send the data via ble
        if (PIPE_OPEN(tx_pipe)) {
          ble_tx(tx_pipe, (void*)&pressure, sizeof(pressure));
        }
      }
        break;

      /* Bluetooth events */
      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {
        // unsigned int new_rate;
        BaseType_t type = 0;
        if (PIPE_OPEN(PIPE_AERO_PRESSURE_BAROMETRIC_PRESSURE_TX)) {
          //new_rate = 100 / portTICK_PERIOD_MS;
          //type |= SENSOR_REQUEST_AIR_PRESSURE;
          type |= SENSOR_REQUEST_ACCEL;
          dbg_print("Starting to send pressure\n");
        } else {
          //new_rate = portMAX_DELAY;
          type |= SENSOR_REQUEST_STOP;
          dbg_print("Stopping to send pressure\n");
        }
          
        xQueueSend(g.sensor_queue_g, &type, 0);
      }
        break;

      case GLOBAL_EVT_NRF8001_RDY: {
        xSemaphoreGive(g.ble_data_g->semphr);
        g_given++;
      }
        break;

      case GLOBAL_EVT_NRF8001_EVENT: 
        nrf8001_handle_event(&evt.payload.nrf8001_cmd);
        break;

      case GLOBAL_EVT_LAST:
        break;

      default:
        state = GLOBAL_STATE_RESET;
        break;
    }

    (void) state;

    g_events_processed++;
  }

  (void) state; // XXX Remove me once we start using this

}
