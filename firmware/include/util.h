/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __UTIL_H
#define __UTIL_H

#include <stdint.h>
#if ((defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING) || (TESTING))
#  include <stdio.h>
#endif

#if 0
// #define assert(x) {if ((x) == 0) __asm("BKPT 0");}
#else
#undef assert
#define assert(x) do{ \
  if ((x) == 0) { \
    for(;;);   \
  } \
  }while (0)
#endif

void delay_ms(uint32_t ms);

#define MS_TO_TICKS(x)   ((x) / portTICK_PERIOD_MS)

#define DWT_CYCCNT   ((volatile uint32_t *) 0xE0001004)

uint32_t cpu_cycle_count(void);

#define MAX(x, y) ((x) > (y) ? (x) : (y))

#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define swap_endian(x)     \
  (((x>>24) & 0xff)     |  \
   ((x>>8)  & 0xff00)   |  \
   ((x<<8 ) & 0xff0000) |  \
   ((x<<24) & 0xff000000))  
    

#if ((defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING) || (TESTING))
#  define dbg_print printf
#else
#  define dbg_print(...)
#endif

#define STRUCT_SIZE_ASSERT(type, size) \
  extern char __size_check[1 - 2*!(sizeof(type) == size)]

#endif
