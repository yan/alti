#include <config.h>
#include <globals.h>

#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <pins.h>
#include <spi.h>

void config_clock(void)
{
  rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_24MHZ]);
  // rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_MSI_RAW_4MHZ]);

  rcc_periph_clock_enable(RCC_PWR);
  pwr_disable_backup_domain_write_protect();


  rcc_osc_on(LSE);
  rcc_wait_for_osc_ready(LSE);

  rcc_rtc_select_clock(RCC_CSR_RTCSEL_LSE);

  //RCC_CSR |= RCC_CSR_RTCEN;

  //rtc_unlock();

  // Initialize GPIO
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);

  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOBEN);

  g.rcc_clock_freq = rcc_apb2_frequency;
}

void config_nvic(void)
{
  scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB);
}

void config_io(void)
{

  /* Configure SPI for nrf8001 and flash */
  rcc_periph_clock_enable(RCC_SPI1);
  gpio_mode_setup(SPI1_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_PINS);
  gpio_set_output_options(SPI1_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SPI1_PINS);
  gpio_set_af(SPI1_GPIO, GPIO_AF5, SPI1_PINS);
  aero_spi_config(1, BYTEORDER_LSB);

  /* Configure SPI for ms5611 and bmx055 */
  rcc_periph_clock_enable(RCC_SPI2);
  gpio_mode_setup(SPI2_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI2_PINS);
  gpio_set_output_options(SPI2_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SPI2_PINS);
  gpio_set_af(SPI2_GPIO, GPIO_AF5, SPI2_PINS);
  aero_spi_config(2, BYTEORDER_MSB);

  /* Configure all enable pins */
  gpio_mode_setup(MS5611_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MS5611_EN);
  gpio_set_output_options(MS5611_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, MS5611_EN);
  gpio_set(MS5611_GPIO, MS5611_EN);

  gpio_mode_setup(BMX055_EN_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BMX055_EN_PINS);
  gpio_set_output_options(BMX055_EN_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, BMX055_EN_PINS);
  gpio_set(BMX055_EN_GPIO, BMX055_EN_PINS);

  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, STATUS_LED);
  gpio_set_output_options(STATUS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, STATUS_LED);
  gpio_set(STATUS_GPIO, STATUS_LED);
}

void config_globals(void)
{
  g.flash_buffer.lock = (SemaphoreHandle_t) xSemaphoreCreateMutex();
}
