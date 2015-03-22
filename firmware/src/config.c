#include <config.h>

#include <libopencm3/stm32/rcc.h>

void config_clock(void)
{
  rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_24MHZ]);
}
