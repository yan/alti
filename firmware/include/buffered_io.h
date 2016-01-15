
#include <flash.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

size_t buffered_read(uint32_t addr, uint8_t *data, size_t len);

size_t buffered_read_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin, int should_wrap);

size_t buffered_write(uint32_t addr, const uint8_t *data, size_t len);

size_t buffered_write_wrapped(uint32_t addr, const uint8_t *data, size_t len,
    uint32_t start_margin, int should_wrap);

uint8_t *buffered_get_page(uint32_t addr);

void buffered_flush(void);

#ifdef __cplusplus
}
#endif
