
#include <stdint.h>
#include <string.h>

#include <FreeRTOS.h>

#include <libopencm3/stm32/spi.h>

#include <util.h>
#include <nrf8001.h>
#include <pins.h>

int g_nrf_events_received = 0;
/**
 *
 *
 */
void nrf8001_handle_event(struct nrf8001_cmd_s *event)
{
  configASSERT(event != NULL);

  switch (event->opcode) {
    case NRF8001_EVT_CMD_RSP:
      /* NOP */
      g_nrf_events_received++;
      dbg_print("Response opcode: %d, status = %d\n",  event->data[0], event->data[1]);
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
