
#include <globals.h>
#include <gtest/gtest.h>
#include <rtos.h>
#include <task_ble.h>
#include <memory.h>
#include "utils.h"

extern "C" {


namespace {


char g_buf[256];
int g_buf_idx = 0;
class BleTest;
class BleTest *s_instance;

class BleTest : public ::testing::Test {
  protected:
    BleTest() {
    }
    virtual ~BleTest() {
    }
    virtual void SetUp() {
        memset(g.pipes_open, '\xff', sizeof(g.pipes_open));

        memset(g_buf, '\x00', sizeof(g_buf));

        s_instance = this;
        g_buf_idx = 0;
    }
    virtual void TearDown() {
        s_instance = nullptr;
    }
};
}


void pin_clear(int, int);

void spi_lock(void *ptr);

void spi_lock(void *ptr)
{
    (void) ptr;
}

int xQueueSend(void *queue, void *cmd, int delay);

int xQueueSend(void *queue, void *cmd, int delay)
{
    uint8_t *data = (uint8_t*)cmd;
    // std::cout << "Sending on pipe: " << data[0] << "\n";
    memcpy(g_buf+g_buf_idx, cmd, sizeof(nrf8001_cmd_s));
    g_buf_idx += sizeof(nrf8001_cmd_s);
    // hexDump("foo", data, sizeof(nrf8001_cmd_s));
    return pdPASS;
}

void pin_clear(int, int)
{
    return;
}

}

namespace {

TEST_F(BleTest, CorrectLength) {
    uint8_t msg[] = {1, 2, 3, 4, 5};

    ble_tx_head(5, 0x20, msg, sizeof(msg));
    // Length is fine
    ASSERT_EQ(g_buf[0], sizeof(msg) + 3);
}

TEST_F(BleTest, CorrectId) {
    uint8_t msg[] = {1, 2, 3, 4, 5};

    ble_tx_head(5, 0x20, msg, sizeof(msg));
    // Make sure senddata is selected (0x15)
    ASSERT_EQ(g_buf[1], 0x15);
}

TEST_F(BleTest, CorrectPipe) {
    uint8_t msg[] = {1, 2, 3, 4, 5};

    ble_tx_head(5, 0x20, msg, sizeof(msg));

    ASSERT_EQ(g_buf[2], 5);
}


TEST_F(BleTest, CorrectHeaderByte) {
    uint8_t msg[] = {1, 2, 3, 4, 5};

    ble_tx_head(5, 0x20, msg, sizeof(msg));

    ASSERT_EQ(g_buf[3], 0x20);
}

TEST_F(BleTest, CorrectlySplitsMessage) {
    uint8_t msg[30];

    for (int i = 0; i < sizeof(msg); i++) {
        msg[i] = i;
    }

    ble_tx_head(5, 0x20, msg, sizeof(msg));

    // Make sure we have a full first message (header byte, 19 data bytes + 2 header)
    ASSERT_EQ(g_buf[0], 22); 

    // Make sure we stored a remainder in the second message (remaining 11 bytes, + 2 header)
    ASSERT_EQ(g_buf[sizeof(nrf8001_cmd_s)], 13);

    // Make sure the second message starts with the correct byte (19)
    ASSERT_EQ(g_buf[sizeof(nrf8001_cmd_s) + 3], 19);
}

}


