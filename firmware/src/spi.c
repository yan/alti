
 
#include "util.h"
#include "spi.h"
#include "globals.h"

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>



/** @brief ...
 */
void aero_spi_config(uint32_t port, uint16_t byte_order)
{
  port = ((port == 2) ? SPI2 : SPI1);

  byte_order = !!byte_order;

  spi_reset(port);
  spi_init_master(port,
                  SPI_CR1_BAUDRATE_FPCLK_DIV_32,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT,
                  byte_order ? SPI_CR1_MSBFIRST : SPI_CR1_LSBFIRST);

  g.spi_endian |= byte_order << (port - 1);

  spi_enable_software_slave_management(port);
  spi_disable_ss_output(port);
  spi_set_nss_high(port);

  spi_enable(port);
}


void aero_spi_enable(uint32_t port)
{
  rcc_periph_clock_enable((port == 2) ? RCC_SPI2 : RCC_SPI1);

  port = ((port == 2) ? SPI2 : SPI1);
  spi_enable(port);
}

/** @brief
 */
void aero_spi_disable(uint32_t port)
{
  port -= 1;

  rcc_periph_clock_disable((port == 2) ? RCC_SPI2 : RCC_SPI1);
  spi_disable(SPI1 + port);
}

/**
 * @brief
 */
void spi_send_byte(uint32_t port, uint8_t cmd)
{
  uint8_t val;
  val = spi_xfer(port, cmd);
  (void) val;
}

/**
 * @brief
 */
void spi_send_buf(uint32_t port, uint8_t *buf, uint32_t length)
{
  unsigned int i = 0;

  for (i = 0; i < length; i++) {
    spi_write(port, buf[i]);
  }
}

/**
 * @brief
 */
void spi_read_data(uint32_t port, uint8_t *data, uint32_t length)
{
  unsigned int i;

  for (i = 0; i < length; ++i) {
    data[i] = spi_read(port);
  }
}

/**
 * @brief
 */
uint32_t spi_read_octets(uint32_t port, unsigned octets)
{

  uint32_t value = 0, offset = 0;
  uint8_t byte;
  assert(octets > 0 && octets <= 4);


  do {
    octets--;
    byte = spi_xfer(port, 0);
    if (g.spi_endian << port) {
      value |= byte << offset;
      offset += 8;
    } else {
      value |= byte << (octets * 8);
    }
  } while (octets);

  return value;
}


