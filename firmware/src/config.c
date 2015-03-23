#include <config.h>
#include <globals.h>

#include <libopencm3/stm32/rcc.h>

void config_clock(void)
{
  rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_24MHZ]);

  // Initialize GPIO
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOBEN);

  rcc_clock_freq = rcc_apb2_frequency;
}
