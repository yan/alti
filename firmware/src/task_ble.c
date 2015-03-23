
#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <task_ble.h>
#include <nrf8001.h>

/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  BaseType_t status;
  ble_task_data_t data = *(ble_task_data_t*)p;
  struct nrf8001_cmd_s outgoing, *incoming, *outgoing_p;

  for (;;) {

    status = xSemaphoreTake(data.semphr, portMAX_DELAY);
    if (status != pdPASS) {
      continue;
    }
    

    status = xQueueReceive(data.in, &incoming, portMAX_DELAY);
    if (status != pdPASS) {
      xSemaphoreGive(data.semphr);
      continue;
    }

    nrf8001_exchange_cmds(incoming, &outgoing);

    if (outgoing.length > 0) {
      outgoing_p = &outgoing;
      xQueueSend(data.out, &outgoing_p, portMAX_DELAY);
    }

    xSemaphoreGive(data.semphr);
  }
}
