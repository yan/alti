/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <string.h>

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

#if CONFIG_USE_USART_ISR
  g.usart_mutex_g = xSemaphoreCreateBinary();
  configASSERT(g.usart_mutex_g != NULL);

  memset((void*)&g.usart_isr_state, '\0', sizeof(g.usart_isr_state));
#endif
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

/**
 * @brief Set an alarm 
 */
void config_set_alarm(unsigned alarm_idx, struct alarm_s alarm)
{
  assert(alarm_idx < ALARM_LEN);

  g.persisted_config.alarms[alarm_idx] = alarm;

  config_store_persistent();
}

/**
 * @brief Get an alarm
 */
struct alarm_s config_get_alarm(unsigned alarm_idx)
{
  assert(alarm_idx < ALARM_LEN);

  return g.persisted_config.alarms[alarm_idx];
}
