/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdio.h>

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

void task_main(void *p)
{
  (void) p;
  portBASE_TYPE status;
  enum global_state_e state = GLOBAL_STATE_RESET;
  struct sensor_packet_s packet = {0};
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
        packet.gps_sample = evt.payload.gps_sample;

        BaseType_t type = SENSOR_REQUEST_AIR_PRESSURE 
                         | SENSOR_REQUEST_ACCEL;
        xQueueSend(g.sensor_queue_g, &type, 0);


        // const uint8_t tx_pipe = PIPE_DATA_UART_TX_TX;

        // if (PIPE_OPEN(tx_pipe)) {
          // ble_tx(tx_pipe, (void*)&accuracy, sizeof(accuracy));
        // }

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
        packet.ticks = xTaskGetTickCount();

        const uint8_t pipe = PIPE_DATA_UART_TX_TX;
        if (PIPE_OPEN(pipe)) {
          ble_tx(pipe, (void*)&packet, sizeof(packet));
        }
      }
      break;

      case GLOBAL_EVT_SENSOR_BARO: {
        packet.mbarc =  evt.payload.baro_sample.mbarc;

        // Send the data via ble
        // if (PIPE_OPEN(tx_pipe)) {
        //   ble_tx(tx_pipe, (void*)&pressure, sizeof(pressure));
        // }
      }
      break;

      /* Bluetooth events */
      case GLOBAL_EVT_NRF8001_PIPES_CHANGED: {

        BaseType_t action = EVT_GPS_SLEEP;
        if (PIPE_OPEN(PIPE_DATA_UART_TX_TX)) {
          action = EVT_GPS_START;
          xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
        } 
        // xQueueSend(g.gps_queue_g, &action, portMAX_DELAY);
      }
        break;

      case GLOBAL_EVT_NRF8001_RDY: {
        xSemaphoreGive(g.ble_data_g->semphr);
        counter_add_event(COUNTER_GIVEN);
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

    counter_add_event(COUNTER_EVENTS_PROCESSED);
  }

  (void) state; // XXX Remove me once we start using this

}
