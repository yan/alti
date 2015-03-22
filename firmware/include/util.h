
#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>

#define assert(x) {if ((x) == 0) __asm("BKPT 0");}

void delay_ms(uint32_t ms);

#endif
