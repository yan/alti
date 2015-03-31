
#ifndef __TASK_ALERT_LED_H
#define __TASK_ALERT_LED_H

#include <FreeRTOS.h>
#include <queue.h>

typedef enum task_alert_event_e {
  ALERT_VIBRATE,

  ALERT_HIGH_INCREASING,
  ALERT_HIGH_DECREASING,

  ALERT_LOW_BLINK,
  ALERT_LOW_PULSE,
  ALERT_LOW_OFF,
  ALERT_LOW_ON,

  ALERT_BUZZ
} task_alert_event_t;


void send_alert(uint16_t type, uint16_t argument);

void task_alert_led(void *p);

#endif
