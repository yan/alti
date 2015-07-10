
 
#include "util.h"
#include "spi.h"
#include "globals.h"

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>



/**
 * @brief Configure and enable SPI1 or SPI2. Initialize it with a default
 * byte order, although this can change on a per-message basis.
 *
 * @param port 1 for SPI1 or 2 for SPI2
 * @param byte_order 0 for LSB first, 1 for MSB first
 */
void aero_spi_config(uint32_t port, uint16_t byte_order)
{
  port = ((port == 1) ? SPI1 : SPI2);

  byte_order = !!byte_order;

  spi_reset(port);
  spi_init_master(port,
                  SPI_CR1_BAUDRATE_FPCLK_DIV_64,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT,
                  byte_order ? SPI_CR1_MSBFIRST : SPI_CR1_LSBFIRST);

  spi_enable_software_slave_management(port);
  spi_disable_ss_output(port);
  spi_set_nss_high(port);

  spi_enable(port);
}

/**
 * @brief Enable the SPI peripheral.
 *
 * @param port 1 for SPI1, 2 for SPI2
 */
void aero_spi_enable(uint32_t port)
{
  rcc_periph_clock_enable((port == 2) ? RCC_SPI2 : RCC_SPI1);

  port = ((port == 2) ? SPI2 : SPI1);
  spi_enable(port);
}

/**
 * @brief Disaable the SPI peripheral and its clock.
 *
 * @param port 1 for SPI1, 2 for SPI2
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
  spi_xfer(port, cmd);
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
uint32_t spi_read_octets(uint32_t port, unsigned octets, uint32_t byte_order)
{

  uint32_t value = 0, offset = 0;
  uint8_t byte;

  assert(octets > 0 && octets <= 4);
  assert(byte_order == BYTEORDER_LSB || byte_order == BYTEORDER_MSB);

  do {
    octets--;
    byte = spi_xfer(port, 0);
    if (byte_order == BYTEORDER_LSB) {
      value |= byte << offset;
      offset += 8;
    } else {
      value |= byte << (octets * 8);
    }
  } while (octets);

  return value;
}

void spi_set_msb(uint32_t port)
{
  spi_set_msb_first(port);
}

void spi_set_lsb(uint32_t port)
{
  spi_set_lsb_first(port);
}
