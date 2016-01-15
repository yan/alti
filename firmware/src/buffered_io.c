
#include <string.h>

#include <buffered_io.h>
#include <util.h>
#include <globals.h>

#ifndef WRITE_OP
#  define WRITE_OP flash_write
#endif

#ifndef READ_OP
#  define READ_OP flash_read
#endif

size_t buffered_read(uint32_t addr, uint8_t *data, size_t len)
{
  return buffered_read_wrapped(addr, data, len, STORAGE_PAGE_SIZE, 1);
}

size_t buffered_read_wrapped(uint32_t addr, uint8_t *data, size_t len, 
    uint32_t start_margin, int should_wrap)
{
  size_t transferred = 0;
  uint32_t page_addr = addr & ~STORAGE_PAGE_MASK;
  uint32_t page_offset = addr & STORAGE_PAGE_MASK;

  /* Make sure our buffer isn't dirty */
  buffered_flush();
  
  while (transferred < len) {

    buffered_get_page(page_addr);

    size_t remaining = MIN(STORAGE_PAGE_SIZE - page_offset, len);

    memcpy(data, &g.flash_buffer.data[page_offset], remaining);

    transferred += remaining;
  }

  return transferred;
}

size_t buffered_write(uint32_t addr, const uint8_t *data, size_t len)
{
  return buffered_write_wrapped(addr, data, len, STORAGE_PAGE_SIZE, 1);
}

size_t buffered_write_wrapped(uint32_t addr, const uint8_t *data, size_t len,
    uint32_t start_margin, int should_wrap)
{
  uint32_t remaining = len;

  while (remaining > 0) {
    uint32_t page_addr = addr & ~STORAGE_PAGE_MASK;
    uint32_t page_offset = addr & STORAGE_PAGE_MASK;
    uint32_t page_available = STORAGE_PAGE_SIZE - page_offset;
    uint32_t n = MIN(page_available, remaining);

    /* If we overflow, check if we |should_wrap|. If we don't, just bail now
     * and return how may bytes we wrote
     */
    if (page_addr >= STORAGE_SIZE) {
      if (should_wrap) {
        page_addr = start_margin;
      } else {
        break;
      }
    }

    assert(STORAGE_PAGE_SIZE >= page_offset);

    buffered_get_page(page_addr);
    
    /* Data's in cache and we can write what we have available */
    memcpy(&g.flash_buffer.data[page_offset], data, n);
    g.flash_buffer.dirty = 1;

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
    READ_OP(addr, g.flash_buffer.data, STORAGE_PAGE_SIZE);
    g.flash_buffer.address = addr;
  }

  return g.flash_buffer.data;
}

void buffered_flush(void)
{
  if (!g.flash_buffer.dirty)
    return;

  WRITE_OP(g.flash_buffer.address, g.flash_buffer.data, STORAGE_PAGE_SIZE);
  g.flash_buffer.dirty = 0;
}
