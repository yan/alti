/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#include <hal.h>
#include <globals.h>
#include <pins.h>
#include <util.h>
#include <task_alert.h>

static void task_alert_config(void);
static uint32_t easing_f(uint32_t);
static void step_pulse(void);
static void enable_pulse(void);
static void disable_pulse(void);
static void enable_piezo(void);
static void disable_piezo(void);




static void task_alert_config(void)
{
  // int ksps = 100, period = 12000/ksps, half_period = period / 2;
  const int USE_PIEZO = 0;

  // Initialize timer
  arch_init_timer(STATUS_LED_TIMER, STATUS_LED_CHANNEL, 11, 1000);
  enable_pulse();

  if (USE_PIEZO) {
    arch_init_timer(PIEZO_OUT_TIMER, PIEZO_OUT_CHANNEL, 5, 1000);
    enable_piezo();
  } else {
    disable_piezo();
  }

}

static uint32_t easing_f(uint32_t offset)
{
  const uint32_t x = offset >> 5;
  return x * x;
}

static void step_pulse(void)
{
  static int value = 0, direction = 10;
  int res;

  if (value >= 1000) {
    direction = -10;
  } else if (value <= 0) {
    direction = 10;
  }

  value += direction;
  res = easing_f(value);

  arch_timer_set(STATUS_LED_TIMER, STATUS_LED_CHANNEL, res);
}

static void enable_pulse(void)
{
  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, STATUS_LED);            
  gpio_set_output_options(STATUS_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, STATUS_LED);
  gpio_set_af(STATUS_GPIO, STATUS_LED_AF, STATUS_LED);

  timer_enable_counter(STATUS_LED_TIMER);
}

static void enable_piezo(void)
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

static void disable_piezo(void)
{
  rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
  timer_disable_counter(PIEZO_OUT_TIMER);

  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIEZO_EN);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_EN);
  pin_clear(PIEZO_GPIO, PIEZO_EN);

  pin_clear(PIEZO_GPIO, PIEZO_OUT);
}

static void disable_pulse(void)
{
  timer_disable_counter(STATUS_LED_TIMER);
  gpio_mode_setup(STATUS_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, STATUS_LED);
}

enum task_alert_state_e {
  TASK_ALERT_STATE_OFF,
  TASK_ALERT_STATE_ON,
  TASK_ALERT_STATE_BLINKING,
  TASK_ALERT_STATE_PULSING
};

void send_alert(uint16_t type, uint16_t argument)
{
  uint32_t to_send = argument;
  to_send |= type << 16;

  xQueueSend(g.alert_queue_g, &to_send, portMAX_DELAY);
}

void task_alert_led(void *p)
{
  (void) p;
  BaseType_t status = 0,
             state = TASK_ALERT_STATE_PULSING,
             delay = 10 / portTICK_PERIOD_MS;
  enum task_alert_event_e received_event;
  uint32_t received;
  uint16_t argument;

  task_alert_config();

  for (;;) {
    status = xQueueReceive(g.alert_queue_g, &received, delay);

    if (status != pdPASS) {
      if (state == TASK_ALERT_STATE_PULSING) {
        step_pulse();
      }
      continue;
    }

    received_event = (received >> 16) & 0xFFFF;
    argument = received & 0xFFFF;

    switch (received_event) {
      case ALERT_LOW_OFF:
        disable_pulse();
        pin_clear(STATUS_GPIO, STATUS_LED);
        state = TASK_ALERT_STATE_OFF;
        break;
        
      case ALERT_LOW_ON:
        disable_pulse();
        pin_set(STATUS_GPIO, STATUS_LED);
        state = TASK_ALERT_STATE_ON;
        break;

      case ALERT_LOW_PULSE:
        enable_pulse();
        state = TASK_ALERT_STATE_PULSING;
        break;

      case ALERT_LOW_BLINK: {
        disable_pulse();
        while (argument-- > 0) {
          gpio_toggle(STATUS_GPIO, STATUS_LED);
          delay_ms(250);
          pin_clear(STATUS_GPIO, STATUS_LED);
          delay_ms(250);
        }
        
        if (state == TASK_ALERT_STATE_ON) {
          pin_set(STATUS_GPIO, STATUS_LED);
        } else if (state == TASK_ALERT_STATE_PULSING) {
          enable_pulse();
        }
        // If the status was off, just leave as is since we just turned
      }
        break;

      case ALERT_VIBRATE: 
        /** TODO **/
        break;

      case ALERT_HIGH_INCREASING:
        /** TODO **/
        break;

      case ALERT_HIGH_DECREASING:
        /** TODO **/
        break;

      case ALERT_BUZZ:
        /** TODO **/
        break;
    }
  }

}

