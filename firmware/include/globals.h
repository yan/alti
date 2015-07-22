/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <task_ble.h>
#include <filter.h>

#define WRITE_BUFFER_LEN  512

struct protected_buffer_s {
  void *lock;
  unsigned int write_offset;
  uint8_t data[WRITE_BUFFER_LEN];
};

struct globals {

  /** @brief FreeRTOSConfig.h uses this to refer to the clock rate */
  uint32_t rcc_clock_freq;
  
  /** @brief The main queue onto which events get pushed from ISRs and tasks */
  void *main_queue_g;
  
  /** @brief XXX */
  void *alert_queue_g;

  /** @brief */
  void *sensor_queue_g;

  /** @brief The filter state for baro readings */
  filter_state_t baro_filter_state;

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
