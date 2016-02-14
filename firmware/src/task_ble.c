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

static void ble_tx_helper(uint8_t pipe, uint8_t *data, size_t length);

const char s_null_cmd[sizeof(struct nrf8001_cmd_s)] = { 0 };

int g_received = 0, g_gotsemphrs = 0;


/**
 * @brief Send a command to the nRF8001
 */
int ble_send_cmd(struct nrf8001_cmd_s *cmd)
{
  if (xQueueSend(g.ble_data_g->in, cmd, portMAX_DELAY) == pdPASS) {
    pin_clear(NRF8001_REQN_GPIO, NRF8001_REQN);
    return 1;
  }

  return 0;
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

  while (length > 0) {
    size_t to_send = MIN(NRF8001_MAX_DATA_LENGTH, length);

    ble_tx_helper(pipe, data, to_send);

    length -= to_send;
    data += to_send;
  }

}
/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  (void) p;

  BaseType_t status;
  struct nrf8001_cmd_s incoming, outgoing, *outgoing_p;

  config_nrf8001();

  for (;;) {

    g_received++;

    status = xSemaphoreTake(g.ble_data_g->semphr, portMAX_DELAY);
    if (status != pdPASS) {
      return;
    }

    g_gotsemphrs++;

    status = xQueueReceive(g.ble_data_g->in, &outgoing, 0);
    if (status == pdFAIL) {
      outgoing_p = (struct nrf8001_cmd_s*)&s_null_cmd;
    } else {
      outgoing_p = &outgoing;
    }

    nrf8001_exchange_cmds(outgoing_p, &incoming);

    if (incoming.length > 0) {
      nrf8001_handle_event(&incoming);
    }
  }

  /* NOTREACHED */
}
