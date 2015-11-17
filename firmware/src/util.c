/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <FreeRTOS.h>
#include <task.h>

#include <util.h>


void delay_ms(uint32_t ms)
{
  vTaskDelay(MS_TO_TICKS(ms));
}

uint32_t cpu_cycle_count(void)
{
  return *DWT_CYCCNT;
}
