
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

#include <globals.h>
#include <pins.h>
#include <util.h>
#include <task_status_led.h>

static void task_status_config(void);
static uint32_t easing_f(uint32_t);
static void step_pulse(void);
static void enable_pulse(void);
static void disable_pulse(void);

static void task_status_config(void)
{
  // int ksps = 100, period = 12000/ksps, half_period = period / 2;

  // Initialize GPIO
  // rcc_periph_clock_enable(RCC_GPIOB);

  // Initialize timer
  rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);

  timer_reset(TIM4);

  timer_set_mode(BLUE_LED_TIMER, TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

  timer_direction_up(BLUE_LED_TIMER);
  timer_continuous_mode(BLUE_LED_TIMER);
  timer_set_prescaler(BLUE_LED_TIMER, 11);
  timer_set_oc_mode(BLUE_LED_TIMER, BLUE_LED_CHANNEL, TIM_OCM_PWM1);
  timer_enable_oc_output(BLUE_LED_TIMER, BLUE_LED_CHANNEL);
  timer_set_oc_value(BLUE_LED_TIMER, BLUE_LED_CHANNEL, 0);
  timer_set_oc_idle_state_set(BLUE_LED_TIMER, BLUE_LED_CHANNEL);
  timer_set_period(BLUE_LED_TIMER, 1000);

  enable_pulse();
}

static uint32_t easing_f(uint32_t offset)
{
  uint32_t x = offset >> 5;
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

  timer_set_oc_value(BLUE_LED_TIMER, BLUE_LED_CHANNEL, res);
}

static void enable_pulse(void)
{
  gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, BLUE_LED);            
  gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, BLUE_LED);
  gpio_set_af(GPIOB, BLUE_LED_AF, BLUE_LED);

  timer_enable_counter(BLUE_LED_TIMER);
}

static void disable_pulse(void)
{
  timer_disable_counter(BLUE_LED_TIMER);
  gpio_mode_setup(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, BLUE_LED);
}

enum task_status_state_e {
  TASK_STATUS_STATE_OFF,
  TASK_STATUS_STATE_ON,
  TASK_STATUS_STATE_BLINKING,
  TASK_STATUS_STATE_PULSING
};

void task_status_led(void *p)
{
  (void) p;
  BaseType_t status = 0,
             state = TASK_STATUS_STATE_PULSING,
             delay = 10 / portTICK_PERIOD_MS;
  enum task_status_event_e received_event;

  task_status_config();

  for (;;) {
    status = xQueueReceive(status_queue_g, &received_event, delay);

    if (status != pdPASS) {
      if (state == TASK_STATUS_STATE_PULSING) {
        step_pulse();
      }
      continue;
    }

    switch (received_event) {
      case STATUS_EVENT_OFF:
        disable_pulse();
        gpio_clear(GPIOB, BLUE_LED);
        state = TASK_STATUS_STATE_OFF;
        break;
        
      case STATUS_EVENT_ON:
        disable_pulse();
        gpio_set(GPIOB, BLUE_LED);
        state = TASK_STATUS_STATE_ON;
        break;

      case STATUS_EVENT_BEGIN_PULSING:
        enable_pulse();
        state = TASK_STATUS_STATE_PULSING;
        break;

      case STATUS_EVENT_BLINK_ONCE:
      case STATUS_EVENT_BLINK_TWICE:
      case STATUS_EVENT_BLINK_THRICE: {
        int blink_count = received_event - STATUS_EVENT_BLINK_ONCE + 1;
        disable_pulse();
        while (blink_count-- > 0) {
          gpio_toggle(GPIOB, BLUE_LED);
          delay_ms(250);
          gpio_clear(GPIOB, BLUE_LED);
          delay_ms(250);
        }
        
        if (state == TASK_STATUS_STATE_ON) {
          gpio_set(GPIOB, BLUE_LED);
        } else if (state == TASK_STATUS_STATE_PULSING) {
          enable_pulse();
        }
        // If the status was off, just leave as is since we just turned
      }
        break;
    }
  }

}

