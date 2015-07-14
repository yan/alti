
/**
 * 
 */


#include <hal.h>
#include <util.h>
#include <config.h>
#include <pins.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/l1/nvic.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/i2c.h>

void pin_set(int port, int pin)
{
  gpio_set(port, pin);
}

void pin_clear(int port, int pin)
{
  gpio_clear(port, pin);
}

void pin_config(int port, int pin, int options)
{
  uint32_t mode;

  if (options == PINMODE_INPUT) {
    mode = GPIO_MODE_INPUT;
  } else if (options == PINMODE_OUTPUT) {
    mode = GPIO_MODE_OUTPUT;
  } else {
    assert(0);
  }

  gpio_mode_setup(port, mode, GPIO_PUPD_NONE, pin);
}

void spi_config(int port, int options)
{
  (void) port;
  (void) options;
}

void timer_config(int timer, int channel, int options)
{
  (void) timer;
  (void) channel;
  (void) options;
}

void arch_config_ble(void)
{
  rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SYSCFGEN);

  nvic_enable_irq(NVIC_EXTI3_IRQ);
  nvic_set_priority(NVIC_EXTI3_IRQ, BLE_EXTI_ISR_PRIORITY);

  exti_select_source(EXTI3, NRF8001_GPIO);
  exti_set_trigger(EXTI3, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI3);
}

void isr_reset(void)
{
  exti_reset_request(EXTI_PR & EXTI3);
}

void config_isr(int port)
{
  (void) port;
}
