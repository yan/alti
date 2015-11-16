/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifdef OPENCM3

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

#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/adc.h>


void arch_config_uart(usart_t port, int baud);

void pin_set(gpio_t port, pin_t pin)
{
  gpio_set(port, pin);
}

void pin_clear(gpio_t port, pin_t pin)
{
  gpio_clear(port, pin);
}

void pin_toggle(gpio_t port, pin_t pin)
{
  gpio_toggle(port, pin);
}

void pin_config(gpio_t port, pin_t pin, int options)
{
  uint32_t mode;
  gpio_af_mode_t af = 0;
  int should_set = 0;

  if (options == PINMODE_INPUT) {
    mode = GPIO_MODE_INPUT;
  } else if (options == PINMODE_OUTPUT) {
    mode = GPIO_MODE_OUTPUT;
  } else if (options >= PINMODE_AF_1 && options <= PINMODE_AF_8) {
    mode = GPIO_MODE_AF;
    af = options;
    should_set = 1;
  } else {
    mode = 0;
    assert(0);
  }

  gpio_mode_setup(port, mode, GPIO_PUPD_NONE, pin);
  gpio_set_output_options(port, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, pin);

  if (should_set) {
    gpio_set_af(port, af, pin);
  }
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
  rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SYSCFGEN);

  nvic_enable_irq(NVIC_EXTI3_IRQ);
  nvic_set_priority(NVIC_EXTI3_IRQ, BLE_EXTI_ISR_PRIORITY);

  exti_select_source(EXTI3, NRF8001_RDYN_GPIO);
  exti_set_trigger(EXTI3, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI3);
}

void arch_usart_send(usart_t port, uint8_t data)
{
  usart_send_blocking(port, data);
}

uint16_t arch_usart_recv(usart_t port)
{
  return usart_recv_blocking(port);
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

  // RCC_CSR |= RCC_CSR_RTCEN;

  rtc_unlock();

  // Initialize GPIO
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOAEN);

  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOBEN);

  rcc_periph_clock_enable(RCC_GPIOC);
  rcc_peripheral_enable_clock(&RCC_AHBENR, RCC_AHBENR_GPIOCEN);

  g.rcc_clock_freq = rcc_apb2_frequency;
}

void arch_config_uart(usart_t port, int baud)
{
  /* Enable clocks for USART1. */
  if (port == USART1) {
    rcc_periph_clock_enable(RCC_USART1);
  } else if (port == USART2) {
    rcc_periph_clock_enable(RCC_USART2);
  } else {
    assert(0);
  }
  
  /* AF7 == USART? */
  pin_config(UBLOX_UART_GPIO, UBLOX_UART_PINS, PINMODE_AF_7);

  /* Configure UART for ublox max 7 */
  usart_disable(port);

  usart_set_baudrate(port, baud);
  usart_set_databits(port, 8);
  usart_set_stopbits(port, USART_STOPBITS_1);
  usart_set_mode(port, USART_MODE_TX_RX);
  usart_set_parity(port, USART_PARITY_NONE);
  usart_set_flow_control(port, USART_FLOWCONTROL_NONE);

  /* Finally enable the USART. */
  usart_enable(port);
}

void arch_config_io(void)
{
  unsigned i;
  /* Configure SPI for nrf8001 and flash */
  pin_config(BT_STORE_GPIO, BT_STORE_PINS, PINMODE_AF_5);
  arch_spi_config(BT_STORE);

  /* Configure SPI for ms5611 and bmx055 */
  pin_config(SENSORS_GPIO, SENSORS_PINS, PINMODE_AF_5);
  arch_spi_config(SENSORS);
  
  /* Configure UART for ublox GPS */
  arch_config_uart(UBLOX_MAX7_BUS, 9600);

  /* Configure all enable pins */
#define __PIN(name) { name ## _GPIO, name }
  struct {
    gpio_t port;
    pin_t pin;
  } init_pins[] = {
    __PIN(MS5611_EN),
    __PIN(BMX055_EN_ACC),
    __PIN(BMX055_EN_GYRO),
    __PIN(BMX055_EN_MAG),
    __PIN(ADESTO_FLASH_CS),
    __PIN(STATUS_LED),
    __PIN(UBLOX_MAX7_RESET),
    __PIN(WARN_LED_A),
    __PIN(WARN_LED_B),
    // __PIN(PIEZO_EN),
    // __PIN(PIEZO_OUT),
  };
#undef __PIN

  for (i = 0; i < sizeof(init_pins)/sizeof(init_pins[0]); i++) {
    pin_config(init_pins[i].port, init_pins[i].pin, PINMODE_OUTPUT);
    pin_set(init_pins[i].port, init_pins[i].pin);
  }

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

void arch_init_timer(pwm_timer_t timer, uint32_t channel, uint32_t prescaler, uint32_t period)
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
void arch_spi_config(spi_t port)
{
  if (port == SPI1) {
    rcc_periph_clock_enable(RCC_SPI1);
  } else if (port == SPI2) {
    rcc_periph_clock_enable(RCC_SPI2);
  } else {
    assert(0);
  }

  spi_reset(port);
  spi_init_master(port,
                  SPI_CR1_BAUDRATE_FPCLK_DIV_8,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT,
                  SPI_CR1_MSBFIRST);

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
  if (port == SPI1) {
    rcc_periph_clock_enable(RCC_SPI1);
  } else {
    rcc_periph_clock_enable(RCC_SPI2);
  }

  spi_enable(port);
}


void enable_piezo(void)
{

  pin_config(PIEZO_EN_GPIO, PIEZO_EN, PINMODE_OUTPUT);
  pin_set(PIEZO_EN_GPIO, PIEZO_EN);

  pin_config(PIEZO_OUT_GPIO, PIEZO_OUT, PINMODE_AF_2);

  // pin_clear(PIEZO_OUT_GPIO, PIEZO_OUT);

  timer_enable_counter(PIEZO_OUT_TIMER);
}

void disable_piezo(void)
{
  rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
  timer_disable_counter(PIEZO_OUT_TIMER);

  pin_config(PIEZO_EN_GPIO, PIEZO_EN, PINMODE_OUTPUT);
  pin_clear(PIEZO_EN_GPIO, PIEZO_EN);
  pin_clear(PIEZO_OUT_GPIO, PIEZO_OUT);
}

void enable_pulse(void)
{
  pin_config(STATUS_LED_GPIO, STATUS_LED, PINMODE_AF_1);

  timer_enable_counter(STATUS_LED_TIMER);
}


void disable_pulse(void)
{
  timer_disable_counter(STATUS_LED_TIMER);
  pin_config(STATUS_LED_GPIO, STATUS_LED, PINMODE_OUTPUT);
}

void batt_sense_enable(void)
{
#if 0
  rcc_periph_clock_enable(RCC_ADC);

  gpio_mode_setup(BATT_SENSE_GPIO, GPIO_MODE_ANALOG,
                                           GPIO_PUPD_NONE, BATT_SENSE);

  adc_power_off(ADC1);
  adc_set_clk_source(ADC1, ADC_CLKSOURCE_ADC);
  adc_calibrate_start(ADC1);
  adc_calibrate_wait_finish(ADC1);
#endif
}

void batt_sense_disable(void)
{
}

uint16_t batt_sans_sample(void)
{
  return 0;
}

#endif // ifdef OPENCM3
