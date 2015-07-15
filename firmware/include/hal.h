/**
 * The hardware abstraction layer functions for micros
 */

#include <spi.h>

enum pinmode_e {
  PINMODE_OUTPUT,
  PINMODE_INPUT,
};

/** XXX move this to a arch-specific header */
#ifdef STM32L1
#  include <libopencm3/stm32/spi.h>
#endif

void pin_set(int port, int pin);

void pin_clear(int port, int pin);

void pin_config(int port, int pin, int options);

void spi_config(int port, int options);

void timer_config(int timer, int channel, int options);

void config_isr(int port);

void arch_config_ble(void);

void arch_config_clocks(void);

void arch_config_io(void);

void arch_config_nvic(void);
