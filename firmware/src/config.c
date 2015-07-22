/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <config.h>
#include <globals.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <pins.h>
#include <spi.h>

void config_globals(void)
{
  g.flash_buffer.lock = (SemaphoreHandle_t) xSemaphoreCreateMutex();
  g.flash_buffer.write_offset = 0;
}
