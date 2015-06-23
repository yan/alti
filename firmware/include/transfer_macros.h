
#ifndef __TRANSFER_MACROS_H
#define __TRANSFER_MACROS_H

#ifndef BYTEORDER
#  error "BYTEORDER macro must be defined when including transfer_macros.h"
#endif

#ifndef SPI_PORT
#  error "SPI_PORT must be defined"
#endif

/**
 * @brief Convenience functions to send/receive a few bytes via i2c or spi. i2c
 * kept in, but it is less tested than SPI.
 */

#if defined(USE_SPI)
#  include <libopencm3/stm32/spi.h>
#  include <spi.h>
#  define send_byte(b) spi_send_byte(SPI_PORT, b)
#  define read8()  (uint8_t)spi_read_octets(SPI_PORT, 1, BYTEORDER);
#  define read16() (uint16_t)spi_read_octets(SPI_PORT, 2, BYTEORDER);
#  define read24() (uint32_t)spi_read_octets(SPI_PORT, 3, BYTEORDER);
#  define read32() (uint32_t)spi_read_octets(SPI_PORT, 4, BYTEORDER);
#else
#  include <libopencm3/stm32/i2c.h>
#  include <i2c.h>
#  define send_byte(b) i2c_send_byte(SPI_PORT, b)
#  define read8()  (uint8_t)i2c_read_octets(SPI_PORT, 1);
#  define read16() (uint16_t)i2c_read_octets(SPI_PORT, 2);
#  define read24() (uint32_t)i2c_read_octets(SPI_PORT, 3);
#  define read32() (uint32_t)i2c_read_octets(SPI_PORT, 4);
#endif


#endif
