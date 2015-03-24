
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <config.h>
#include <ble.h>
#include <events.h>
#include <task_main.h>
#include <task_ble.h>
#include <task_status_led.h>

int g_given = 0;

void task_main(void *p)
{
  QueueHandle_t main_queue = (QueueHandle_t) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  enum { BLE_START, BLE_INIT, BLE_IDLE, BLE_XFER } ble_state = BLE_START;

  enum global_state_e state = GLOBAL_STATE_RESET;

  for (;;) {
    status = xQueueReceive(main_queue, &evt, MAIN_EVENT_LOOP_TIMEOUT);

    if (status == pdFAIL) {
      // ??
      continue;
    }

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

        switch (ble_state) {
          case BLE_START: {
            enum task_status_event_e status_event = STATUS_EVENT_BLINK_ONCE;
            xQueueSend(status_queue_g, &status_event, portMAX_DELAY);

            nrf8001_setup();
            ble_state = BLE_INIT;
            break;
          }

          default:
            // XXX
            break;
        }
      }
      break;

      case GLOBAL_EVT_LAST:
      break;

      default:
      state = GLOBAL_STATE_RESET;
      break;
    }
  }

  (void) state; // XXX Remove me once we start using this

}
