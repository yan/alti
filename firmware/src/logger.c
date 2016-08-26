/**
 *
 */

#include <string.h>
#include <unistd.h>
#include <globals.h>

#include <rtos.h>
#include <logger.h>
#include <buffered_io.h>


#ifdef __cplusplus

#if TESTING
#include <stdio.h>
#endif

extern "C" {
#endif

#include "logger_private.h"

static inline uint32_t logger_get_last_event(void)
{
  struct storage_header_s *storage_header =
    (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

  assert(storage_header->last_event < STORAGE_SIZE);

  return storage_header->last_event;
}

static inline void logger_init_event(struct event_s *event, uint32_t event_address)
{
  event->header.samples = 0;
  event->header.sample_size = sizeof(struct sensor_packet_s);
  event->header.features = CONFIG_FEATURES;
  event->header.in_progress = 1;
  event->header.rtc_start = 0; // TODO: Get the actual time stamp

  /* Set the private fields */
  event->_private.start_address = event_address;
  event->_private.current_address = event_address + STORED_EVENT_HEADER_SIZE;
  event->_private.finished_logging = 0;
  event->_private.written = 0;
}

static inline void logger_write_event(struct event_s *event, uint32_t address) {
  struct stored_event_header_s stored_event;

  stored_event.sentinel = SENTINEL_VALUE;
  stored_event.header = *event;

  buffered_write_wrapped(address, (const uint8_t*) &stored_event,
      STORED_EVENT_HEADER_SIZE, DATA_START_ADDR);
}

static inline int logger_read_event(uint32_t address, struct event_s *event) {
  struct stored_event_header_s stored_event;

  if (address < DATA_START_ADDR) {
    return 0;
  }

  buffered_read_wrapped(address, (uint8_t*) &stored_event, STORED_EVENT_HEADER_SIZE,
      DATA_START_ADDR);

  if (stored_event.sentinel != SENTINEL_VALUE) {
    return 0;
  }

  *event = stored_event.header;
  event->_private.start_address = address;
  event->_private.current_address = address + STORED_EVENT_HEADER_SIZE;
  event->_private.finished_logging = 1;
  event->_private.written = 1;
  // };

  return 1;
}

/**
 * @brief Lazily reformat storage; overwrites header.
 */
void logger_format_storage(void)
{
  // struct event_header_s first_event = {0};

  struct storage_header_s *header =
    (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {

    memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);

    /* Set all header vals here */
    header->last_event = 0;

    /* Set the dirty flag manually since we weren't using read/write funcs */
    g.flash_buffer.dirty = 1;

    /* Make sure we write to the beginning of storage */
    buffered_flush();
  }
  xSemaphoreGive(g.flash_buffer.lock);
}

/**
 * @brief Start a new event, initializing |event| in the process.
 * 
 * @param event An allocated event instance.
 */
void logger_start_event(struct event_s *event)
{
  /** 
   * 1. Read the header to find the last event
   * 2. Read the last events size and index
   * 3. Write current event at the correct offset
   * 4. Update header to point to current event
   */

  if (event == NULL) {
    return;
  }

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {
    uint32_t event_address;
    uint16_t event_id;

    /* 1. Read the header to get the last event recorded.  */
    uint32_t last_event_address = logger_get_last_event();

    if (last_event_address != 0) {
      struct event_s last_event = {{0}, {0}};

      logger_read_event(last_event_address, &last_event);

      /* 2. Get the address of the new event by the total size of the previous
       * event. Leave room for sentinel value
       */
      event_address = buffered_wrap_addr(last_event_address +
          TOTAL_STORED_EVENT_SIZE(last_event), DATA_START_ADDR);
      event_id = last_event.header.event_id + 1;
    } else {
      event_address = DATA_START_ADDR;
      event_id = 0;
    }

    /* 2.5. Initialize the new event from the last event's size */
    logger_init_event(event, event_address);

    event->header.event_id = event_id;
    event->header.last_event = last_event_address;

    logger_write_event(event, event_address);

    /* Update the main header to point to the new event */
    struct storage_header_s storage_header;
    // printf("last even taddress: %u\n", event_address);
    storage_header.last_event = event_address;
    buffered_write_wrapped(HEADER_ADDR, (const uint8_t*)&storage_header,
        sizeof(storage_header), 0);

    /* Make sure everything's written */
    buffered_flush();

  }
  xSemaphoreGive(g.flash_buffer.lock);
}

/**
 * @brief Commit |event| to storage.
 */
void logger_end_event(struct event_s *event)
{
  /*  1. Go to the address stored in the private section of |event|
   *  2. Write the final event header
   *  3. Update the initial page with the new free offset
   */
  if (event == NULL) {
    return;
  }

  /* If the event was already finished, do nothing */
  if (!event->header.in_progress && event->_private.written) {
      return;
  }

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {
    uint32_t event_addr = event->_private.start_address;

    assert(event_addr < STORAGE_SIZE);

    event->_private.finished_logging = 1;
    event->header.in_progress = 0;

    logger_write_event(event, event_addr);

    /* Update storage header to point to new event */
    struct storage_header_s *storage_header =
      (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

    storage_header->last_event = event_addr;

    buffered_write_wrapped(HEADER_ADDR, (uint8_t*)storage_header,
        sizeof(*storage_header), 0);

    /* Make sure we flush once we complete an event */
    buffered_flush();

    event->_private.written = 1;

  }
  xSemaphoreGive(g.flash_buffer.lock);

}


/**
 * @brief Add a sample to an in-progress event. Failure in this case corresponds
 * to a freakishly-large event that overwrites its own header.
 *
 * TODO: What's the right course of action? Either:
 *   - Return failure 
 *   - Finish the current event
 *   - fail an assert? (that'd be a whole-storage event)
 *
 * 
 * @return 1 on success, 0 on failure.
 */
int logger_write_sample(struct event_s *event,
    struct sensor_packet_s *packet)
{
  assert(packet != NULL);
  assert(event != NULL);
  assert(event->header.in_progress != 0);
  /*
   * Make sure that the sample doesn't overwrite its own event's header
   */
  if (buffered_ranges_overlap(event->_private.start_address, STORED_EVENT_HEADER_SIZE,
                              event->_private.current_address, event->header.sample_size,
                              DATA_START_ADDR))
  {
    return 0;
  }

  /* TODO: Revisit adding a real timeout here */
  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);

  buffered_write_wrapped(event->_private.current_address, (uint8_t*) packet,
      sizeof(*packet), DATA_START_ADDR);

  uint32_t next_sample =  buffered_wrap_addr(event->_private.current_address +
      sizeof(*packet), DATA_START_ADDR);

  event->_private.current_address = next_sample;
  event->header.samples += 1;
  
  xSemaphoreGive(g.flash_buffer.lock);

  return 1;
}

/**
 * @brief Read the |n|th (0-indexed) sample fro |event| into the packet pointed
 * to by |dest|
 */
int logger_read_sample(struct event_s *event, uint32_t n,
    struct sensor_packet_s *dest)
{
  assert(event != NULL);
  assert(dest != NULL);

  uint32_t source;

  if (n >= event->header.samples) {
    return 0;
  }
  
  source = buffered_wrap_addr(event->_private.start_address +
      STORED_EVENT_HEADER_SIZE + event->header.sample_size * n, DATA_START_ADDR);

  assert(source < STORAGE_SIZE);

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);

  buffered_read_wrapped(source, (uint8_t*)dest, sizeof(*dest), DATA_START_ADDR);

  xSemaphoreGive(g.flash_buffer.lock);

  return 1;
}

/**
 * @brief If |prev| is not null, write the event prior to |prev| into *|dst|. 
 * Otherwise, write the most recent event into |dst|
 *
 * @return 1 on successful write to |dst|, 0 otherwise. (i.e. we reached the end
 * of events)
 */
int logger_get_event(struct event_s *prev, struct event_s *dst)
{
  assert (dst != NULL);

  uint32_t address;
  int status = 0;

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {
    /* Return the last event stored if we don't get a prev event */
    if (prev == NULL) {
      address = logger_get_last_event();
    } else {
      address = prev->header.last_event;
    }

    if (address == 0) {
      status = 0;
    } else {
      status = logger_read_event(address, dst);
    }
  }
  xSemaphoreGive(g.flash_buffer.lock);

  return status;
}

#ifdef __cplusplus
}
#endif
