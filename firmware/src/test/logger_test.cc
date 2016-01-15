
#include <gtest/gtest.h>
#include <flash.h>

void hexDump(char *desc, void *addr, int len);
void hexDump(char *desc, void *addr, int len) {
    int i;
    unsigned char buff[17],
                 *pc = (unsigned char*)addr;

    // Output description if given.
    if (desc != NULL)
        printf ("%s:\n", desc);

    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) printf ("  %s\n", buff);
            printf ("  %04x ", i);
        }
        printf (" %02x", pc[i]);
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) buff[i % 16] = '.';
        else buff[i % 16] = pc[i];
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) { printf ("   "); i++; }
    printf ("  %s\n", buff);
}

uint8_t __testing_storage[STORAGE_SIZE] = {0};

extern "C" {
void flash_read(uint32_t addr, uint8_t *data, size_t size)
{
  assert(addr + size < STORAGE_SIZE);
  memcpy(data, &__testing_storage[addr], size);
  // hexDump(NULL, data, size);
}

void flash_write(uint32_t addr, uint8_t *data, size_t size)
{
  assert(addr + size < STORAGE_SIZE);
  // hexDump(NULL, data, size);
  memcpy(&__testing_storage[addr], data, size);
}

}


//#include "../logger.c"
#include <logger.h>

namespace {

class LoggerTest : public ::testing::Test {
  protected:
    LoggerTest() {
    }
    virtual ~LoggerTest() {
    }
    virtual void SetUp() {
      logger_format_storage();
    }
    virtual void TearDown() {
    }
    storage_header_s *getHeader() const {
      return (storage_header_s *) __testing_storage;
    }
    template <typename T>
    T* getAt(uint32_t addr) const {
      return reinterpret_cast<T*>(&__testing_storage[addr]);
    }


};

TEST_F(LoggerTest, FormatsHeader) {

  auto header = getHeader();

  bool valid = 
    header->events == 0 &&
    header->free_offset == STORAGE_PAGE_SIZE &&
    header->last_event == sizeof(struct storage_header_s) + sizeof(sentinel_t);

  ASSERT_EQ(valid,true);
}

TEST_F(LoggerTest, CreatesFirstSentinel) {
  auto header = getHeader();

  sentinel_t sentinel = *(sentinel_t*)(header + 1);

  ASSERT_EQ(sentinel, SENTINEL_VALUE);
}

TEST_F(LoggerTest, CreatesEmptyEvent) {
  auto header = getHeader();
  auto first_event = getAt<event_header_s>(header->last_event);

  bool firstEventIsNull = 
    first_event->event_id == 0 &&
    first_event->samples == 0 &&
    first_event->sample_size == 0 &&
    first_event->features == 0 &&
    first_event->rtc_start == 0;

  ASSERT_EQ(firstEventIsNull, true);
}

// Make sure that the first event starts at the correct address
TEST_F(LoggerTest, StartsAtRightAddress) {
  struct storage_header_s *header = getHeader();
  struct event_header_s event;

  logger_start_event(&event);

  ASSERT_EQ(event.__start_address, header->free_offset + EVENT_HEADER_SIZE);
}

TEST_F(LoggerTest, MarksANewEventStarted) {
  struct storage_header_s *header = getHeader();
  struct event_header_s event;

  logger_start_event(&event);

  ASSERT_EQ(event.__started, 1);
}
}

