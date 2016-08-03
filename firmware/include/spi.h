/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __SPI_H
#define __SPI_H

#include <unistd.h>
#include <stdint.h>
#include <hal.h>

#define spi_send_buf(port, buf, length) \
    spi_exchange_buf(port, buf, NULL, length)
#define spi_recv_buf(port, buf, length) \
    spi_exchange_buf(port, NULL, buf, length)

void spi_lock(spi_t port);

void spi_unlock(spi_t port);

void spi_exchange_buf(spi_t port, uint8_t *out, uint8_t *in, uint32_t length);

uint32_t spi_read_octets(spi_t port, unsigned octets, uint32_t byte_order);

#define spi_read16(port) (uint16_t)spi_read_octets(port, 2);
#define spi_read24(port) (uint32_t)spi_read_octets(port, 3);
#define spi_read32(port) (uint32_t)spi_read_octets(port, 4);


#endif // __SPI_H
