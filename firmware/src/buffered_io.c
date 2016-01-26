
#include <string.h>

#include <buffered_io.h>
#include <util.h>
#include <globals.h>
#include <logger.h>

enum xfer_direction_e {
  DIRECTION_READ,
  DIRECTION_WRITE
};


static size_t buffered_xfer_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin, enum xfer_direction_e direction);

uint32_t buffered_wrap_addr(uint32_t x, uint32_t margin) {
  if (x > STORAGE_SIZE) {
    return x % STORAGE_SIZE + margin;
  } else {
    return x;
  }
}
size_t buffered_read(uint32_t addr, uint8_t *data, size_t len)
{
  return buffered_read_wrapped(addr, data, len, 0);
}


size_t buffered_write(uint32_t addr, const uint8_t *data, size_t len)
{
  return buffered_write_wrapped(addr, data, len, 0);
}

size_t buffered_read_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin)
{
  return buffered_xfer_wrapped(addr, data, len, start_margin, DIRECTION_READ);
}

size_t buffered_write_wrapped(uint32_t addr, const uint8_t *data, size_t len,
    uint32_t start_margin)
{
  return buffered_xfer_wrapped(addr, (uint8_t*) data, len, start_margin,
      DIRECTION_WRITE);
}

static size_t buffered_xfer_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin, enum xfer_direction_e direction)
{
  size_t remaining = len;

  /* Make sure our buffer isn't dirty */
  buffered_flush();
  
  while (remaining > 0) {
    if (addr >= STORAGE_SIZE) {
      if (start_margin) {
        addr = addr % STORAGE_SIZE + start_margin;
      } else {
        break;
      }
    }

    uint32_t page_addr = addr & ~STORAGE_PAGE_MASK;
    uint32_t page_offset = addr & STORAGE_PAGE_MASK;
    uint32_t page_available = STORAGE_PAGE_SIZE - page_offset;
    uint32_t n = MIN(page_available, remaining);

    assert(STORAGE_PAGE_SIZE >= page_offset);
    assert(direction == DIRECTION_READ || direction == DIRECTION_WRITE);

    buffered_get_page(page_addr);

    if (direction == DIRECTION_READ) {
      memcpy(data, &g.flash_buffer.data[page_offset], n);
    } else if (direction == DIRECTION_WRITE) {
      /* Data's in cache and we can write what we have available */
      memcpy(&g.flash_buffer.data[page_offset], data, n);
      g.flash_buffer.dirty = 1;
    }

    addr += n;
    data += n;
    remaining -= n;
  }

  return len;
}

uint8_t *buffered_get_page(uint32_t addr)
{
  assert (addr % STORAGE_PAGE_SIZE == 0);

  if (addr != g.flash_buffer.address) {
    buffered_flush();
    flash_read(addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);
    g.flash_buffer.address = addr;
  }

  return g.flash_buffer.data;
}

void buffered_flush(void)
{
  if (!g.flash_buffer.dirty)
    return;

  flash_write(g.flash_buffer.address, g.flash_buffer.data, STORAGE_PAGE_SIZE);
  g.flash_buffer.dirty = 0;
}

// |--|=====================|--|
int buffered_ranges_overlap(uint32_t lhs_start, uint32_t lhs_size, uint32_t rhs_start,
    uint32_t rhs_size)
{
  if (lhs_start < rhs_start) {
    return lhs_start + lhs_size > rhs_start;
  } else {
    return rhs_start + rhs_size > lhs_start;
  }
}
