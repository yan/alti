/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __SPI_H
#define __SPI_H

#include <unistd.h>
#include <stdint.h>
#include <hal.h>

void     spi_send_buf(spi_t port, uint8_t *buf, uint32_t length);
void     spi_read_data(spi_t port, uint8_t *data, uint32_t length);
uint32_t spi_read_octets(spi_t port, unsigned octets, uint32_t byte_order);

#define spi_read16(port) (uint16_t)spi_read_octets(port, 2);
#define spi_read24(port) (uint32_t)spi_read_octets(port, 3);
#define spi_read32(port) (uint32_t)spi_read_octets(port, 4);


#endif // __SPI_H
