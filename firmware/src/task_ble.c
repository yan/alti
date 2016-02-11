/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdio.h>

#include <rtos.h>
#include <hal.h>
#include <util.h>
#include <events.h>
#include <pins.h>
//#include <ble.h>
#include <nrf8001.h>
#include <task_ble.h>
#include <aci_cmds.h>
#include <globals.h>

static void exchange_commands(struct nrf8001_cmd_s *incoming);
static void ble_tx_helper(uint8_t pipe, uint8_t *data, size_t length);

const char s_null_cmd[sizeof(struct nrf8001_cmd_s)] = { 0 };

int g_received = 0, g_gotsemphrs = 0;

static void exchange_commands(struct nrf8001_cmd_s *incoming)
{
  BaseType_t status;
  struct nrf8001_cmd_s outgoing;

  status = xSemaphoreTake(g.ble_data_g->semphr, portMAX_DELAY);
  if (status != pdPASS) {
    return;
  }

  g_gotsemphrs++;

  status = xQueueReceive(g.ble_data_g->in, &outgoing, 0);
  if (status == pdFAIL) {
    nrf8001_exchange_cmds((struct nrf8001_cmd_s*)&s_null_cmd, incoming);
  } else {
    nrf8001_exchange_cmds(&outgoing, incoming);
  }

  if (incoming->length > 0) {
    struct global_event_s evt;
    evt.type = GLOBAL_EVT_NRF8001_EVENT;
    evt.payload.nrf8001_cmd = *incoming;
    xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
  }
}

/**
 * @brief Send a command (by reference) to the nRF8001
 */
void ble_send_cmd(struct nrf8001_cmd_s *cmd)
{
  if (xQueueSend(g.ble_data_g->in, cmd, portMAX_DELAY) == pdPASS) {
    pin_clear(NRF8001_REQN_GPIO, NRF8001_REQN);
  }
}

static void ble_tx_helper(uint8_t pipe, uint8_t *data, size_t length)
{
  static struct nrf8001_cmd_s cmd;
  size_t i;

  assert(length + 2 < NRF8001_MAX_CMD_LENGTH);
  assert(PIPE_OPEN(pipe));

  cmd.opcode = ACI_CMD_SEND_DATA;
  cmd.length = 2 + length;

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
 * @brief Transmit some data on an open pipe
 */
void ble_tx(uint8_t pipe, uint8_t *data, size_t length)
{
  size_t to_send;// sent = 0;

  while (length > 0) {
    // TODO: Move this constant elsewhere
    to_send = MIN(20, length);
    ble_tx_helper(pipe, data, to_send);
    length -= to_send;
    data += to_send;
  }

#if 0
  static struct nrf8001_cmd_s cmd;
  size_t i;

  assert(length + 2 < NRF8001_MAX_CMD_LENGTH);
  assert(PIPE_OPEN(pipe));

  cmd.opcode = ACI_CMD_SEND_DATA;
  cmd.length = 2 + length;

  cmd.data[0] = pipe;
  for (i = 0; i < length; i++) {
    cmd.data[i+1] = data[i];
  }

  // Not necessary, but leaving it in.
  for (i = i + 1; i < NRF8001_MAX_CMD_LENGTH; i++) {
    cmd.data[i+1] = 0;
  }

  ble_send_cmd(&cmd);
#endif
}
/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  (void) p;

  /** XXX: outgoing should not be ever freed. */
  // struct nrf8001_cmd_s *incoming = pvPortMalloc(sizeof(struct nrf8001_cmd_s));
  struct nrf8001_cmd_s incoming;// = pvPortMalloc(sizeof(struct nrf8001_cmd_s));

  config_nrf8001();

  for (;;) {

    g_received++;

    exchange_commands(&incoming);
  }

  /* NOTREACHED */
}
