
#include <gtest/gtest.h>
#include <logger.h>
#include "flash_mock.h"
#include "../logger_private.h"

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

  ASSERT_EQ(header->last_event, DATA_START_ADDR);
}

TEST_F(LoggerTest, CreatesFirstSentinel) {
  auto header = getHeader();

  sentinel_t sentinel = *(sentinel_t*)(header + 1);

  ASSERT_EQ(sentinel, SENTINEL_VALUE);
}

TEST_F(LoggerTest, CreatesEmptyEvent) {
  auto header = getHeader();
  auto first_event = getAt<stored_event_header_s>(header->last_event);

  bool firstEventIsNull = 
    first_event->header.event_id == 0 &&
    first_event->header.samples == 0 &&
    first_event->header.sample_size == 0 &&
    first_event->header.features == 0 &&
    first_event->header.rtc_start == 0;

  ASSERT_EQ(firstEventIsNull, true);
}

// Make sure that the first event starts at the correct address
TEST_F(LoggerTest, StartsAtRightAddress) {
  struct storage_header_s *header = getHeader();
  struct event_header_s event;

  logger_start_event(&event);

  ASSERT_EQ(event._prv.start_address,
      sizeof(struct storage_header_s) + // Past the storage header
      STORED_EVENT_HEADER_SIZE); // and past the first event header 
}

TEST_F(LoggerTest, MarksANewEventStarted) {
  struct storage_header_s *header = getHeader();
  struct event_header_s event;

  logger_start_event(&event);

  ASSERT_EQ(event.in_progress, 1);
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

#if 0
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

#endif
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

TEST_F(LoggerTest, LoggingWrapsCorrectly) {
  struct event_header_s event;
  struct sensor_packet_s packet;

  const int kEndMargin = 10;

  // Fill up storage with enough values to get close to the end
  logger_start_event(&event);
  while (event._prv.current_address < (STORAGE_SIZE - kEndMargin * event.sample_size)) {
    logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);

  // Try to log more to wrap around
  uint32_t prev;
  logger_start_event(&event);
  for (int i = 0; i < kEndMargin * 2; i++) {
    logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);

  ASSERT_GT(event._prv.start_address, event._prv.current_address);
}

TEST_F(LoggerTest, SampleDoesntOverwriteItsOwnHeader) {
  struct event_header_s event;
  struct sensor_packet_s packet;
  uint32_t writes = STORAGE_SIZE / sizeof(packet) + 30;
  int status = 1;

  logger_start_event(&event);
  for (; status || writes > 0; writes--) {
    status = logger_write_sample(&event, &packet);
  }

  ASSERT_EQ(status, 0);
}

TEST_F(LoggerTest, CanRetrieveFirstEvent) {
  struct event_header_s event, dst_event;
  struct sensor_packet_s packet;

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);

  logger_get_event(0, &dst_event);

  ASSERT_EQ(event.event_id, dst_event.event_id);
}

TEST_F(LoggerTest, CanRetrieveNextEvent) {
  struct event_header_s event, dst_event = {0};
  struct sensor_packet_s packet;
  int status;
  const uint32_t kHighestId = 5;

  for (int i = 0; i < kHighestId; i++) {
    logger_start_event(&event);
    logger_write_sample(&event, &packet);
    logger_end_event(&event);
  }

  // Get the first event
  logger_get_event(NULL,                &event);
  // event id is now kHighestId
  logger_get_event(&event, &event);
  // event id  is now kHighestId - 1
  logger_get_event(&event, &dst_event);
  // now dst_event is of id kHighestId - 2

  ASSERT_EQ(event.event_id, dst_event.event_id + 1);
}

TEST_F(LoggerTest, ReturnZeroWhenRunningOutOfEvents) {
  struct event_header_s event, dst_event = {0}, *pevent;
  struct sensor_packet_s packet;
  int status;
  bool valid = true;

  const uint32_t kHighestId = 5;

  for (int i = 0; i < kHighestId; i++) {
    logger_start_event(&event);
    logger_write_sample(&event, &packet);
    logger_end_event(&event);
  }

  pevent = NULL;
  for (int i = 0; i < kHighestId + 1; i++) {
    if (!logger_get_event(pevent, &event)) {
      valid = false;
    } 
    pevent = &event;
  }

  if (logger_get_event(&event, &event)) {
    valid = false;
  }

  ASSERT_EQ(valid, true);
}

}

