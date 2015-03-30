
#ifndef __TASK_ALERT_LED_H
#define __TASK_ALERT_LED_H

#include <FreeRTOS.h>
#include <queue.h>

typedef enum task_alert_event_e {
  ALERT_EVENT_OFF,
  ALERT_EVENT_ON,
  ALERT_EVENT_BEGIN_PULSING,
  ALERT_EVENT_BLINK_ONCE,
  ALERT_EVENT_BLINK_TWICE,
  ALERT_EVENT_BLINK_THRICE,
} task_alert_event_t;

void task_alert_led(void *p);

#endif
