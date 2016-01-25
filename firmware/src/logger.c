/**
 *
 * TODO:
 *  1) Finish naïve event logging
 *
 */

#include <string.h>
#include <unistd.h>

#include <rtos.h>
#include <logger.h>
#include <buffered_io.h>

#ifdef __cplusplus
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

static inline void logger_init_event(struct event_header_s *event)
{
  event->samples = 0;
  event->sample_size = sizeof(struct sensor_packet_s);
  event->features = CONFIG_FEATURES;
  event->in_progress = 1;
  event->rtc_start = 0; // TODO: Get the actual time stamp
}

static inline void logger_write_event(struct event_header_s *event, uint32_t address) {
  struct stored_event_header_s stored_event;

  stored_event.sentinel = SENTINEL_VALUE;
  stored_event.header = *event;

  buffered_write_wrapped(address, (const uint8_t*) &stored_event,
      STORED_EVENT_HEADER_SIZE, DATA_START_ADDR);
}

static inline void logger_read_event(struct event_header_s *event, uint32_t address) {
  struct stored_event_header_s stored_event;

  buffered_read_wrapped(address, (uint8_t*) &stored_event, STORED_EVENT_HEADER_SIZE,
      DATA_START_ADDR);

  assert(stored_event.sentinel == SENTINEL_VALUE);

  *event = stored_event.header;
}

/**
 * @brief Lazily reformat storage; overwrites header.
 */
void logger_format_storage(void)
{
  struct event_header_s first_event = {0};

  struct storage_header_s *header =
    (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

  TAKE_SEMPHR;
  {

    memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);

    /* Set all header vals here */
    header->last_event = DATA_START_ADDR;
    logger_write_event(&first_event, DATA_START_ADDR);

    /* Set the dirty flag manually since we weren't using read/write funcs */
    // g.flash_buffer.dirty = 1;

    /* Write to the beginning of storage */
    buffered_flush();
  }
  GIVE_SEMPHR;
}

/**
 * @brief Start a new event, initializing |event| in the process.
 * 
 * @param event An allocated event instance.
 */
void logger_start_event(struct event_header_s *event)
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

  TAKE_SEMPHR;
  {
    struct event_header_s last_event;
    // struct stored_event_header_s last_event;
    uint32_t event_address;

    /* 1. Read the header to get the last event recorded.  */
    uint32_t last_event_address = logger_get_last_event();

    logger_read_event(&last_event, last_event_address);

    /* 2. Get the address of the new event by the total size of the previous
     * event. Leave room for sentinel value
     */
    event_address = buffered_wrap_addr(last_event_address +
        TOTAL_STORED_EVENT_SIZE(last_event), DATA_START_ADDR);

    /* 2.5. Initialize the new event from the last event's size */
    logger_init_event(event);

    event->event_id = last_event.event_id + 1;
    event->last_event = last_event_address;
    /* Set the private fields */
    event->__start_address = event_address;
    event->__current_address = event_address + STORED_EVENT_HEADER_SIZE;
    event->__finished_logging = 0;
    event->__written = 0;

    logger_write_event(event, event_address);

    /* Update the main header to point to the new event */
    struct storage_header_s storage_header;
    storage_header.last_event = event_address;
    buffered_write_wrapped(HEADER_ADDR, (const uint8_t*)&storage_header,
        sizeof(storage_header), 0);

    /* Make sure everything's written */
    buffered_flush();

  }
  GIVE_SEMPHR;
}

/**
 * @brief Commit |event| to storage.
 */
void logger_end_event(struct event_header_s *event)
{
  // 1. Go to the address stored in the private section of |event|
  // 2. Write the final event header
  // 3. Update the initial page with the new free offset
  if (event == NULL) {
    return;
  }

  TAKE_SEMPHR;
  {
    uint32_t event_addr = event->__start_address;

    assert(event_addr < STORAGE_SIZE);

    event->__finished_logging = 1;
    event->in_progress = 0;

    logger_write_event(event, event_addr);

    /* Update storage header to point to new event */
    struct storage_header_s *storage_header =
      (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

    storage_header->last_event = event_addr;
    // storage_header->free_offset = event->__current_address;

    buffered_write_wrapped(HEADER_ADDR, (uint8_t*)storage_header,
        sizeof(*storage_header), 0);

    /* Make sure we flush once we complete an event */
    buffered_flush();

    event->__written = 1;

  }
  GIVE_SEMPHR;

}


/**
 * @brief Add a sample to an in-progress event.
 */
void logger_write_sample(struct event_header_s *event, struct sensor_packet_s *packet)
{
  assert(packet != NULL);
  assert(event != NULL);
  assert(event->in_progress != 0);
  /*
   * Make sure that the sample doesn't overwrite its own event's header
   *
   * TODO: What's the right course of action? Either:
   *   - Return failure 
   *   - Finish the current event
   *   - fail an assert? (that'd be a whole-storage event)
   */
  assert(!buffered_ranges_overlap(event->__start_address, STORED_EVENT_HEADER_SIZE,
                                  event->__current_address, event->sample_size));

  /* TODO: Revisit adding a real timeout here */
  TAKE_SEMPHR;

  buffered_write_wrapped(event->__current_address, (uint8_t*) packet,
      sizeof(*packet), DATA_START_ADDR);

  uint32_t next_sample =  buffered_wrap_addr(event->__current_address + sizeof(*packet),
      DATA_START_ADDR);

  uint32_t next_sample_end = buffered_wrap_addr(next_sample + sizeof(*packet),
      DATA_START_ADDR);

  event->__current_address = next_sample;
  event->samples += 1;
  
  GIVE_SEMPHR;
}

/**
 * @brief Read the |n|th (0-indexed) sample fro |event| into the packet pointed
 * to by |dest|
 */
int logger_read_sample(struct event_header_s *event, uint32_t n, struct sensor_packet_s *dest)
{
  assert(event != NULL);
  assert(dest != NULL);

  uint32_t source;

  if (n >= event->samples) {
    return 0;
  }
  
  source = buffered_wrap_addr(event->__start_address + STORED_EVENT_HEADER_SIZE
      + event->sample_size * n, DATA_START_ADDR);

  assert(source < STORAGE_SIZE);

  TAKE_SEMPHR;

  buffered_read_wrapped(source, (uint8_t*)dest, sizeof(*dest), DATA_START_ADDR);

  GIVE_SEMPHR;

  return 1;
}


#ifdef __cplusplus
}
#endif
