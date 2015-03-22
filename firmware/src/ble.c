
/**
 * Implements the communication to the nrf8001 BLE ic.
 *
 */

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/l1/nvic.h>
#include <libopencm3/stm32/exti.h>

#include <FreeRTOS.h>
#include <queue.h>

#include <pins.h>
#include <config.h>
#include <nrf8001.h>

#include <task_status_led.h>
#include <ble.h>

#define ble_isr exti15_10_isr

static void config_ble_pins(void);
static void config_ble_isr(void);

void ble_isr(void)
{
  BaseType_t higher;
  enum task_status_event_e evt = STATUS_EVENT_BLINK_THRICE;

  exti_reset_request(EXTI_PR & EXTI11);

  xQueueSendFromISR(status_queue_g, &evt, &higher);
  portYIELD_FROM_ISR(higher);
}

static void config_ble_pins(void)
{
  /* Configure alternate function for SPI pins */
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, NRF8001_PINS);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, NRF8001_PINS);
  gpio_set_af(GPIOB, GPIO_AF5, NRF8001_PINS);

  /* We'll use RDYN pin to interrupt*/
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,   NRF8001_REQN);
  gpio_mode_setup(GPIOB, GPIO_MODE_INPUT,  GPIO_PUPD_PULLUP, NRF8001_RDYN);
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,   NRF8001_RST);

  gpio_set(GPIOB, NRF8001_RST);

  /* Configure LD3 to toggle on ISR */
  rcc_periph_clock_enable(RCC_GPIOB);
  gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GREEN_LED);
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GREEN_LED);
  gpio_set(GPIOB, GREEN_LED);

  rcc_periph_clock_enable(RCC_SPI2);
  spi_reset(SPI2);

  spi_init_master(SPI2,
                  SPI_CR1_BAUDRATE_FPCLK_DIV_16,
                  SPI_CR1_CPOL_CLK_TO_0_WHEN_IDLE,
                  SPI_CR1_CPHA_CLK_TRANSITION_1,
                  SPI_CR1_DFF_8BIT,
                  SPI_CR1_LSBFIRST);

  spi_enable_software_slave_management(SPI2);
  spi_enable_ss_output(SPI2);
  // spi_set_nss_high(SPI2);

  spi_enable(SPI2);

}

static void config_ble_isr(void)
{

  nvic_enable_irq(NVIC_EXTI15_10_IRQ);
  nvic_set_priority(NVIC_EXTI15_10_IRQ, BLE_EXTI_ISR_PRIORITY);

  exti_select_source(EXTI11,  GPIOB);
  exti_set_trigger(EXTI11,    EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI11);

}

void config_ble(void)
{
  config_ble_pins();
  config_ble_isr();
}
