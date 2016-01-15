
#include <gtest/gtest.h>
#include <logger.h>
#include "flash_mock.h"

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

//#include "../logger.c"

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

TEST_F(LoggerTest, UpdatesSampleCount) {
  struct event_header_s event;
  struct sensor_packet_s packet;

  const uint32_t kNumberWrites = 3;

  logger_start_event(&event);

  uint32_t old_sample_size = event.samples;
  for (uint32_t i = 0; i < kNumberWrites; i++) {
    logger_write_sample(&event, &packet);
  }

  ASSERT_EQ(event.samples, old_sample_size + kNumberWrites);
}

TEST_F(LoggerTest, MaintainsEventCount) {
  struct event_header_s event;
  struct sensor_packet_s packet;
  struct storage_header_s *header = getHeader();

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);
  
  ASSERT_EQ(header->events, 1);
}

TEST_F(LoggerTest, UpdatesFreeOffset) {
  struct event_header_s event;
  struct sensor_packet_s packet;
  struct storage_header_s *header = getHeader();

  const auto kSamplesToWrite = 400;

  uint32_t free_offset_pre = header->free_offset;

  logger_start_event(&event);
  for (int i = 0; i < kSamplesToWrite; i++) {
    logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);
  
  uint32_t free_offset_post = header->free_offset;
  
  ASSERT_EQ(free_offset_post - free_offset_pre,
      EVENT_HEADER_SIZE + sizeof(struct sensor_packet_s) * kSamplesToWrite);
}

TEST_F(LoggerTest, FailsToReadNonexistentSample) {
  struct event_header_s event;
  struct sensor_packet_s packet;
  int returned;

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);

  returned = logger_read_sample(&event, 123, &packet);

  ASSERT_EQ(returned, 0);
}

TEST_F(LoggerTest, WriteSampleReadSample) {
  struct event_header_s event;
  struct sensor_packet_s packet;
  const auto kTickVal = 0x1234;

  packet.ticks = kTickVal;

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);

  packet.ticks = 0;

  logger_read_sample(&event, 0, &packet);

  ASSERT_EQ(packet.ticks, kTickVal);
}


}

