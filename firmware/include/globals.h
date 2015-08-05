/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <filter.h>
#include <alarm.h>

#define WRITE_BUFFER_LEN  512

/**
 * @brief A simple block-length buffer protected by a lock. Also includes a 
 * write offset to make writing easier. XXX: Probably should move w_o out.
 */
struct protected_buffer_s {
  void *lock;
  unsigned int write_offset;
  uint8_t data[WRITE_BUFFER_LEN];
};

/**
 * @brief Configuration of the altimeter that is persisted to non volatile
 * storage.
 */
struct persisted_config_s {
  char name[16];
  struct alarm_s alarms[ALARM_LEN];
  uint32_t logged_jumps;
  uint32_t last_written_jump;
} __attribute__((packed));

typedef struct ble_task_data_s {
  void *in;
  void *semphr;
} ble_task_data_t;

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

  /** @brief Data local to the BLE task */
  struct ble_task_data_s *ble_data_g;

  /** @brief Flash read/write buffer */
  struct protected_buffer_s flash_buffer;

  /** @brief Persisted config */
  struct persisted_config_s persisted_config;

  /** @brief Bitmasks of open and closed nRF8001 pipes */
  uint8_t pipes_open[8], pipes_closed[8];
};

extern struct globals g;

#endif
