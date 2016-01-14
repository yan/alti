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
#include <flash.h>
// #include <globals.h>
// #include <sample.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @brief The address of the storage header */
const uint32_t HEADER_ADDR = 0x00;

/** @brief Type of the sentinel value. */
typedef uint32_t sentinel_t;

/** @brief The address of the first data page (skip the first two pages) */
const uint32_t DATA_START_ADDR = 2 * STORAGE_PAGE_SIZE;

/** @brief Value to prepend events with to detect wrap-around */
const sentinel_t SENTINEL_VALUE = 0xAABBCCDD;

#if TESTING
#  define TAKE_SEMPHR
#  define GIVE_SEMPHR
#else
#  define TAKE_SEMPHR    xSemaphoreTake(g.flash_buffer.lock, portMAX_DELAY)
#  define GIVE_SEMPHR    xSemaphoreGive(g.flash_buffer.lock)
#endif // TESTING

/** ========================================================================= */
/* These functions should be moved elsewhere once working */

enum { OP_READ, OP_WRITE };

void flash_cache_flush(void);
size_t flash_cached_read(uint32_t addr, uint8_t *data, size_t len);
size_t flash_cached_write(uint32_t addr, uint8_t *data, size_t len);

size_t flash_cached_read(uint32_t addr, uint8_t *data, size_t len)
{
  size_t transferred = 0;
  uint32_t page_addr = addr & ~STORAGE_PAGE_MASK;
  uint32_t page_offset = addr & STORAGE_PAGE_MASK;

  /* We're about to read but have a dirty buffer, flush it */
  flash_cache_flush();
  
  while (transferred < len) {

    // Buffer isn't dirty, we can overwrite it
    if (g.flash_buffer.address != page_addr) {
      flash_read(page_addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);
      g.flash_buffer.address = page_addr;
    }

    size_t remaining = MIN(STORAGE_PAGE_SIZE - page_offset, len);

    memcpy(data, &g.flash_buffer.data[page_offset], remaining);

    transferred += remaining;

  }

  return transferred;
}

size_t flash_cached_write(uint32_t addr, uint8_t *data, size_t len)
{
  ssize_t remaining = len;
  
  while (remaining > 0) {
    uint32_t page_addr = addr & ~STORAGE_PAGE_MASK;
    uint32_t page_offset = addr & STORAGE_PAGE_MASK;
    uint32_t page_available = STORAGE_PAGE_SIZE - page_offset;

    assert(STORAGE_PAGE_SIZE >= page_offset);

    size_t n = MIN(page_available, (uint32_t) remaining);
    dbg_print("n = %zu, remaining = %zu, page_addr = %x, page_available = %x,"
                  " len = %zu, addr = %x\n", n, remaining, page_addr,
                  page_available, len, addr);

    /* We don't have the data in cache, bring it in */
    if (g.flash_buffer.address != page_addr) {
      flash_cache_flush();

      // XXX: Should only memset if we are debugging
      memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);
      // flash_read(page_addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);
      g.flash_buffer.address = page_addr;
    }
    
    /* Data's in cache and we can write what we have available */
    memcpy(&g.flash_buffer.data[page_offset], data, n);
    g.flash_buffer.dirty = 1;

    addr += n;
    remaining -= n;
  }

  assert(remaining >= 0);

  return len;
}

