
#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>

#define assert(x) {if ((x) == 0) __asm("BKPT 0");}

void delay_ms(uint32_t ms);

#define DWT_CYCCNT   ((volatile uint32_t *) 0xE0001004)

uint32_t cpu_cycle_count(void);

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#endif
