
#ifndef __TASK_STATUS_LED_H
#define __TASK_STATUS_LED_H

#include <FreeRTOS.h>
#include <queue.h>

/** @brief The global queue that status events get pushed to */
extern QueueHandle_t status_queue_g;

typedef enum task_status_event_e {
  STATUS_EVENT_OFF,
  STATUS_EVENT_ON,
  STATUS_EVENT_BEGIN_PULSING,
  STATUS_EVENT_BLINK_ONCE,
  STATUS_EVENT_BLINK_TWICE,
  STATUS_EVENT_BLINK_THRICE,
} task_status_event_t;

void task_status_led(void *p);

#endif
