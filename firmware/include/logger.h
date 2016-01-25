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
  uint32_t event_id : sizeof(uint32_t) * 8 - 1;
  /** @brief Number of samples associated with this event */
  uint32_t samples;
  /** @brief Size of each sample */
  uint16_t sample_size;
  /** @brief Bitmask representing what features were enabled during logging.
   * Set to CONFIG_FEATURES */
  uint16_t features;
  /** @brief Whether the event is in progress */
  uint8_t in_progress;
  /** @brief Milliseconds since Jan 1, 1970 to the start of this event */
  uint64_t rtc_start;
  /** @brief The address of the previously recorded field */
  uint32_t last_event;

  /** Private fields that will be set to the start offset of this event in 
   * storage */

  /** @brief The start address of the samples. */
  uint32_t __start_address;
  uint32_t __current_address;
  struct {
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

/**
 * @brief Lazily reformat storage; overwrites header.
 */
void logger_format_storage(void);

/**
 * @brief Start a new event, initializing |event| in the process.
 * 
 * @param event An allocated event instance.
 */
void logger_start_event(struct event_header_s *event);

/**
 * @brief Commit |event| to storage.
 */
void logger_end_event(struct event_header_s *event);

/**
 * @brief Read the |n|th (0-indexed) sample fro |event| into the packet pointed
 * to by |dest|
 */
int  logger_read_sample(struct event_header_s *event, uint32_t n, struct sensor_packet_s *dest);

/**
 * @brief Add a sample to an in-progress event.
 */
void logger_write_sample(struct event_header_s *event, struct sensor_packet_s *packet);



/* private declarations below this line ======== */

#ifdef __cplusplus
}
#endif

