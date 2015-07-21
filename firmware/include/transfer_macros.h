/**
 * Copyright 2015 Yan Ivnitskiy
 */


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
#  include <spi.h>
#  define send_byte(b) arch_spi_xfer(SPI_PORT, b)
#  define read8()  (uint8_t)spi_read_octets(SPI_PORT, 1, BYTEORDER);
#  define read16() (uint16_t)spi_read_octets(SPI_PORT, 2, BYTEORDER);
#  define read24() (uint32_t)spi_read_octets(SPI_PORT, 3, BYTEORDER);
#  define read32() (uint32_t)spi_read_octets(SPI_PORT, 4, BYTEORDER);
#else
#  error "USE_SPI must be defined"
#endif


#endif
