
#include <FreeRTOS.h>
#include <queue.h>

#include <config.h>
#include <ble.h>
#include <events.h>
#include <task_main.h>
#include <task_status_led.h>

void task_main(void *p)
{
  QueueHandle_t mainQueue = (QueueHandle_t) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  enum global_state_e state = GLOBAL_STATE_RESET;

  for (;;) {
    status = xQueueReceive(mainQueue, &evt, MAIN_EVENT_LOOP_TIMEOUT);

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
        enum task_status_event_e status_event = STATUS_EVENT_BLINK_TWICE;
        xQueueSend(status_queue_g, &status_event, portMAX_DELAY);
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
