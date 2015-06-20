
#ifndef __CONFIG_H
#define __CONFIG_H

#define BLE_EXTI_ISR_PRIORITY     (10 << 4)

#define MAIN_EVENT_LOOP_TIMEOUT   (1000 / portTICK_PERIOD_MS)

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

#endif
