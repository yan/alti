/**
 *
 *
 *
 */

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>

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
static void init_status_timer(void);
static void init_piezo_timer(void);


static void init_status_timer(void)
{
  rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM2EN);

  timer_reset(STATUS_LED_TIMER);

  timer_set_mode(STATUS_LED_TIMER, TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

  timer_direction_up(STATUS_LED_TIMER);
  timer_continuous_mode(STATUS_LED_TIMER);
  timer_set_prescaler(STATUS_LED_TIMER, 11);
  timer_set_oc_mode(STATUS_LED_TIMER, STATUS_LED_CHANNEL, TIM_OCM_PWM1);
  timer_enable_oc_output(STATUS_LED_TIMER, STATUS_LED_CHANNEL);
  timer_set_oc_value(STATUS_LED_TIMER, STATUS_LED_CHANNEL, 0);
  timer_set_oc_idle_state_set(STATUS_LED_TIMER, STATUS_LED_CHANNEL);
  timer_set_period(STATUS_LED_TIMER, 1000);
}

static void init_piezo_timer(void)
{
  rcc_peripheral_enable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);

  timer_reset(PIEZO_OUT_TIMER);

  timer_set_mode(PIEZO_OUT_TIMER, TIM_CR1_CKD_CK_INT,
                 TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

  timer_direction_up(PIEZO_OUT_TIMER);
  timer_continuous_mode(PIEZO_OUT_TIMER);
  timer_set_prescaler(PIEZO_OUT_TIMER, 5);
  timer_set_oc_mode(PIEZO_OUT_TIMER, PIEZO_OUT_CHANNEL, TIM_OCM_PWM1);
  timer_enable_oc_output(PIEZO_OUT_TIMER, PIEZO_OUT_CHANNEL);
  timer_set_period(PIEZO_OUT_TIMER, 1000);
  timer_set_oc_idle_state_set(PIEZO_OUT_TIMER, PIEZO_OUT_CHANNEL);

  timer_set_oc_value(PIEZO_OUT_TIMER, PIEZO_OUT_CHANNEL, 500);
}

static void task_alert_config(void)
{
  // int ksps = 100, period = 12000/ksps, half_period = period / 2;
  const int USE_PIEZO = 0;

  // Initialize timer
  init_status_timer();
  enable_pulse();

  if (USE_PIEZO) {
    init_piezo_timer();
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

  timer_set_oc_value(STATUS_LED_TIMER, STATUS_LED_CHANNEL, res);
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
  gpio_set(PIEZO_GPIO, PIEZO_EN);

  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_AF, GPIO_PUPD_NONE, PIEZO_OUT);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_OUT);
  //gpio_set_af(PIEZO_GPIO, PIEZO_OUT_AF, PIEZO_OUT);

  gpio_clear(PIEZO_GPIO, PIEZO_OUT);

  //timer_enable_counter(PIEZO_OUT_TIMER);
}

static void disable_piezo(void)
{
  rcc_peripheral_disable_clock(&RCC_APB1ENR, RCC_APB1ENR_TIM4EN);
  timer_disable_counter(PIEZO_OUT_TIMER);

  gpio_mode_setup(PIEZO_GPIO, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PIEZO_EN);
  gpio_set_output_options(PIEZO_GPIO, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PIEZO_EN);
  gpio_clear(PIEZO_GPIO, PIEZO_EN);

  gpio_clear(PIEZO_GPIO, PIEZO_OUT);
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

#if 0
  for (;;) {
    status = xQueueReceive(g.alert_queue_g, &received, delay);

    gpio_toggle(STATUS_GPIO, STATUS_LED);
    delay_ms(50);
    gpio_toggle(STATUS_GPIO, STATUS_LED);
  }
#endif

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
        gpio_clear(STATUS_GPIO, STATUS_LED);
        state = TASK_ALERT_STATE_OFF;
        break;
        
      case ALERT_LOW_ON:
        disable_pulse();
        gpio_set(STATUS_GPIO, STATUS_LED);
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
          gpio_clear(STATUS_GPIO, STATUS_LED);
          delay_ms(250);
        }
        
        if (state == TASK_ALERT_STATE_ON) {
          gpio_set(STATUS_GPIO, STATUS_LED);
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

