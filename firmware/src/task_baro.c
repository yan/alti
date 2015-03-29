
#include <FreeRTOS.h>
#include <queue.h>

#include <globals.h>
#include <task_baro.h>

/**
 *
 */
void config_baro(void)
{
}

/**
 *
 */
void task_baro(void *p)
{
  (void) p;
  BaseType_t sleep_period = portMAX_DELAY, status;

  for (;;) {
    status = xQueueReceive(g.baro_queue_g, &sleep_period, sleep_period);
    if (status != pdPASS) {
      continue;
    }

  }
}
