/**
 * Copyright 2016 Yan Ivnitskiy
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
#include <logger.h>
#include <nrf8001.h>

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
  // struct {
    unsigned int dirty : 1;
    unsigned int write_offset : sizeof(unsigned int) * 8 - 1;
  // };
  uint8_t data[WRITE_BUFFER_LEN];
};

struct coordinates_s {
  int32_t lat, lon;
};

#define HOME_DZ_LEN 64

/**
 * @brief Configuration of the altimeter that is persisted to non volatile
 * storage.
 */
struct storage_header_s {
  char name[16];
  char home_dz[HOME_DZ_LEN];
  struct alarm_s alarms[ALARM_LEN];
  struct coordinates_s geofences[ALARM_LEN]; // TODO: create a define for the len
  uint32_t logged_jumps;
  uint32_t last_event;
} __attribute__((packed));

STRUCT_SIZE_LT(head, struct storage_header_s, STORAGE_PAGE_SIZE);


struct ble_task_data_s {
  void *in;
  void *semphr;
};


struct port_lock_s {
    uint32_t port;
    void *semphr;
};

struct globals_s {
  /** @brief FreeRTOSConfig.h uses this to refer to the clock rate */
  uint32_t rcc_clock_freq;
  
  /** @brief The main queue onto which events get pushed from ISRs and tasks */
  void *main_queue_g;
  
  /** @brief XXX */
  void *alert_queue_g;

  /** @brief */
  void *sensor_queue_g;

  /** @brief */
  struct filter_state_s filter_state;

  /** @brief Currently logged event */
  struct event_s current_event_g;

  /** @brief Semaphores to protect shared ports (SPI, etc) */
  struct port_lock_s port_locks[2];

#if CONFIG_USE_GPS
  /** @brief */
  void *gps_queue_g;

  /** @brief */
  void *gps_isr_semphr_g;
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

  /** @brief A null command that we will use as filler. XXX Probably redundant */
  struct nrf8001_cmd_s nrf8001_nul;

  /** @brief Flash read/write buffer */
  struct protected_buffer_s flash_buffer;

  /** @brief Persisted config */
  struct storage_header_s persisted_config;

  /** @brief Bitmasks of open and closed nRF8001 pipes */
  uint8_t pipes_open[8], pipes_closed[8];
};



extern struct globals_s g;

#ifdef __cplusplus
}
#endif

#endif
