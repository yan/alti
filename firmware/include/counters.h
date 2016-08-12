
#ifndef __COUNTERS_H
#define __COUNTERS_H

#include <features.h>

#if CONFIG_USE_COUNTERS

enum counter_name_e {
  COUNTER_EVENTS_PROCESSED,
  COUNTER_EVENTS_RECEIVED,
  COUNTER_BLE_RECEIVED,
  COUNTER_BLE_SEMAPHORES,
  COUNTER_GIVEN,
  COUNTER_BLE_ISR,
  COUNTER_CREDITS_RECEIVED,
  COUNTER_CREDITS_TAKEN,

  COUNTER_LAST
};

struct counters_s {
  uint32_t vals[COUNTER_LAST];
};


#  define counter_add_event(evt) do {    \
    portENTER_CRITICAL();                \
    g.counters.vals[evt] += 1;           \
    portEXIT_CRITICAL();                 \
  } while(0)

#else // CONFIG_USE_COUNTERS
#  define counter_add_event(evt)
#endif // CONFIG_USE_COUNTERS
#endif // __COUNTERS_H
