/**
 *
 * TODO:
 *  1) Finish naïve event logging
 *  2) Make events wrap around and delete old events
 *
 *
 */
#include <FreeRTOS.h>
#include <semphr.h>

#include <string.h>

#include <config.h>
#include <logger.h>
#include <sample.h>
#include <flash.h>
#include <globals.h>
#include <sample.h>


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

/**
 * @brief Flush the current flash buffer to |*addr| in flash. Bump *addr the 
 * page size after writing.
 */
static void logger_flush_buffer(uint32_t *addr)
{
  dbg_print("Writing to flash");

  flash_write(*addr, g.flash_buffer.data, WRITE_BUFFER_LEN);

  /* XXX: This clearing isn't needed; put it behind an ifdef or fill it with a
   * filler byte for debugging
   */
  memset(g.flash_buffer.data, '\0', WRITE_BUFFER_LEN);
  g.flash_buffer.write_offset = 0;

  *addr += WRITE_BUFFER_LEN;
}

void logger_format_storage(void)
{
  uint32_t zero_addr = 0;

  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {
    struct storage_header_s *first_page = (void *) g.flash_buffer.data;
    struct event_header_s *first_event = (void*) (first_page + 1);

    memset(g.flash_buffer.data, '\0', WRITE_BUFFER_LEN);

    /* Set all header vals here */
    first_page->free_offset = WRITE_BUFFER_LEN;
    first_page->last_event = sizeof(struct storage_header_s);

    first_event->event_id = 0;
    first_event->samples = 0;
    first_event->sample_size = 0;
    first_event->features = 0;
    first_event->rtc_start = 0;
    first_event->last_event = 0;

    logger_flush_buffer(&zero_addr);
  }
  xSemaphoreGive(g.flash_buffer.lock);
}


/**
 *
 */
void logger_start_event(struct event_header_s *event)
{
  // 1. Read the last event written
  // 2. Write the event as incomplete (event_id->sample_size == 0)
  // 3. Update |event| with the address
  uint32_t addr = 0;

  if (event == NULL) {
    return;
  }


  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);
  {
    struct storage_header_s *first_page = (void *) g.flash_buffer.data;

    flash_read(0, (uint8_t*) first_page, WRITE_BUFFER_LEN);

    /* Make room for the event header */
    addr = first_page->free_offset + EVENT_HEADER_SIZE;

  }
  xSemaphoreGive(g.flash_buffer.lock);

  event->__start_address = addr;
  event->__current_address = 0;

  event->__started = 1;
}

/**
 *
 *
 *
 */
void logger_end_event(struct event_header_s *event)
{
  // 1. Go to the address stored in the private section of |event|
  // 2. Write the final event header
  // 3. Update the initial page with the new free offset
  if (event == NULL) {
    return;
  }

  (void) event;
}

void logger_read_sample(struct event_header_s *event, uint32_t n)
{
  (void) event;
  (void) n;
}

/**
 *
 *
 *
 */
void logger_write_packet(struct event_header_s *event, struct sensor_packet_s *packet)
{
  size_t remaining = WRITE_BUFFER_LEN - g.flash_buffer.write_offset,
         to_write = sizeof(*packet);
  uint8_t *src = (uint8_t*)  packet,
          *dst = (uint8_t*) &g.flash_buffer.data[g.flash_buffer.write_offset];

  if (event == NULL) {
    return;
  }

  /* TODO: Revisit adding a real timeout here */
  xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY);

  /* If we can't fit a sample packet in what's left,  */
  if (remaining < to_write) {

    /* 1. Fill up the remainder of the page with a part of the packet */
    memcpy(dst, src, remaining);

    /* 2. Flush buffer to storage */
    logger_flush_buffer(&event->__current_address);

    /* 3. If there's a packet remainder, it'll be written after */
    to_write = to_write - remaining;
    src = src + remaining;
    dst = &g.flash_buffer.data[g.flash_buffer.write_offset];
  }

  memcpy(dst, src, to_write);

  g.flash_buffer.write_offset += to_write;
  
  xSemaphoreGive(g.flash_buffer.lock);
}
