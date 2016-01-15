
#include <flash.h>

#pragma once


size_t buffered_read(uint32_t addr, uint8_t *data, size_t len);
size_t buffered_write(uint32_t addr, uint8_t *data, size_t len);
uint8_t *buffered_get_page(uint32_t addr);
void buffered_flush(void);
