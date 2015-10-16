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
  PINMODE_OUTPUT,
  PINMODE_INPUT,
};


#define BYTEORDER_LSB   0
#define BYTEORDER_MSB   1

#if defined(STM32L1) && defined(OPENCM3)
#  include "hal_stm32_libopencm3.h"
#elif defined(STM32L1) && defined(STM32_STDPERIPH_LIB)
#  include "hal_stm32_stdperiphlib.h"
#elif defined(TESTING)
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

uint8_t arch_usart_recv(usart_t port);

/** High level config functions */
void config_isr(int port);

void arch_config_ble(void);

void arch_config_clocks(void);

void arch_config_io(void);

void arch_config_nvic(void);

/** Timer functions */
void timer_config(pwm_timer_t timer, int channel, int options);

void arch_init_timer(pwm_timer_t timer, uint32_t channel, uint32_t prescaler, uint32_t period);

void arch_timer_set(pwm_timer_t timer, uint32_t channel, uint32_t value);

/** SPI functions */
void spi_config(spi_t port, int options);

void spi_set_msb(spi_t port);

void spi_set_lsb(spi_t port);

void arch_spi_config(spi_t port, uint16_t byte_order);

uint8_t arch_spi_xfer(spi_t port, uint8_t cmd);

void arch_spi_enable(spi_t port);

/** Device-specific */
void enable_piezo(void);

void disable_piezo(void);

void enable_pulse(void);

void disable_pulse(void);

#endif // __HAL_H
