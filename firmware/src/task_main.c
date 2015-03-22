
#include <FreeRTOS.h>
#include <queue.h>

#include <config.h>
#include <ble.h>
#include <events.h>
#include <task_main.h>

void task_main(void *p)
{
  QueueHandle_t mainQueue = (QueueHandle_t) p;
  portBASE_TYPE status;
  struct global_event_s evt;
  enum global_state_e state = GLOBAL_STATE_RESET;

  config_ble();

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

      case GLOBAL_EVT_LAST:
      break;

      default:
      state = GLOBAL_STATE_RESET;
      break;
    }
  }

  (void) state; // XXX Remove me once we start using this

}
