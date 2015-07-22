/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <semphr.h>

#include <hal.h>
#include <util.h>
#include <events.h>
#include <pins.h>
#include <ble.h>
#include <task_ble.h>
#include <nrf8001.h>
#include <aci_cmds.h>

// todo: remove me after testing flash
#include <flash.h>

static void exchange_commands(struct nrf8001_cmd_s *outgoing, struct nrf8001_cmd_s *incoming);

static struct nrf8001_cmd_s s_null_cmd = {
  .length = 0,
  .opcode = 0,
  .data = { 0 }
};


int g_received = 0, g_gotsemphrs = 0;

static void exchange_commands(struct nrf8001_cmd_s *outgoing, struct nrf8001_cmd_s *incoming)
{
  BaseType_t status;

  status = xSemaphoreTake(g.ble_data_g->semphr, portMAX_DELAY);
  if (status != pdPASS) {
    return;
  }

  /**
   * Try receiving an outgoing command, if we don't have one, send 0's to get
   * an event
   */
  if (outgoing == NULL) {
    status = xQueueReceive(g.ble_data_g->in, &outgoing, 0);
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
    xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
  }
}

/**
 * @brief Send a command (by reference) to the nRF8001
 */
void ble_send_cmd(struct nrf8001_cmd_s *cmd)
{
  if (xQueueSend(g.ble_data_g->in, &cmd, portMAX_DELAY) == pdPASS) {
    pin_clear(NRF8001_GPIO, NRF8001_REQN);
  }
}

/**
 * @brief Transmit some data on an open pipe
 */
void ble_tx(uint8_t pipe, uint8_t *data, size_t length)
{
  static struct nrf8001_cmd_s cmd;
  size_t i;

  configASSERT(length < (NRF8001_MAX_CMD_LENGTH - 1));
  configASSERT(PIPE_OPEN(pipe));

  cmd.opcode = ACI_CMD_SEND_DATA;
  cmd.length = 1 + length;

  cmd.data[0] = pipe;
  for (i = 0; i < length; i++) {
    cmd.data[i+1] = data[i];
  }

  // Not necessary, but leaving it in.
  for (i = i + 1; i < NRF8001_MAX_CMD_LENGTH; i++) {
    cmd.data[i+1] = 0;
  }

  ble_send_cmd(&cmd);
}
/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  (void) p;

  /** XXX: outgoing should not be ever freed. */
  struct nrf8001_cmd_s *incoming = pvPortMalloc(sizeof(struct nrf8001_cmd_s));

  config_ble();

  for (;;) {

    g_received++;

    exchange_commands(NULL, incoming);
  }

  /* NOTREACHED */
}
