/**
 * The hardware abstraction layer functions for micros
 * 
 *
 * Copyright 2015 Yan Ivnitskiy
 */

#include <stdint.h>

enum pinmode_e {
  PINMODE_OUTPUT,
  PINMODE_INPUT,
};

/** XXX move this to a arch-specific header */
#ifdef STM32L1
//#  include <libopencm3/stm32/spi.h>
#endif

/** GPIO functions */
void pin_set(int port, int pin);

void pin_clear(int port, int pin);

void pin_config(int port, int pin, int options);


/** High level config functions */
void config_isr(int port);

void arch_config_ble(void);

void arch_config_clocks(void);

void arch_config_io(void);

void arch_config_nvic(void);

/** Timer functions */
void timer_config(int timer, int channel, int options);

void arch_init_timer(uint32_t timer, uint32_t channel, uint32_t prescaler, uint32_t period);

void arch_timer_set(uint32_t timer, uint32_t channel, uint32_t value);

/** SPI functions */
void spi_config(int port, int options);

void spi_set_msb(uint32_t port);

void spi_set_lsb(uint32_t port);

void arch_spi_config(uint32_t port, uint16_t byte_order);

uint8_t arch_spi_xfer(uint32_t port, uint8_t cmd);

void arch_spi_enable(uint32_t port);

void arch_spi_disable(uint32_t port);
