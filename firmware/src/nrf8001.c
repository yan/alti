/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <rtos.h>
#include <util.h>
#include <hal.h>
#include <nrf8001.h>
#include <task_ble.h>
#include <pins.h>
#include <events.h>
#include <spi.h>
#include <globals.h>

/** The order of includes is important */
#include <aci_cmds.h>
#include <aci_evts.h>
#include <services.h>

/** Set to 1 to enable nRF8001-related debugging output */
#define NRF8001_DEBUG  ( 1 )

#if NRF8001_DEBUG != 1
#  undef dbg_print
#  define dbg_print(x...)
#endif

static void nrf8001_connect(void);
static void nrf8001_setup(void);
static void handle_pipe_status(struct nrf8001_cmd_s *evt);
static void handle_connected(struct nrf8001_cmd_s *evt);
static void handle_data_received(struct nrf8001_cmd_s *evt);

enum nrf8001_state_e {
  STATE_IDLE,
  STATE_SETUP,
  STATE_STANDBY,
  STATE_ERROR
} state = STATE_IDLE;


struct nrf8001_state_s {
  enum nrf8001_state_e state;
  uint32_t events_received;
  unsigned int setup_msg_idx;
} s_nrf8001_state = {
  .state = STATE_IDLE,
  .events_received = 0,
  .setup_msg_idx = 0
};

struct nrf8001_cmd_s cmd_buf;

static void config_nrf8001_pins(void);
static void config_nrf8001_isr(void);
static void nrf8001_reset(void);


void nrf8001_isr(void)
{
  BaseType_t higher;
  
#if CONFIG_USE_COUNTERS
  g.counters.vals[COUNTER_BLE_ISR]++;
#endif // CONFIG_USE_COUNTERS
  xSemaphoreGiveFromISR(g.ble_data_g->semphr, &higher);
  portYIELD_FROM_ISR(higher);
}

static void config_nrf8001_pins(void)
{
  pin_config(NRF8001_REQN_GPIO, NRF8001_REQN, PINMODE_OUTPUT);
  pin_config(NRF8001_RST_GPIO, NRF8001_RST, PINMODE_OUTPUT);

  pin_set(NRF8001_REQN_GPIO, NRF8001_REQN);
  pin_set(NRF8001_RST_GPIO, NRF8001_RST);
}

static void config_nrf8001_isr(void)
{
  arch_config_ble();

  /* We'll use RDYN pin to interrupt*/
  pin_config(NRF8001_RDYN_GPIO, NRF8001_RDYN, PINMODE_INPUT);

}

static void nrf8001_reset(void)
{
  int i = 0;
  pin_set(NRF8001_RST_GPIO, NRF8001_RST);
  pin_clear(NRF8001_RST_GPIO, NRF8001_RST);
  // Spec calls for the line to be low at least 200ns, set it low and busy wait
  for (; i < 30; i++);
  pin_set(NRF8001_RST_GPIO, NRF8001_RST);
}

/**
 *
 */
void config_nrf8001(void)
{
  config_nrf8001_pins();
  config_nrf8001_isr();
  nrf8001_reset();
  memset(&g.nrf8001_nul, '\0', sizeof(g.nrf8001_nul));

  //pin_clear(NRF8001_REQN_GPIO, NRF8001_REQN);
}

static void nrf8001_connect(void) {
  uint16_t *args = (uint16_t*) cmd_buf.data;
  cmd_buf.opcode = ACI_CMD_CONNECT;
  cmd_buf.length = 5;

  args[0] = NRF8001_CONNECT_TIMEOUT;
  args[1] = NRF8001_CONNECT_INTERVAL;

  ble_send_cmd(&cmd_buf);
}


/**
 * @brief 
 */
