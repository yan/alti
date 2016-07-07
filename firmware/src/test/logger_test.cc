
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

  ASSERT_EQ(header->last_event, 0);
}


// Make sure that the first event starts at the correct address
TEST_F(LoggerTest, StartsAtRightAddress) {
  struct storage_header_s *header = getHeader();
  struct event_s event;

  logger_start_event(&event);

  ASSERT_EQ(event._private.start_address,
      DATA_START_ADDR); // and past the first event header 
}

TEST_F(LoggerTest, InitializesWithNoEvents) {
  struct event_s event;
  int status;

  status = logger_get_event(NULL, &event);

  ASSERT_EQ(status, 0);
}

TEST_F(LoggerTest, MarksANewEventStarted) {
  struct storage_header_s *header = getHeader();
  struct event_s event;

  logger_start_event(&event);

  ASSERT_EQ(event.header.in_progress, 1);
}

TEST_F(LoggerTest, UpdatesSampleCount) {
  struct event_s event;
  struct sensor_packet_s packet;

  const uint32_t kNumberWrites = 3;

  logger_start_event(&event);

  uint32_t old_sample_size = event.header.samples;
  for (uint32_t i = 0; i < kNumberWrites; i++) {
    logger_write_sample(&event, &packet);
  }

  ASSERT_EQ(event.header.samples, old_sample_size + kNumberWrites);
}

TEST_F(LoggerTest, FailsToReadNonexistentSample) {
  struct event_s event;
  struct sensor_packet_s packet;
  int returned;

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);

  returned = logger_read_sample(&event, 123, &packet);

  ASSERT_EQ(returned, 0);
}

TEST_F(LoggerTest, WriteSampleReadSample) {
  struct event_s event;
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
  struct event_s event;
  struct sensor_packet_s packet;
  int status = 0;

  const int kEndMargin = 10;

  // Fill up storage with enough values to get close to the end
  logger_start_event(&event);
  while (event._private.current_address < (STORAGE_SIZE - kEndMargin * event.header.sample_size)) {
    status = logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);

  // Try to log more to wrap around
  uint32_t prev;
  logger_start_event(&event);
  for (int i = 0; i < kEndMargin * 2; i++) {
    logger_write_sample(&event, &packet);
  }
  logger_end_event(&event);

  ASSERT_GT(event._private.start_address, event._private.current_address);
}

TEST_F(LoggerTest, SampleDoesntOverwriteItsOwnHeader) {
  struct event_s event;
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
  struct event_s event, dst_event;
  struct sensor_packet_s packet;

  logger_start_event(&event);
  logger_write_sample(&event, &packet);
  logger_end_event(&event);

  logger_get_event(0, &dst_event);

  ASSERT_EQ(event.header.event_id, dst_event.header.event_id);
}

TEST_F(LoggerTest, CanRetrieveNextEvent) {
  struct event_s event, dst_event = {0};
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

  ASSERT_EQ(event.header.event_id, dst_event.header.event_id + 1);
}

TEST_F(LoggerTest, ReturnZeroWhenRunningOutOfEvents) {
  struct event_s event, dst_event = {0}, *pevent;
  struct sensor_packet_s packet;
  int status = 0;
  bool valid = true;

  const uint32_t kHighestId = 5;

  for (int i = 0; i < kHighestId; i++) {
    logger_start_event(&event);
    logger_write_sample(&event, &packet);
    logger_end_event(&event);
  }

  pevent = NULL;
  for (int i = kHighestId; i > 0; i--) {
    status = logger_get_event(pevent, &event);

    if (!status) {
      valid = false;
    } 

    if (event.header.event_id != (i - 1)) {
      valid = false;
    }

    pevent = &event;
  }

  if (logger_get_event(&event, &event)) {
    valid = false;
  }

  ASSERT_EQ(valid, true);
}

TEST_F(LoggerTest, DontReturnOverwrittenEvents) {
  const uint32_t kNumEvents = 4;
  const uint32_t kNumSamples = STORAGE_SIZE / sizeof(sensor_packet_s) / kNumEvents;
  const uint32_t kLargeEventSize = sizeof(event_header_s) +
    sizeof(sensor_packet_s) * kNumEvents;

  struct event_s event, *pevent;
  struct sensor_packet_s packet;
  int status = 0;

  // First, record one more than how many events we can keep in storage
  for (int i = 0; i < kNumEvents; i++) {
    logger_start_event(&event);
    for (int j = 0; j < kNumSamples; j++) {
      logger_write_sample(&event, &packet);
    }
    logger_end_event(&event);
  }

  // Then, get all but one of them
  pevent = NULL;
  for (int i = kNumEvents - 1; i > 0; i--) {
    status = logger_get_event(pevent, &event);
    pevent = &event;
  }

  // Then make sure we can't get the last one since it's been overwritten
  status = logger_get_event(pevent, &event);

  ASSERT_EQ(status, 0);
}

}
