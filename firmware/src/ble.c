
/**
 * Implements the communication to the nrf8001 BLE ic.
 *
 */

#include <stdio.h>

#include <hal.h>

#include <FreeRTOS.h>
#include <queue.h>
#include <task.h>

#include <pins.h>
#include <config.h>
#include <nrf8001.h>
#include <globals.h>
#include <events.h>
#include <util.h>

#include <task_ble.h>
#include <ble.h>
#include <services.h>
#include <spi.h>

static void config_ble_pins(void);
static void config_ble_isr(void);
static void nrf8001_reset(void);
void ble_isr(void);

int g_isr_hit = 0;

void ble_isr(void)
{
  BaseType_t higher = pdFALSE;
  enum event_type_e evt = GLOBAL_EVT_NRF8001_RDY;

  isr_reset();

  ++g_isr_hit;

  xQueueSendToFrontFromISR(g.main_queue_g, &evt, &higher);

  portYIELD_FROM_ISR(higher);
}

static void config_ble_pins(void)
{
  pin_config(NRF8001_GPIO, NRF8001_REQN, PINMODE_OUTPUT);
  pin_config(NRF8001_GPIO, NRF8001_RST, PINMODE_OUTPUT);

  pin_set(NRF8001_GPIO, NRF8001_REQN);
  pin_set(NRF8001_GPIO, NRF8001_RST);
}

static void config_ble_isr(void)
{
  arch_config_ble();

  /* We'll use RDYN pin to interrupt*/
  pin_config(NRF8001_GPIO, NRF8001_RDYN, PINMODE_INPUT);

}

static void nrf8001_reset(void)
{
  int i = 0;
  pin_set(NRF8001_GPIO, NRF8001_RST);
  pin_clear(NRF8001_GPIO, NRF8001_RST);
  // Spec calls for the line to be low at least 200ns, set it low and busy wait
  for (; i < 30; i++);
  pin_set(NRF8001_GPIO, NRF8001_RST);
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

  //pin_clear(NRF8001_GPIO, NRF8001_REQN);
}
