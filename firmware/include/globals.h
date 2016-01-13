/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>
#include <filter.h>
#include <alarm.h>
#include <config.h>
#include <features.h>
#include <counters.h>
#include <flash.h>

#if CONFIG_USE_GPS
#include <ublox_isr.h>
#endif

// #define WRITE_BUFFER_LEN        512
#define WRITE_BUFFER_LEN         STORAGE_PAGE_SIZE

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A simple block-length buffer protected by a lock. Also includes a 
 * write offset to make writing easier. XXX: Probably should move w_o out.
 */
struct protected_buffer_s {
  void *lock;
  unsigned int address;
  struct {
    unsigned int dirty : 1;
    unsigned int write_offset : sizeof(unsigned int) * 8 - 1;
  };
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


struct ble_task_data_s {
  void *in;
  void *semphr;
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

#if CONFIG_USE_GPS
  /** @brief */
  void *gps_queue_g;
#endif

#if CONFIG_USE_COUNTERS
  struct counters_s counters;
#endif

#if CONFIG_USE_GPS && CONFIG_USE_USART_ISR
  /* USART interupt mutex for gps */
  void *usart_mutex_g;

  /* The buffer and offsets into which the usart interrupt writes data */
  struct usart_isr_state_s usart_isr_state;
#endif

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

#ifdef __cplusplus
}
#endif

#endif