static void nrf8001_setup(void)
{
  struct nrf8001_cmd_s *to_send;

  /**
   * This adds 1388 bytes to the image
   */
  static const struct {
    uint8_t status;
    uint8_t cmd[32];
  } init_cmds[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;

  to_send = (struct nrf8001_cmd_s*) init_cmds[s_nrf8001_state.setup_msg_idx].cmd;
  ble_send_cmd(to_send);
  s_nrf8001_state.setup_msg_idx++;
}

static void handle_data_received(struct nrf8001_cmd_s *evt)
{
  struct global_event_s response_evt;

  response_evt.type = GLOBAL_EVT_NRF8001_DATA_RECEIVED;
  response_evt.payload.nrf8001_cmd = *evt;

  xQueueSend(g.main_queue_g, &response_evt, 0);
}

/**
 * @brief Gets invoked when the state of tx/rx pipes is changed. Write pipe 
 * status to global state, then notify the main event loop that we received an
 * update.
 */
static void handle_pipe_status(struct nrf8001_cmd_s *evt)
{
  struct global_event_s response_evt;
  int i = 0;

#if NRF8001_DEBUG == 1
  dbg_print("Pipe status: \n");
  dbg_print("  pipes open: %x%x%x%x%x%x%x%x\n", evt->data[0], evt->data[1],
      evt->data[2], evt->data[3], evt->data[4], evt->data[5], evt->data[6],
      evt->data[7]);
  dbg_print("  pipes closed: %x%x%x%x%x%x%x%x\n", evt->data[8], evt->data[9],
      evt->data[10], evt->data[11], evt->data[12], evt->data[13], evt->data[14],
      evt->data[15]);
#endif

  (void) evt;

  for (i = 0; i < 8; i++) {
    g.pipes_open[i] = evt->data[i];
    g.pipes_closed[i] = evt->data[i+8];
  }

  response_evt.type = GLOBAL_EVT_NRF8001_PIPES_CHANGED;

  xQueueSend(g.main_queue_g, &response_evt, 0);
}

static void handle_connected(struct nrf8001_cmd_s *evt)
{
#if NRF8001_DEBUG == 1
    uint16_t interval = evt->data[7] | (evt->data[8] << 8);
    uint16_t latency = evt->data[9] | (evt->data[10] << 8);
    uint16_t sup_timeout = evt->data[11] | (evt->data[12] << 8);

    (void) interval; (void) latency; (void) sup_timeout;

    dbg_print("Device connected:\n");
    dbg_print("  type: %x\n", evt->data[0]);
    dbg_print("  peer addr: %x:%x:%x:%x:%x:%x\n", evt->data[1],
        evt->data[2], evt->data[3], evt->data[4], evt->data[5],
        evt->data[6]);
    dbg_print("  interval: %x\n", interval);
    dbg_print("  latency: %x\n", latency);
    dbg_print("  supervision timeout: %x\n", sup_timeout);
    dbg_print("  master clock accuracy: %x\n", evt->data[13]);
#else
    (void) evt;
#endif
}
/**
 *
 *
 */
void nrf8001_handle_event(struct nrf8001_cmd_s *event)
{
  assert(event != NULL);

  dbg_print("Event = %x, response opcode: %x, status = %x\n", event->opcode, event->data[0], event->data[1]);

  switch (event->opcode) {
    case ACI_EVT_CMD_RSP:
      if (event->data[0] == ACI_CMD_SETUP &&
          event->data[1] == ACI_STATUS_TRANSACTION_CONTINUE) {
        nrf8001_setup();
      }
      /* NOP */
      s_nrf8001_state.events_received++;
      break;

    case ACI_EVT_DEVICE_STARTED:

      if (event->data[0] == ACI_DEVICE_SETUP) {
        s_nrf8001_state.state = STATE_SETUP;
        nrf8001_setup();
      } else if (event->data[0] == ACI_DEVICE_STANDBY) {
        s_nrf8001_state.state = STATE_STANDBY;
        nrf8001_connect();
        dbg_print("Sent connection.\n");
      }
      break;

    case ACI_EVT_DISCONNECTED:
      if (event->data[0] == ACI_STATUS_ERROR_ADVT_TIMEOUT) {
        dbg_print("Timeout while advertising\n");
      }

      s_nrf8001_state.state = STATE_STANDBY;
      nrf8001_connect();
      break;

    case ACI_EVT_CONNECTED: 
      handle_connected(event);
      break;

    case ACI_EVT_PIPE_STATUS:
      handle_pipe_status(event);
      break;

    case ACI_EVT_DATA_RECEIVED:
      handle_data_received(event);
      break;

    default:
      ;
      break;
  }
}

/**
 * @brief Perform a SPI exchange of NRF8001 commands.
 *
 * This function assumes we have exclusive access to the SPI bus.
 */
void nrf8001_exchange_cmds(struct nrf8001_cmd_s *out, struct nrf8001_cmd_s *in)
{
  assert(out != NULL && in != NULL);
  assert(out->length <= NRF8001_MAX_CMD_LENGTH);

  int bytes_to_xfer = 0, i;
  uint8_t *out_ptr = (uint8_t*) out;

  pin_clear(NRF8001_REQN_GPIO, NRF8001_REQN);

  /* Make sure the tail end of *out is zero. If out->length is 0, this will 
   * effectively zero out the entire structure.
   *
   * TODO: Make sure this function is covered by unit tests
   */
  memset(out_ptr + out->length + 1, '\0', NRF8001_MAX_CMD_LENGTH - out->length);

  spi_set_lsb(BT_STORE);

  /* Send length, receive and ignore debug byte */
  arch_spi_xfer(BT_STORE, out->length);

  in->length = arch_spi_xfer(BT_STORE, out->opcode);
  in->opcode = arch_spi_xfer(BT_STORE, out->data[0]);

  /* '- 1' is the opcode byte that we already transferred */
  bytes_to_xfer = MAX(in->length, out->length - 1) - 1;

  for (i = 0; i < bytes_to_xfer; i++) {
    in->data[i] = arch_spi_xfer(BT_STORE, out->data[i + 1]);
  }

  pin_set(NRF8001_REQN_GPIO, NRF8001_REQN);
}

