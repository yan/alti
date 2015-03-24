
#include <libopencm3/stm32/gpio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <events.h>
#include <pins.h>
#include <ble.h>
#include <task_ble.h>
#include <nrf8001.h>

struct ble_task_data_s *ble_data_g;

int g_received = 0, g_gotsemphrs = 0;

/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  BaseType_t status;
  ble_data_g = (ble_task_data_t*)p;
  /** XXX: outgoign is allocated here, which it probably shouldn't be */
  struct nrf8001_cmd_s outgoing, *incoming;

  config_ble();

  for (;;) {


    status = xQueueReceive(ble_data_g->in, &incoming, portMAX_DELAY);
    if (status != pdPASS) {
      xSemaphoreGive(ble_data_g->semphr);
      continue;
    }

    g_received++;

    gpio_clear(NRF8001_GPIO, NRF8001_REQN);

    status = xSemaphoreTake(ble_data_g->semphr, portMAX_DELAY);
    if (status != pdPASS) {
      continue;
    }

    g_gotsemphrs++;

    nrf8001_exchange_cmds(incoming, &outgoing);

    gpio_set(NRF8001_GPIO, NRF8001_REQN);

    if (outgoing.length > 0) {
      struct global_event_s evt;
      evt.type = GLOBAL_EVT_NRF8001_EVENT;
      evt.payload = &outgoing;
      xQueueSend(main_queue_g, &evt, portMAX_DELAY);
    }
  }
}
