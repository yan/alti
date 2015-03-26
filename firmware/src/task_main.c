
#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <config.h>
#include <nrf8001.h>
#include <ble.h>
#include <events.h>
#include <util.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_status_led.h>

int g_given = 0, g_events_received = 0, g_events_processed = 0;

void task_main(void *p)
{
  // QueueHandle_t main_queue = (QueueHandle_t) p;
  (void) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  //enum { BLE_START, BLE_INIT, BLE_IDLE, BLE_XFER } ble_state = BLE_START;
  enum global_state_e state = GLOBAL_STATE_RESET;

  for (;;) {
    status = xQueueReceive(main_queue_g, &evt, MAIN_EVENT_LOOP_TIMEOUT);

    if (status == pdFAIL) {
      // ??
      continue;
    }

    g_events_received++;

    switch (evt.type) {
      case GLOBAL_EVT_RESET:
      state = GLOBAL_STATE_RESET;
      break;

      case GLOBAL_EVT_RCVD_I2C:
      break;

      case GLOBAL_EVT_RCVD_SPI:
      break;

      case GLOBAL_EVT_NRF8001_RDY: {
        xSemaphoreGive(ble_data_g->semphr);
        g_given++;
      }
      break;

      case GLOBAL_EVT_NRF8001_EVENT: {
        struct nrf8001_cmd_s *cmd = evt.payload;
        nrf8001_handle_event(cmd);
      }
      break;

      case GLOBAL_EVT_LAST:
      break;

      default:
      state = GLOBAL_STATE_RESET;
      break;
    }

    g_events_processed++;
  }

  (void) state; // XXX Remove me once we start using this

}
