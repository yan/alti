#include <config.h>
#include <globals.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <pins.h>
#include <spi.h>

void config_globals(void)
{
  g.flash_buffer.lock = (SemaphoreHandle_t) xSemaphoreCreateMutex();
}