#if 0
void flash_cached_op(uint8_t op, uint32_t addr, uint8_t *data, size_t len)
{
  size_t transferred = 0;





  /* First check if the operation is entirely within our region */
  if (g.flash_buffer.address                     <= addr &&
      g.flash_buffer.address + STORAGE_PAGE_SIZE >  addr + len) {
  {
    if (op == OP_READ) {

      /* Make sure we have the write page in the buffer */
      if (g.flash_buffer.address != page_addr) {
        flash_read(page_addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);
        g.flash_buffer.dirty = 0;
      }

        flash_read(g.flash_buffer.address
  }

  while (transferred < len) {
    /* At least some part of the requested data is in the current cache */
    if (g.flash_buffer.address                     <= addr &&
        g.flash_buffer.address + STORAGE_PAGE_SIZE >  addr) {
      if (op == OP_WRITE) {
        size_t remaining = STORAGE_PAGE_SIZE - g.flash_buffer.write_offset;
        size_t to_copy = MIN(remaining, len);
        
        
        
      }

    } else {
    }
  }
  }
}
}
#endif

// #define flash_cached_write(addr, data, len) flash_cached_op(OP_WRITE, addr, data, len)
// #define flash_cached_read(addr, data, len) flash_cached_op(OP_READ, addr, data, len)

void flash_cache_flush(void)
{
  if (g.flash_buffer.dirty) {
    flash_write(g.flash_buffer.address, g.flash_buffer.data, STORAGE_PAGE_SIZE);
    g.flash_buffer.dirty = 0;
  }
}
/** ========================================================================= */


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
 * @brief Flush the current flash buffer to |*addr| in flash.
 */
static void logger_flush_buffer(uint32_t addr)
{
  dbg_print("Writing to flash\n");

#if ENABLE_FLASH_DEBUG
  assert(addr >= g.flash_buffer.address);
  assert(addr <  g.flash_buffer.address + STORAGE_PAGE_SIZE);
#endif

  flash_write(addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);

  /* XXX: This clearing isn't needed; put it behind an ifdef or fill it with a
   * filler byte for debugging
   */
  memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);
  g.flash_buffer.write_offset = 0;
}

/**
 * @brief Get the last event recorded; trashes the flash buffer.
 *
 */
#if 0
static uint32_t logger_get_last_event(void)
{
  struct storage_header_s *first_page = (void *) &g.flash_buffer.data[0];

  flash_read(HEADER_ADDR, (uint8_t*) first_page, STORAGE_PAGE_SIZE);

#if ENABLE_FLASH_DEBUG
  g.flash_buffer.valid = 0;
#endif

  assert(first_page->last_event < STORAGE_SIZE);

  return first_page->last_event;
}

/**
 * @brief Read some data from storage, crossing page boundaries if necessary. 
 * Can only be used to read non-header data, as it this handles wrap around.
 */
static size_t logger_read(uint32_t addr, uint8_t *dst, size_t len)
{
  assert(addr < STORAGE_SIZE);
  assert(dst != NULL);

  uint32_t page_addr = addr & STORAGE_PAGE_MASK;
  uint32_t remaining_in_page = (page_addr + STORAGE_PAGE_SIZE) - addr;
  uint32_t page_offset = addr - page_addr;
  size_t to_read;

  while (len > 0) {
    flash_read(page_addr, &g.flash_buffer.data[0], STORAGE_PAGE_SIZE);

    to_read = MIN(remaining_in_page, len);

    if (dst != &g.flash_buffer.data[page_offset]) {
      memcpy(dst, &g.flash_buffer.data[page_offset], to_read);
    }

    /* Short circuit the common case */
    if (len == to_read) {
      break;
    }

    dst += to_read;
    len -= to_read;
    page_addr += STORAGE_PAGE_SIZE;

    /* Next page will be completely read in from beginning */
    remaining_in_page = STORAGE_PAGE_SIZE;
    page_offset = 0;

    /* Wrap around to the start of data section */
    if (page_addr >= STORAGE_SIZE) {
      page_addr = DATA_START_ADDR;
    }
  }

  return 1;

}

#endif
/**
 * @brief ...
 *
 *
 */
