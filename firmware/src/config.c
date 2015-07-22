/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <config.h>
#include <globals.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <pins.h>
#include <spi.h>
#include <state.h>
#include <globals.h>
#include <flash.h>


void config_globals(void)
{
  g.flash_buffer.lock = (SemaphoreHandle_t) xSemaphoreCreateMutex();
  g.flash_buffer.write_offset = 0;
}

/**
 * @brief Load the running config from flash
 */
void config_load_persistent(void)
{
  flash_read(CONFIG_ADDR, (uint8_t*) &g.persisted_config, sizeof(g.persisted_config));
}

/**
 * @brief Store the running config to flash
 */
void config_store_persistent(void)
{
  flash_write(CONFIG_ADDR, (uint8_t*)&g.persisted_config, sizeof(g.persisted_config));
}
