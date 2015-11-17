/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __CONFIG_H
#define __CONFIG_H

#include <stdint.h>
#include <globals.h>

#define BLE_EXTI_ISR_PRIORITY     (10 << 4)

#define MAIN_EVENT_LOOP_TIMEOUT   (1000 / portTICK_PERIOD_MS)

#define CONFIG_USE_GPS            ( 1 )

#define CONFIG_USE_ACCEL          ( 1 )
/**
 * @brief Configure the clocks 
 */
void config_clock(void);

/**
 * @brief Configure the interrupt controller
 */
void config_nvic(void);

/**
 * @brief Configure all I/O ports
 */
void config_io(void);

/**
 * @brief Initialize all global values
 */
void config_globals(void);

/**
 * @brief Load the running config from flash
 */
void config_load_persistent(void);

/**
 * @brief Store the running config to flash
 */
void config_store_persistent(void);

/**
 * @brief Set an alarm 
 */
void config_set_alarm(unsigned alarm_idx, struct alarm_s alarm);

/**
 * @brief Get an alarm
 */
struct alarm_s config_get_alarm(unsigned alarm_idx);

#endif
