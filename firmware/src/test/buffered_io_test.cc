#include <string.h>
#include <buffered_io.h>
#include <globals.h>
#include <gtest/gtest.h>
#include "flash_mock.h"

namespace {

class BufferedIoTest : public ::testing::Test {
  protected:
    BufferedIoTest() {
      // memset(__testing_storage, '\0', sizeof(__testing_storage));
      for (unsigned int i = 0; i < sizeof(__testing_storage); i++) {
        __testing_storage[i] = i % 0xFF;
      }

      memset((void*)&g.flash_buffer, '\0', sizeof(g.flash_buffer));
    }
    virtual ~BufferedIoTest() {
    }
    virtual void SetUp() {

    }
    virtual void TearDown() {
    }
    uint8_t *getStorage() const {
      return __testing_storage;
    }

};
// Test that a dirty buffer will be flushed
TEST_F(BufferedIoTest, FlushDirtyBuffer) {
  // bypass the read/write functions as we're testing just the flushing
  const uint8_t kValue = 100;
  g.flash_buffer.data[0] = kValue;
  g.flash_buffer.dirty = 1;

  buffered_flush();

  ASSERT_EQ(__testing_storage[0], kValue);
}

// Test that a clean buffer will not be flushed
TEST_F(BufferedIoTest, PreserveCleanBuffer) {
  const uint8_t kValue = 100;
  g.flash_buffer.data[0] = kValue;

  buffered_flush();

  ASSERT_NE(__testing_storage[0], kValue);
}

// Test that a write will mark a buffer dirty
TEST_F(BufferedIoTest, WriteMarkDirty) {
  const uint32_t kAddress = 0x00;
  uint8_t src = 0xFF;

  buffered_write(0, &src, sizeof(src));

  ASSERT_EQ(g.flash_buffer.dirty, 1);
}

// Test that a write, followed by a flush, will commit a buffer to storage
TEST_F(BufferedIoTest, WriteAndFlush) {
  const uint8_t kValue = 100;
  const uint32_t kAddress = 0x00;

  uint8_t src = kValue;

  buffered_write(0, &src, sizeof(src));
  buffered_flush();

  ASSERT_EQ(__testing_storage[0], kValue);
}

TEST_F(BufferedIoTest, WriteAcrossPages) {
  const uint32_t kValue = 0x12345678;
  const uint32_t kAddress = STORAGE_PAGE_SIZE - sizeof(kValue) / 2;

  buffered_write(kAddress, (uint8_t *)&kValue, sizeof(kValue));
  buffered_flush();

  ASSERT_EQ(kValue, *(uint32_t*)&__testing_storage[kAddress]);
}

TEST_F(BufferedIoTest, WrapsWithMargin) {
  const uint8_t kValue[4] = {0x12, 0x34, 0x56, 0x78};
  const uint32_t kAddress = STORAGE_SIZE - sizeof(kValue) / 2;
  bool valid;

  buffered_write_wrapped(kAddress, kValue, sizeof(kValue), STORAGE_PAGE_SIZE);
  buffered_flush();

  valid = (
      __testing_storage[STORAGE_SIZE-2] == kValue[0] &&
      __testing_storage[STORAGE_SIZE-1] == kValue[1] &&
      __testing_storage[STORAGE_PAGE_SIZE] == kValue[2] &&
      __testing_storage[STORAGE_PAGE_SIZE+1] == kValue[3]);

  ASSERT_EQ(valid, true);
}

}