void logger_format_storage(void)
{
  struct _first_page_s {
    struct storage_header_s header;
    sentinel_t sentinel;
    struct event_header_s first_event;
  } __attribute__((packed));

  struct _first_page_s *first_page = (struct _first_page_s *) g.flash_buffer.data;

  TAKE_SEMPHR;
  {

    memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);

    /* Set all header vals here */
    first_page->header.free_offset = STORAGE_PAGE_SIZE;
    first_page->header.last_event = sizeof(struct storage_header_s);

    first_page->sentinel = SENTINEL_VALUE;

    first_page->first_event.event_id = 0;
    first_page->first_event.samples = 0;
    first_page->first_event.sample_size = 0;
    first_page->first_event.features = 0;
    first_page->first_event.rtc_start = 0;

    /* Write to the beginning of storage */
    logger_flush_buffer(HEADER_ADDR);
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
  uint32_t addr = 0;

  if (event == NULL) {
    return;
  }


  TAKE_SEMPHR;
  {
    struct storage_header_s *first_page = (struct storage_header_s *) g.flash_buffer.data;

    flash_cache_flush();

    /* Read the header to see where we need to write to */
    flash_read(HEADER_ADDR, g.flash_buffer.data, STORAGE_PAGE_SIZE);
    g.flash_buffer.address = HEADER_ADDR;

    /* Load the page containing free address into buffer */
    addr = first_page->free_offset;

    /* XXX: Handle read errors better */
    assert(addr < STORAGE_SIZE);

    // g.flash_buffer.address = addr & STORAGE_PAGE_MASK;
    // flash_read(g.flash_buffer.address, g.flash_buffer.data, STORAGE_PAGE_SIZE);
  }
  GIVE_SEMPHR;

  event->__start_address = addr + EVENT_HEADER_SIZE;
  event->__current_address = event->__start_address;

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

  flash_cached_write(event->__start_address - EVENT_HEADER_SIZE, (uint8_t*)event, EVENT_HEADER_SIZE);
  flash_cache_flush();
}

#if 0
static void logger_write(uint32_t address, uint8_t *data, size_t len)
{
  size_t remaining;
  uint8_t *dst = (uint8_t*) &g.flash_buffer.data[g.flash_buffer.write_offset];

  assert(data != NULL);
  assert(len > 0);

  remaining = STORAGE_PAGE_SIZE - g.flash_buffer.write_offset;

  /* If the length exceeds how much space we have in the buffer, */
  while (len > remaining) {

    /* 1. Copy as many bytes we have available (coudld be zero) */
    memcpy(dst, data, remaining);

#if ENABLE_FLASH_DEBUG
    assert(g.flash_buffer.address == (address & STORAGE_PAGE_MASK));
#endif

    /* 2. Flush the buffer to storage at the correct address */
    logger_flush_buffer(address & STORAGE_PAGE_MASK);

#if ENABLE_FLASH_DEBUG
    g.flash_buffer.address += STORAGE_PAGE_SIZE;
    memset(g.flash_buffer.data, '\0', STORAGE_PAGE_SIZE);
#endif

    /* 3. Reset the flash buffer for the next page */
    g.flash_buffer.write_offset = 0;
    dst = &g.flash_buffer.data[0];

    /* 4. Realign the data we need to write. Remaining is now again a
     * full page
     */
    data = data + remaining;
    address = address + remaining;
    len = len - remaining;
    remaining = STORAGE_PAGE_SIZE;

    /* Wrap around to the start of data section */
    if (address >= STORAGE_SIZE) {
      address = DATA_START_ADDR;
    }
  }

  /* At this point, we have enough space in the current buffer */
  memcpy(dst, data, len);

  g.flash_buffer.write_offset += len;
}
#endif

/**
 *
 */
void logger_write_sample(struct event_header_s *event, struct sensor_packet_s *packet)
{

  assert(event != NULL);
  assert(packet != NULL);

  /* TODO: Revisit adding a real timeout here */
  TAKE_SEMPHR;

  // logger_write(event->__current_address, (uint8_t*) packet, sizeof(*packet));
  flash_cached_write(event->__current_address, (uint8_t*) packet, sizeof(*packet));

  event->__current_address += sizeof(*packet);

  event->samples += 1;
  
  GIVE_SEMPHR;
}

void logger_read_sample(struct event_header_s *event, uint32_t n, struct sensor_packet_s *dest)
{
  assert(event != NULL);
  assert(dest != NULL);

  uint32_t source = event->__start_address + sizeof(*dest) * n;

  if (source >= STORAGE_SIZE) {
    source = (source % STORAGE_SIZE) + DATA_START_ADDR;
  }

  assert(source < STORAGE_SIZE);

  TAKE_SEMPHR;

  flash_cached_read(source, (uint8_t*)dest, sizeof(*dest));

  GIVE_SEMPHR;
}


#ifdef __cplusplus
}
#endif
