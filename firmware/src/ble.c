
/**
 * Implements the communication to the nrf8001 BLE ic.
 *
 */

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/l1/nvic.h>
#include <libopencm3/stm32/exti.h>

#include <pins.h>
#include <config.h>
#include <nrf8001.h>

#include <ble.h>

#define ble_isr exti15_10_isr

static void config_ble_pins(void);
static void config_ble_isr(void);

void ble_isr(void)
{
}

static void config_ble_pins(void)
{
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
