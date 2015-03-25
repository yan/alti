
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

#include <services.h>

struct ble_task_data_s *ble_data_g;

int g_received = 0, g_gotsemphrs = 0;

static void nrf8001_setup(struct nrf8001_cmd_s *outgoing_buf);
static void exchange_commands(struct nrf8001_cmd_s *incoming, struct nrf8001_cmd_s *outgoing);

/**
 *
 */
static void nrf8001_setup(struct nrf8001_cmd_s *outgoing_buf)
{
  int i = 0;

  /**
   * This adds 1388 bytes to the image
   */
  static struct {
    uint8_t status;
    uint8_t cmd[32];
  } init_cmds[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;

  dbg_print("Starting setup\n");

  for (i = 0; i < NB_SETUP_MESSAGES; i++) {
    xSemaphoreTake(ble_data_g->semphr, portMAX_DELAY);

    dbg_print("Sending message %d\n", i);

    g_gotsemphrs++;

    struct nrf8001_cmd_s *to_send = (struct nrf8001_cmd_s*) init_cmds[i].cmd;
    exchange_commands(to_send, outgoing_buf);
    //xQueueSend(ble_data_g->in, &to_send, portMAX_DELAY);
  }
}

static void exchange_commands(struct nrf8001_cmd_s *incoming, struct nrf8001_cmd_s *outgoing)
{
  BaseType_t status;

  gpio_clear(NRF8001_GPIO, NRF8001_REQN);

  status = xSemaphoreTake(ble_data_g->semphr, portMAX_DELAY);
  if (status != pdPASS) {
    return;
  }

  g_gotsemphrs++;

  nrf8001_exchange_cmds(incoming, outgoing);

  gpio_set(NRF8001_GPIO, NRF8001_REQN);

  if (outgoing->length > 0) {
    struct global_event_s evt;
    evt.type = GLOBAL_EVT_NRF8001_EVENT;
    evt.payload = outgoing;
    xQueueSend(main_queue_g, &evt, portMAX_DELAY);
  }
}

/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  BaseType_t status;
  ble_data_g = (ble_task_data_t*)p;
  /** XXX: outgoing should not be ever freed. */
  struct nrf8001_cmd_s *outgoing = pvPortMalloc(sizeof(struct nrf8001_cmd_s));
  struct nrf8001_cmd_s *incoming;

  config_ble();
  nrf8001_setup(outgoing);

  for (;;) {


    status = xQueueReceive(ble_data_g->in, &incoming, portMAX_DELAY);
    if (status != pdPASS) {
      xSemaphoreGive(ble_data_g->semphr);
      continue;
    }

    g_received++;

    exchange_commands(incoming, outgoing);
  }

  /* NOTREACHED */
}
