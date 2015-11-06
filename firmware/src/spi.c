/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include "util.h"
#include "spi.h"
#include "globals.h"
#include "hal.h"


/**
 * @brief
 */
void spi_exchange_buf(spi_t port, uint8_t *out, uint8_t *in, uint32_t length)
{
  unsigned int i = 0;
  uint8_t c = 0;

  for (i = 0; i < length; i++) {
    if (out != NULL) {
      c = out[i];
    }

    c = arch_spi_xfer(port, c);

    if (in != NULL) {
      in[i] = c;
    }
  }
}

/**
 * @brief
 */
uint32_t spi_read_octets(spi_t port, unsigned octets, uint32_t byte_order)
{

  uint32_t value = 0, offset = 0;
  uint8_t byte;

  assert(octets > 0 && octets <= 4);
  assert(byte_order == BYTEORDER_LSB || byte_order == BYTEORDER_MSB);

  do {
    octets--;
    byte = arch_spi_xfer(port, 0);
    if (byte_order == BYTEORDER_LSB) {
      value |= byte << offset;
      offset += 8;
    } else {
      value |= byte << (octets * 8);
    }
  } while (octets);

  return value;
}

