#include <globals.h>

/** @brief Type of the sentinel value. (only here for use in testing) */
typedef uint32_t sentinel_t;

/** @brief Value to prepend events with to detect wrap-around */
#define SENTINEL_VALUE       ((sentinel_t) 0xAABBCCDD)

/** @brief The address of the storage header */
const uint32_t HEADER_ADDR = 0x00;

/** @brief The address of the first non-header data */
const uint32_t DATA_START_ADDR = sizeof(struct storage_header_s);

#if TESTING
#  define TAKE_SEMPHR
#  define GIVE_SEMPHR
#else
#  define TAKE_SEMPHR    xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY)
#  define GIVE_SEMPHR    xSemaphoreGive(g.flash_buffer.lock)
#endif // TESTING

struct stored_event_header_s {
  sentinel_t sentinel;
  struct event_s header;
};

#ifndef EVENT_HEADER_SIZE
#  define EVENT_HEADER_SIZE sizeof(struct event_header_s) /*offsetof(struct event_header_s, _prv)*/
#endif

#define STORED_EVENT_HEADER_SIZE (EVENT_HEADER_SIZE + sizeof(sentinel_t))
  
#define TOTAL_STORED_EVENT_SIZE(ev)  \
  (((ev).header.samples * (ev).header.sample_size) + STORED_EVENT_HEADER_SIZE)

