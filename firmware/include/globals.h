
#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <task_ble.h>
#include <ble.h>

struct protected_buffer_s {
  void *lock;
  uint8_t data[512];
};

struct globals {

  /** @brief FreeRTOSConfig.h uses this to refer to the clock rate */
  uint32_t rcc_clock_freq;
  
  /** @brief The main queue onto which events get pushed from ISRs and tasks */
  void *main_queue_g;
  
  /** @brief XXX */
  void *alert_queue_g;

  /** @brief */
  void *baro_queue_g;

  /** @brief Byte order of SPI links.
   *    Bit 1 set = MSB for SPI1
   *    Bit 2 set = MSB for SPI2
   */
  uint8_t spi_endian;

  /** @brief Data local to the BLE task */
  struct ble_task_data_s *ble_data_g;

  /** @brief Flash read/write buffer */
  struct protected_buffer_s flash_buffer;

  /** @brief Bitmasks of open and closed nRF8001 pipes */
  uint8_t pipes_open[8], pipes_closed[8];
};

extern struct globals g;

#endif
