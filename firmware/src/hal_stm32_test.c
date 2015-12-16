/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <hal.h>
#include <nrf8001.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <pins.h>
#include <spi.h>

void exti3_isr(void);

void pin_set(gpio_t port, pin_t pin)
{
  (void) port;
  (void) pin;
}

void pin_clear(gpio_t port, pin_t pin)
{
  (void) port;
  (void) pin;
}

void pin_toggle(gpio_t port, pin_t pin)
{
  (void) port;
  (void) pin;
}

void pin_config(gpio_t port, pin_t pin, int options)
{
  (void) port;
  (void) pin;
  (void) options;
}

void spi_config(spi_t port, int options)
{
  (void) port;
  (void) options;
}

void timer_config(pwm_timer_t timer, int channel, int options)
{
  (void) timer;
  (void) channel;
  (void) options;
}

void arch_config_ble(void)
{
}

void arch_usart_send(usart_t port, uint8_t data)
{
  (void) port;
  (void) data;
}

uint16_t arch_usart_recv(usart_t port)
{
  (void) port;
  return 0;
}

void arch_config_clocks(void)
{
}

void arch_config_nvic(void)
{
}

void exti3_isr(void)
{
}

void arch_init_timer(pwm_timer_t timer, uint32_t channel, uint32_t prescaler, uint32_t period)
{
  (void) timer;
  (void) channel;
  (void) prescaler;
  (void) period;
}

void arch_timer_set(uint32_t timer, uint32_t channel, uint32_t value)
{
  (void) timer;
  (void) channel;
  (void) value;
}

/**
 * @brief Configure and enable SPI1 or SPI2. Initialize it with a default
 * byte order, although this can change on a per-message basis.
 *
 * @param port 1 for SPI1 or 2 for SPI2
 * @param byte_order 0 for LSB first, 1 for MSB first
 */
void arch_spi_config(spi_t port)
{
  (void) port;
}


void spi_set_msb(spi_t port)
{
  (void) port;
}

void spi_set_lsb(spi_t port)
{
  (void) port;
}

/**
 * @brief
 */
uint8_t arch_spi_xfer(spi_t port, uint8_t cmd)
{
  (void) port;
  (void) cmd;
  return 0;
}

/**
 * @brief Enable the SPI peripheral.
 *
 * @param port 1 for SPI1, 2 for SPI2
 */
void arch_spi_enable(spi_t port)
{
  (void) port;
}

