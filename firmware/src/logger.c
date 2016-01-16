/**
 *
 * TODO:
 *  1) Finish naïve event logging
 *  2) Make events wrap around and delete old events
 *  3) Document when the flash buffer is preserved and when overwritten
 *  4) This is in serious need of unit tests
 *
 */

#include <string.h>
#include <unistd.h>

#include <rtos.h>
#include <logger.h>
// #include <util.h>
// #include <config.h>
// #include <sample.h>
#include <buffered_io.h>
// #include <globals.h>
// #include <sample.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The address of the storage header */
const uint32_t HEADER_ADDR = 0x00;

/** @brief The address of the first data page (skip the first two pages) */
const uint32_t DATA_START_ADDR = 2 * STORAGE_PAGE_SIZE;

#if TESTING
#  define TAKE_SEMPHR
#  define GIVE_SEMPHR
#else
#  define TAKE_SEMPHR    xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY)
#  define GIVE_SEMPHR    xSemaphoreGive(g.flash_buffer.lock)
#endif // TESTING


/**
 * @brief Lazily reformat storage; overwrites header.
 */
void logger_format_storage(void)
{
  struct _first_page_s {
    struct storage_header_s header;
    sentinel_t sentinel;
    struct event_header_s first_event;
  } __attribute__((packed));

  struct _first_page_s *first_page =
    (struct _first_page_s *) buffered_get_page(HEADER_ADDR);

  TAKE_SEMPHR;
  {

    memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);

    /* Set all header vals here */
    first_page->header.free_offset = STORAGE_PAGE_SIZE;
    first_page->header.last_event = offsetof(struct _first_page_s, first_event);

    first_page->sentinel = SENTINEL_VALUE;

    first_page->first_event.event_id = 0;
    first_page->first_event.samples = 0;
    first_page->first_event.sample_size = 0;
    first_page->first_event.features = 0;
    first_page->first_event.rtc_start = 0;

    g.flash_buffer.dirty = 1;

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
  // 1. Read the last event written
  // 2. Write the event as incomplete (event_id->sample_size == 0)
  // 3. Update |event| with the address

  if (event == NULL) {
    return;
  }

  // Initialize the event struct first
  memset(event, '\0', sizeof(*event));

  TAKE_SEMPHR;
  {
    struct event_header_s previous_event;
    uint32_t event_address;
    struct storage_header_s *storage_header =
      (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

    /* Get the last event recorded.  */
    buffered_read(storage_header->last_event, (uint8_t*) &previous_event, EVENT_HEADER_SIZE);

    /* Initialize the new event */
    event->event_id = previous_event.event_id + 1;
    event->in_progress = 1;
    event->samples = 0;
    event->sample_size = sizeof(struct sensor_packet_s);
    event->features = CONFIG_FEATURES;
    event->rtc_start = 0; // TODO: Get the actual time stamp
    
    event_address = storage_header->free_offset;

    /* Set the private fields */
    event->__start_address = event_address + EVENT_HEADER_SIZE;
    event->__current_address = event_address + EVENT_HEADER_SIZE;
    event->__started = 1;
    event->__finished_logging = 0;
    event->__written = 0;

    /* Write the new event to disk (will be overwritten once the event completes */
    buffered_write(event_address, (const uint8_t*) event, EVENT_HEADER_SIZE);

    /* Update the main header to point to the new event */
    storage_header->last_event = event_address;
    buffered_write(HEADER_ADDR, (const uint8_t*)storage_header, sizeof(*storage_header));

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
    uint32_t event_write_addr = event->__start_address - EVENT_HEADER_SIZE;
    event->__started = 0;
    event->__finished_logging = 1;
    event->in_progress = 0;

    /* Write the event to storage */
    buffered_write(event_write_addr, (uint8_t*)event, EVENT_HEADER_SIZE);

    /* Update storage header to point to new event */
    struct storage_header_s *storage_header =
      (struct storage_header_s *) buffered_get_page(HEADER_ADDR);

    storage_header->events = storage_header->events + 1;
    storage_header->last_event = event_write_addr;
    storage_header->free_offset = event->__current_address;
    buffered_write(HEADER_ADDR, (uint8_t*)storage_header, sizeof(*storage_header));

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

  /* TODO: Revisit adding a real timeout here */
  TAKE_SEMPHR;

  buffered_write(event->__current_address, (uint8_t*) packet, sizeof(*packet));

  event->__current_address += sizeof(*packet);

  /** TODO: Move wrapping logic to a single place */
  if (event->__current_address > STORAGE_SIZE) {
    event->__current_address %= STORAGE_SIZE;
    event->__current_address += STORAGE_PAGE_SIZE;
  }

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
  
  source = event->__start_address + sizeof(*dest) * n;

  if (source >= STORAGE_SIZE) {
    source = (source % STORAGE_SIZE) + DATA_START_ADDR;
  }

  assert(source < STORAGE_SIZE);

  TAKE_SEMPHR;

  buffered_read(source, (uint8_t*)dest, sizeof(*dest));

  GIVE_SEMPHR;

  return 1;
}


#ifdef __cplusplus
}
#endif
