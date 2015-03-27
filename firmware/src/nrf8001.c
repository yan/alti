
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>

#include <libopencm3/stm32/spi.h>

#include <util.h>
#include <nrf8001.h>
#include <task_ble.h>
#include <pins.h>

/** The order of includes is important */
#include <aci_cmds.h>
#include <aci_evts.h>
#include <services.h>

/** Set to 1 to enable nRF8001-related debugging output */
#define NRF8001_DEBUG  ( 0 )

static void nrf8001_connect(void);
static void nrf8001_setup(void);

enum nrf8001_state_e {
  STATE_IDLE,
  STATE_SETUP,
  STATE_STANDBY,
  STATE_ERROR
} state = STATE_IDLE;


int g_nrf_events_received = 0;

struct nrf8001_state_s {
  enum nrf8001_state_e state;
  uint32_t events_received;
} s_nrf8001_state = {
  .state = STATE_IDLE,
  .events_received = 0
};

struct nrf8001_cmd_s cmd_buf;

static void nrf8001_connect(void) {
  uint16_t *args = (uint16_t*) cmd_buf.data;
  cmd_buf.opcode = ACI_CMD_CONNECT;
  cmd_buf.length = 5;

  args[0] = 20; // timeout, in seconds
  args[1] = 0x100; // interval, 160ms (256 * 0.625ms)
  ble_send_cmd(&cmd_buf);
}


/**
 *
 */
static void nrf8001_setup(void)
{
  static int i = 0;
  struct nrf8001_cmd_s *to_send;

  /**
   * This adds 1388 bytes to the image
   */
  static const struct {
    uint8_t status;
    uint8_t cmd[32];
  } init_cmds[NB_SETUP_MESSAGES] = SETUP_MESSAGES_CONTENT;

#if NRF8001_DEBUG == 1
  dbg_print("Sending %d\n", i);
#endif

  to_send = (struct nrf8001_cmd_s*) init_cmds[i].cmd;
  ble_send_cmd(to_send);
  i++;
}
/**
 *
 *
 */
void nrf8001_handle_event(struct nrf8001_cmd_s *event)
{
  configASSERT(event != NULL);

#if NRF8001_DEBUG == 1
  dbg_print("Event = %x, response opcode: %d, status = %d\n", event->opcode, event->data[0], event->data[1]);
#endif

  switch (event->opcode) {
    case ACI_EVT_CMD_RSP:
      if (event->data[0] == ACI_CMD_SETUP) {
        if (event->data[1] == ACI_STATUS_TRANSACTION_CONTINUE) {
          nrf8001_setup();
        }
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
#if NRF8001_DEBUG == 1
        dbg_print("Sent connection.\n");
#endif
      }
      break;

    case ACI_EVT_DISCONNECTED:
      if (event->data[0] == ACI_STATUS_ERROR_ADVT_TIMEOUT) {
#if NRF8001_DEBUG == 1
        dbg_print("Timeout while advertising\n");
#endif
      }

      s_nrf8001_state.state = STATE_STANDBY;
      break;

    default:
      break;
  }
}

/**
 * @brief Perform a SPI exchange of NRF8001 commands. Calling code must set
 * REQN low before sending and high afterwards.
 */
void nrf8001_exchange_cmds(struct nrf8001_cmd_s *out, struct nrf8001_cmd_s *in)
{
  assert(out != NULL && in != NULL);
  assert(out->length <= NRF8001_MAX_CMD_LENGTH);

  int bytes_to_xfer = 0, i;
  uint8_t *out_ptr = (uint8_t*) out;

  gpio_clear(NRF8001_GPIO, NRF8001_REQN);

  /* Make sure the tail end of *out is zero. If out->length is 0, this will 
   * effectively zero out the entire structure.
   *
   * TODO: Make sure this function is covered by unit tests
   */
  memset(out_ptr + out->length + 1, '\0', NRF8001_MAX_CMD_LENGTH-out->length);

  /* Send length, receive and ignore debug byte */
  spi_xfer(NRF8001_SPI, out->length);

  in->length = spi_xfer(NRF8001_SPI, out->opcode);
  in->opcode = spi_xfer(NRF8001_SPI, out->data[0]);

  bytes_to_xfer = MAX(in->length, out->length - 1) - 1;

  for (i = 0; i < bytes_to_xfer; i++) {
    in->data[i] = spi_xfer(NRF8001_SPI, out->data[i + 1]);
  }

  gpio_set(NRF8001_GPIO, NRF8001_REQN);
}
