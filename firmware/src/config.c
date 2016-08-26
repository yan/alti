/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <string.h>

#include <config.h>
#include <globals.h>

#include <rtos.h>
#include <pins.h>
#include <spi.h>
#include <state.h>
#include <globals.h>
#include <flash.h>


void config_globals(void)
{
  g.flash_buffer.lock = (SemaphoreHandle_t) xSemaphoreCreateMutex();
  g.flash_buffer.write_offset = 0;

  /* Initialize the address to all 1's to ensure it's an invalid address */
  g.flash_buffer.address = ~0;

  /* Initialize port locks. Currently only supports SPI1 and SPI2 */
  // g.port_locks[0].semphr =  xSemaphoreCreateMutex();
  g.port_locks[0].semphr =  xSemaphoreCreateBinary(); 
  g.port_locks[0].port = (uint32_t) SPI1;
  xSemaphoreGive(g.port_locks[0].semphr);

  // g.port_locks[1].semphr =  xSemaphoreCreateMutex();
  g.port_locks[1].semphr =  xSemaphoreCreateBinary(); 
  g.port_locks[1].port = (uint32_t) SPI2;
  xSemaphoreGive(g.port_locks[1].semphr);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  // vQueueAddToRegistry(g.port_locks[0].semphr, "spi1_mutex");
  // vQueueAddToRegistry(g.port_locks[1].semphr, "spi2_mutex");
#endif // configQUEUE_REGISTRY_SIZE

#if CONFIG_USE_USART_ISR
  g.usart_mutex_g = xSemaphoreCreateBinary();
  assert(g.usart_mutex_g != NULL);

  memset((void*)&g.usart_isr_state, '\0', sizeof(g.usart_isr_state));
#endif
}

/**
 * @brief Load the running config from flash
 */
void config_load_persistent(void)
{
  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);

  // g.flash_buffer.address = CONFIG_ADDR;
  // memcpy(g.flash_buffer.buffer, &g.persisted_config, sizeof(g.persisted_config));

  flash_read(CONFIG_ADDR, (uint8_t*)&g.persisted_config, sizeof(g.persisted_config));

  xSemaphoreGive(g.flash_buffer.lock);
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
