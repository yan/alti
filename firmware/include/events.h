/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __EVENTS_H
#define __EVENTS_H

#include <stdint.h>

/**
 * @brief All possible events that can be pushed to the main "thread"
 */
typedef enum event_type_e {
  GLOBAL_EVT_RESET,

  GLOBAL_EVT_RCVD_SPI,

  GLOBAL_EVT_AIR_PRESSURE,

  GLOBAL_EVT_NRF8001_FIRST,

  GLOBAL_EVT_NRF8001_RDY,
  GLOBAL_EVT_NRF8001_EVENT,
  GLOBAL_EVT_NRF8001_PIPES_CHANGED,

  GLOBAL_EVT_NRF8001_LAST,

  GLOBAL_EVT_LAST
} event_type_t;


#ifdef TESTING
// typedef uint32_t event_payload_t;
typedef void *event_payload_t;
#else
typedef void *event_payload_t;
#endif

/**
 *
 */
struct global_event_s {
  enum event_type_e type;
  event_payload_t payload;
};

/**
 *
 *
 */
typedef enum global_state_e {
  GLOBAL_STATE_RESET,
  GLOBAL_STATE_BLE_SETUP,
  GLOBAL_STATE_IDLE
} global_state_t;

#endif
