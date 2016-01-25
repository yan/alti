
#include <flash.h>

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

uint32_t buffered_wrap_addr(uint32_t x, uint32_t margin);

size_t buffered_read(uint32_t addr, uint8_t *data, size_t len);

size_t buffered_read_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin);

size_t buffered_write(uint32_t addr, const uint8_t *data, size_t len);

size_t buffered_write_wrapped(uint32_t addr, const uint8_t *data, size_t len,
    uint32_t start_margin);

uint8_t *buffered_get_page(uint32_t addr);

void buffered_flush(void);

int buffered_ranges_overlap(uint32_t lhs_start, uint32_t lhs_size, uint32_t rhs_start,
    uint32_t rhs_size);

#ifdef __cplusplus
}
#endif
