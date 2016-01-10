/**
 * The hardware abstraction layer functions for micros
 * 
 *
 * Copyright 2015 Yan Ivnitskiy
 */
#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>

enum pinmode_e {
  PINMODE_AF_0 = 0,
  PINMODE_AF_1,
  PINMODE_AF_2,
  PINMODE_AF_3,
  PINMODE_AF_4,
  PINMODE_AF_5,
  PINMODE_AF_6,
  PINMODE_AF_7,
  PINMODE_AF_8,
  PINMODE_OUTPUT,
  PINMODE_INPUT
};
#define IS_VALID_AF_PINMODE(x) (x >= PINMODE_AF_0 && x < PINMODE_AF_8)

#define BYTEORDER_LSB   0
#define BYTEORDER_MSB   1

#if defined(STM32L1) && defined(OPENCM3)
#  include "hal_stm32_libopencm3.h"
#elif defined(STM32L1) && defined(STM32_STDPERIPH_LIB)
#  include "hal_stm32_stdperiphlib.h"
#elif (TESTING)
#  include "hal_empty.h"
#else
#  error "Unsupported architecture"
#endif

/** GPIO functions */
void pin_set(gpio_t port, pin_t pin);

void pin_clear(gpio_t port, pin_t pin);

void pin_toggle(gpio_t port, pin_t pin);

void pin_config(gpio_t port, pin_t pin, int options);

/** USART functions */
void arch_usart_send(usart_t port, uint8_t data);

void arch_config_uart(usart_t port, int baud);

uint16_t arch_usart_recv(usart_t port);

void arch_usart_set_baud(usart_t port, int baud);

void arch_enable_usart_interrupt(usart_t port);

void arch_disable_usart_interrupt(usart_t port);

/** High level config functions */

void arch_config_ble(void);

void arch_config_clocks(void);

void arch_config_nvic(void);

/** Timer functions */
void timer_config(pwm_timer_t timer, int channel, int options);

void arch_init_timer(pwm_timer_t timer, uint32_t channel, uint32_t prescaler, uint32_t period);

void arch_timer_set(pwm_timer_t timer, uint32_t channel, uint32_t value);

void timer_disable(pwm_timer_t timer);

void timer_enable(pwm_timer_t timer);

/** SPI functions */
void spi_config(spi_t port, int options);

void spi_set_msb(spi_t port);

void spi_set_lsb(spi_t port);

void arch_spi_config(spi_t port);

uint8_t arch_spi_xfer(spi_t port, uint8_t cmd);

void arch_spi_enable(spi_t port);

/** Device-specific */

void batt_sense_enable(void);

void batt_sense_disable(void);

uint16_t batt_sans_sample(void);

#endif // __HAL_H
