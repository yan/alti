/**
 * Copyright 2015 Yan Ivnitskiy
 */

#pragma once

#include <sample.h>
// #include <flash.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The header for an 'event'. Likely a jump.
 */
struct event_header_s {
  /** @brief The unique id of this event */
  uint32_t event_id;
  /** @brief Number of samples associated with this event */
  uint32_t samples;
  /** @brief Size of each sample */
  uint16_t sample_size;
  /** @brief Bitmask representing what features were enabled during logging.
   * Set to CONFIG_FEATURES */
  uint16_t features;
  /** @brief Milliseconds since Jan 1, 1970 to the start of this event */
  uint64_t rtc_start;

  /** @brief Private field that will be set to the start offset of this event
   * in storage
   */
  uint32_t __start_address;
  uint32_t __current_address;
  struct {
    uint16_t __started : 1;
    uint16_t __finished_logging : 1;
    uint16_t __written : 1;
  };
};

#ifndef offsetof
#define offsetof(type, member) ( (size_t) & ((type*)0) -> member )
#endif

#define EVENT_HEADER_SIZE offsetof(struct event_header_s, __start_address)
  
#define TOTAL_EVENT_SIZE(ev)  \
  (((ev).samples * (ev).sample_size) + EVENT_HEADER_SIZE)

void logger_format_storage(void);

void logger_start_event(struct event_header_s *event);

void logger_end_event(struct event_header_s *event);

void logger_read_sample(struct event_header_s *event, uint32_t n, struct sensor_packet_s *dest);

void logger_write_sample(struct event_header_s *event, struct sensor_packet_s *packet);



/* private declarations below this line ======== */

/** @brief Type of the sentinel value. (only here for use in testing) */
typedef uint32_t sentinel_t;

/** @brief Value to prepend events with to detect wrap-around */
const sentinel_t SENTINEL_VALUE = 0xAABBCCDD;

/**
 * @brief The first page of storage has some basic info.
 *
 */
struct storage_header_s {
  /** @brief Number of events stored in storage */
  uint32_t events;

  /** @brief The offset of the first free byte */
  uint32_t free_offset;

  /** @brief */
  uint32_t last_event;
};
#ifdef __cplusplus
}
#endif

