/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifdef STM32_STDPERIPH_LIB

#include <hal.h>
#include <nrf8001.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <pins.h>
#include <spi.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <stm32l1xx_conf.h>

void pin_set(gpio_t port, pin_t pin)
{
  GPIO_SetBits(port, pin);
}

void pin_clear(gpio_t port, pin_t pin)
{
  GPIO_ResetBits(port, pin);
}

void pin_toggle(gpio_t port, pin_t pin)
{
  GPIO_ToggleBits(port, pin);
}

void pin_config(gpio_t port, pin_t pin, int options)
{
  GPIO_InitTypeDef init = {
    .GPIO_OType = GPIO_OType_PP,
    .GPIO_Pin = pin,
    .GPIO_Mode = options == PINMODE_INPUT ? GPIO_Mode_IN : GPIO_Mode_OUT,
    .GPIO_PuPd = GPIO_PuPd_NOPULL,
    .GPIO_Speed = GPIO_Speed_10MHz
  };

  if (options != PINMODE_INPUT && options != PINMODE_OUTPUT) {
    assert(0);
  }

  GPIO_Init(port, &init);
}

void spi_config(spi_t port, int options)
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
  // rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SYSCFGEN);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  NVIC_EnableIRQ(EXTI3_IRQn);
  NVIC_SetPriority(EXTI3_IRQn, BLE_EXTI_ISR_PRIORITY);

  EXTI_InitTypeDef ble_isr_init = {
    .EXTI_Line = EXTI_Line3,
    .EXTI_LineCmd = ENABLE,
    .EXTI_Mode = EXTI_Mode_Interrupt,
    .EXTI_Trigger = EXTI_Trigger_Falling
  };

  EXTI_Init(&ble_isr_init);
}

void arch_config_clocks(void)
{
  /* Configure main system clock */
  rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_HSI_PLL_24MHZ]);
  // rcc_clock_setup_pll(&clock_config[CLOCK_VRANGE1_MSI_RAW_4MHZ]);

  rcc_periph_clock_enable(RCC_PWR);
  pwr_disable_backup_domain_write_protect();

  /* Enable LSE xtal */
  rcc_osc_on(LSE);
  rcc_wait_for_osc_ready(LSE);

  /* Make the RTC use the LSE */
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

void arch_config_io(void)
{
  /* Configure SPI for nrf8001 and flash */
  rcc_periph_clock_enable(RCC_SPI1);
  gpio_mode_setup(SPI1_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_PINS);
  gpio_set_output_options(SPI1_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SPI1_PINS);
  gpio_set_af(SPI1_GPIO, GPIO_AF5, SPI1_PINS);
  arch_spi_config(1, BYTEORDER_LSB);

  /* Configure SPI for ms5611 and bmx055 */
  rcc_periph_clock_enable(RCC_SPI2);
  gpio_mode_setup(SPI2_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI2_PINS);
  gpio_set_output_options(SPI2_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, SPI2_PINS);
  gpio_set_af(SPI2_GPIO, GPIO_AF5, SPI2_PINS);
  arch_spi_config(2, BYTEORDER_MSB);

  /* Configure all enable pins */
  gpio_mode_setup(MS5611_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MS5611_EN);
  gpio_set_output_options(MS5611_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, MS5611_EN);
  pin_set(MS5611_GPIO, MS5611_EN);

  gpio_mode_setup(BMX055_EN_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BMX055_EN_PINS);
  gpio_set_output_options(BMX055_EN_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, BMX055_EN_PINS);
  pin_set(BMX055_EN_GPIO, BMX055_EN_PINS);

  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, STATUS_LED);
  gpio_set_output_options(STATUS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, STATUS_LED);
  pin_set(STATUS_GPIO, STATUS_LED);
}

void config_isr(int port)
{
  (void) port;
}


void arch_config_nvic(void)
{
  scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP16_NOSUB);
}

void exti3_isr(void)
{
  exti_reset_request(EXTI_PR & EXTI3);

  nrf8001_isr();

}

void arch_init_timer(uint32_t timer, uint32_t channel, uint32_t prescaler, uint32_t period)
{
  rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);

  timer_reset(timer);

  timer_set_mode(timer, TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

  timer_direction_up(timer);
  timer_continuous_mode(timer);
  timer_set_prescaler(timer, prescaler);
  timer_set_oc_mode(timer, channel, TIM_OCM_PWM1);
  timer_enable_oc_output(timer, channel);
  timer_set_oc_value(timer, channel, 0);
  timer_set_oc_idle_state_set(timer, channel);
  timer_set_period(timer, period);

}

void arch_timer_set(uint32_t timer, uint32_t channel, uint32_t value)
{
  /** TODO: move this to HAL */
  timer_set_oc_value(timer, channel, value);
}

/**
 * @brief Configure and enable SPI1 or SPI2. Initialize it with a default
 * byte order, although this can change on a per-message basis.
 *
 * @param port 1 for SPI1 or 2 for SPI2
 * @param byte_order 0 for LSB first, 1 for MSB first
 */
void arch_spi_config(spi_t port, uint16_t byte_order)
{
  port = ((port == 1) ? SPI1 : SPI2);

  byte_order = !!byte_order;

  spi_reset(port);
  spi_init_master(port,
                  SPI_CR1_BAUDRATE_FPCLK_DIV_4,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT,
                  byte_order ? SPI_CR1_MSBFIRST : SPI_CR1_LSBFIRST);

  spi_enable_software_slave_management(port);
  spi_disable_ss_output(port);
  spi_set_nss_high(port);

  spi_enable(port);
}


void spi_set_msb(spi_t port)
{
  spi_send_msb_first(port);
}

void spi_set_lsb(spi_t port)
{
  spi_send_lsb_first(port);
}

/**
 * @brief
 */
uint8_t arch_spi_xfer(spi_t port, uint8_t cmd)
{
  return spi_xfer(port, cmd);
}

/**
 * @brief Enable the SPI peripheral.
 *
 * @param port 1 for SPI1, 2 for SPI2
 */
void arch_spi_enable(spi_t port)
{
  rcc_periph_clock_enable((port == 2) ? RCC_SPI2 : RCC_SPI1);

  port = ((port == 2) ? SPI2 : SPI1);
  spi_enable(port);
}


void enable_piezo(void)
{
  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIEZO_EN);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_EN);
  pin_set(PIEZO_GPIO, PIEZO_EN);

  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, PIEZO_OUT);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_OUT);
  //gpio_set_af(PIEZO_GPIO, PIEZO_OUT_AF, PIEZO_OUT);

  pin_clear(PIEZO_GPIO, PIEZO_OUT);

  //timer_enable_counter(PIEZO_OUT_TIMER);
}

void disable_piezo(void)
{
  rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
  timer_disable_counter(PIEZO_OUT_TIMER);

  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIEZO_EN);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_EN);
  pin_clear(PIEZO_GPIO, PIEZO_EN);

  pin_clear(PIEZO_GPIO, PIEZO_OUT);
}

void enable_pulse(void)
{
  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, STATUS_LED);
  gpio_set_output_options(STATUS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, STATUS_LED);
  gpio_set_af(STATUS_GPIO, STATUS_LED_AF, STATUS_LED);

  timer_enable_counter(STATUS_LED_TIMER);
}


void disable_pulse(void)
{
  timer_disable_counter(STATUS_LED_TIMER);
  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, STATUS_LED);
}


#endif // ifdef STM32_STDPERIPH_LIB

