
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
  hexDump(NULL, data, size);
}

void flash_write(uint32_t addr, uint8_t *data, size_t size)
{
  assert(addr + size < STORAGE_SIZE);
  hexDump(NULL, data, size);
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
    }
    virtual void TearDown() {
    }
};

TEST_F(LoggerTest, TestingWhat) {
  EXPECT_EQ(0, 0);
}



}

