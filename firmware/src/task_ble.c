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
#include <spi.h>
#include <nrf8001.h>
#include <task_ble.h>
#include <aci_cmds.h>
#include <globals.h>

static void ble_tx_helper(uint8_t pipe, uint8_t *prefix, size_t prefix_len,
    uint8_t *data, size_t length);

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Send a command to the nRF8001
 */
int ble_send_cmd(struct nrf8001_cmd_s *cmd)
{

  if (xQueueSend(g.ble_data_g.in, cmd, portMAX_DELAY) == pdPASS) {
    spi_lock(BT_STORE);

    pin_clear(NRF8001_REQN_GPIO, NRF8001_REQN);
    return 1;
  }

  return 0;
}

static void ble_tx_helper(uint8_t pipe, uint8_t *prefix, size_t prefix_len,
    uint8_t *data, size_t length)
{
  static struct nrf8001_cmd_s cmd;
  size_t i, dest = 0;

  assert(length + prefix_len > 0);
  assert(length + prefix_len <= NRF8001_MAX_DATA_LENGTH);

  if (!PIPE_OPEN(pipe))
    return;

  cmd.length = 2 + length + prefix_len;
  cmd.opcode = ACI_CMD_SEND_DATA;
  cmd.data[dest++] = pipe;

  /** NRF8001_MAX_DATA_LENGTH + 1 to include pipe byte */
  for (i = 0; i < prefix_len && dest < NRF8001_MAX_DATA_LENGTH+1; i++, dest++) {
    cmd.data[dest] = prefix[i];
  }

  for (i = 0; i < length && dest < NRF8001_MAX_DATA_LENGTH+1; i++, dest++) {
    cmd.data[dest] = data[i];
  }

  // Not necessary, but leaving it in.
  while (dest < NRF8001_MAX_DATA_LENGTH + 1) {
      cmd.data[dest++] = 0;
  }
  // for (i = i + 1; i < NRF8001_MAX_DATA_LENGTH; i++) {
    // cmd.data[i+1] = 0;
  // }

#if CONFIG_USE_COUNTERS
  g.counters.vals[COUNTER_CREDITS_TAKEN] += 1;
#endif // CONFIG_USE_COUNTERS

  BaseType_t status;
  status = xSemaphoreTake(g.ble_data_g.credits, portMAX_DELAY);
  if (status == pdFAIL) {
      assert(0);
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

    ble_tx_helper(pipe, NULL, 0, data, to_send);

    length -= to_send;
    data += to_send;
  }

}

/**
 * XXX This can't handle prefixes that are over NRF8001_MAX_DATA_LENGTH in
 * length
 */
void ble_tx_head(uint8_t pipe, uint8_t head, uint8_t *data, size_t length)
{
  int prefix_len = sizeof(head);
  uint8_t *prefix = &head;

  while (length > 0) {
    size_t to_send = MIN(NRF8001_MAX_DATA_LENGTH, length + prefix_len);

    ble_tx_helper(pipe, prefix, prefix_len, data, to_send - prefix_len);

    if (prefix_len) {
      to_send -= prefix_len;
      prefix_len = 0;
    }

    length -= to_send;
    data += to_send;
  }
}

#if !TESTING

/**
 * @brief This task does nothing but send and receive messages between us and 
 * the nrf8001. Actual logic is elsewhere.
 */
void task_ble(void *p)
{
  (void) p;

  BaseType_t status;
  struct nrf8001_cmd_s incoming , outgoing, *outgoing_p;

  config_nrf8001();
  (void) status;

  for (;;) {

#if CONFIG_USE_COUNTERS
    g.counters.vals[COUNTER_BLE_SEMAPHORES]++;
#endif

    status = xSemaphoreTake(g.ble_data_g.semphr, portMAX_DELAY);

#if CONFIG_USE_COUNTERS
    g.counters.vals[COUNTER_BLE_RECEIVED]++;
#endif

    /**
     * We got here because the nRF8001 is trying to send us data. This is either
     * because we are trying to send it a message and it's ready to receive, or
     * it's trying to deliver a command asynchronously.
     *
     * If we were trying to send data, we'd have taken a lock on the SPI bus 
     * after successfully queueing a message to send. If we didn't, receive
     * queue would be empty, and we'd need to acquire a lock before continuing.
     */
    status = xQueueReceive(g.ble_data_g.in, &outgoing, 0);
    if (status == pdPASS) {
        outgoing_p = &outgoing;
    } else {
        outgoing_p = &g.nrf8001_nul;
        /** If we got an outgoing message, we locked the bus then. If we didn't
         * we lock it now.
         */
        spi_lock(BT_STORE);
    }

    nrf8001_exchange_cmds(outgoing_p, &incoming);

    spi_unlock(BT_STORE);

    if (incoming.length > 0) {
      nrf8001_handle_event(&incoming);
    }
  }

  /* NOTREACHED */
}

#endif // TESTING

#ifdef __cplusplus
}
#endif
