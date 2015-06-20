
/**
 * Implements the communication to the nrf8001 BLE ic.
 *
 */

#include <stdio.h>

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/l1/nvic.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pins.h>
#include <config.h>
#include <nrf8001.h>
#include <globals.h>
#include <events.h>
#include <util.h>

//#include <task_alert.h>
#include <task_ble.h>
#include <ble.h>
#include <services.h>
#include <spi.h>

#define ble_isr exti3_isr

static void config_ble_pins(void);
static void config_ble_isr(void);
static void nrf8001_reset(void);

int g_isr_hit = 0;

void ble_isr(void)
{
  BaseType_t higher = pdFALSE;
  enum event_type_e evt = GLOBAL_EVT_NRF8001_RDY;

  exti_reset_request(EXTI_PR & EXTI3);

  ++g_isr_hit;

  xQueueSendToFrontFromISR(g.main_queue_g, &evt, &higher);

  portYIELD_FROM_ISR(higher);
}

static void config_ble_pins(void)
{
  gpio_mode_setup(NRF8001_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,   NRF8001_REQN);
  gpio_mode_setup(NRF8001_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE,   NRF8001_RST);

  gpio_set(NRF8001_GPIO, NRF8001_REQN);
  gpio_set(NRF8001_GPIO, NRF8001_RST);
}

static void config_ble_isr(void)
{
/* Remove #if block to pulse a pin to detect this on a scope */
#if 0
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO2);
  gpio_clear(GPIOB, GPIO2);
  gpio_set(GPIOB, GPIO2);
  gpio_clear(GPIOB, GPIO2);
#endif

  rcc_peripheral_enable_clock(&RCC_APB2ENR, RCC_APB2ENR_SYSCFGEN);

  /* We'll use RDYN pin to interrupt*/
  gpio_mode_setup(NRF8001_GPIO, GPIO_MODE_INPUT, GPIO_PUPD_NONE, NRF8001_RDYN);

  nvic_enable_irq(NVIC_EXTI3_IRQ);
  nvic_set_priority(NVIC_EXTI3_IRQ, BLE_EXTI_ISR_PRIORITY);

  exti_select_source(EXTI3, NRF8001_GPIO);
  exti_set_trigger(EXTI3, EXTI_TRIGGER_FALLING);
  exti_enable_request(EXTI3);
}

static void nrf8001_reset(void)
{
  int i = 0;
  gpio_set(NRF8001_GPIO, NRF8001_RST);
  gpio_clear(NRF8001_GPIO, NRF8001_RST);
  // Spec calls for the line to be low at least 200ns, set it low and busy wait
  for (; i < 30; i++);
  gpio_set(NRF8001_GPIO, NRF8001_RST);
}

int g_sent = 0;

/**
 *
 */
void config_ble(void)
{
  config_ble_pins();
  config_ble_isr();
  nrf8001_reset();

  //gpio_clear(NRF8001_GPIO, NRF8001_REQN);
}
