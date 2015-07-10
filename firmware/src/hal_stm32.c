
/**
 * 
 */


#include <hal.h>
#include <util.h>

#include <libopencm3/stm32/gpio.h>

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

void config_isr(int port)
{
  (void) port;
}
