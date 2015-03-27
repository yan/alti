
#include <stdio.h>

#include <libopencm3/stm32/gpio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <util.h>
#include <events.h>
#include <pins.h>
#include <ble.h>
#include <task_ble.h>
#include <nrf8001.h>

struct ble_task_data_s *ble_data_g;
static struct nrf8001_cmd_s s_null_cmd = {
  .length = 0,
  .opcode = 0,
  .data = { 0 }
};


int g_received = 0, g_gotsemphrs = 0;

static void exchange_commands(struct nrf8001_cmd_s *incoming, struct nrf8001_cmd_s *outgoing);

static void exchange_commands(struct nrf8001_cmd_s *outgoing, struct nrf8001_cmd_s *incoming)
{
  BaseType_t status;


  status = xSemaphoreTake(ble_data_g->semphr, portMAX_DELAY);
  if (status != pdPASS) {
    return;
  }

  /**
   * Try receiving an outgoing command, if we don't have one, send 0's to get
   * an event
   */
  if (outgoing == NULL) {
    status = xQueueReceive(ble_data_g->in, &outgoing, 0);
    if (status == pdFAIL) {
      outgoing = &s_null_cmd;
    }
  }

  g_gotsemphrs++;

  nrf8001_exchange_cmds(outgoing, incoming);

  if (incoming->length > 0) {
    struct global_event_s evt;
    evt.type = GLOBAL_EVT_NRF8001_EVENT;
    evt.payload = incoming;
    xQueueSend(main_queue_g, &evt, portMAX_DELAY);
  }
}

void ble_send_cmd(struct nrf8001_cmd_s *cmd)
{
  struct nrf8001_cmd_s *ptr_to_send = cmd;
  gpio_clear(NRF8001_GPIO, NRF8001_REQN);

  xQueueSend(ble_data_g->in, &ptr_to_send, portMAX_DELAY);
}
/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  ble_data_g = (ble_task_data_t*)p;
  /** XXX: outgoing should not be ever freed. */
  struct nrf8001_cmd_s *incoming = pvPortMalloc(sizeof(struct nrf8001_cmd_s));

  config_ble();
  //nrf8001_setup(incoming);

  for (;;) {

    g_received++;

    exchange_commands(NULL, incoming);
  }

  /* NOTREACHED */
}
