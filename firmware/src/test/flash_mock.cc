#include <string.h>
#include <util.h>

#include "flash_mock.h"

extern "C" {

uint8_t __testing_storage[STORAGE_SIZE] = {0};

void flash_read(uint32_t addr, uint8_t *data, size_t size)
{
  assert(addr + size <= STORAGE_SIZE);
  memcpy(data, &__testing_storage[addr], size);
  // hexDump(NULL, data, size);
}

void flash_write(uint32_t addr, uint8_t *data, size_t size)
{
  assert(addr + size <= STORAGE_SIZE);
  // hexDump(NULL, data, size);
  memcpy(&__testing_storage[addr], data, size);
}

}


