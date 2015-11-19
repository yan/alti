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

#define WRITE_BUFFER_LEN        512

#define USART_ISR_BUFFER_LEN    128


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

  /** @brief */
  void *gps_queue_g;

#if CONFIG_USE_USART_ISR

  /* USART interupt mutex for gps */
  void *usart_mutex_g;

  /* The buffer and offsets into which the usart interrupt writes data */
  struct usart_isr_state_s {
    /* The actual usart buffer */
    uint8_t buffer[USART_ISR_BUFFER_LEN];

    /* Offset where the ISR is writing to */
    uint8_t write_offset;

    /* Offset where the receiving call is reading from */
    uint8_t read_offset;

    /* Marked when the ISR finishes reading a UBX message. Separate from
     * |write_offset| since the ISR can write more data while the receiving task
     * is still not done reading a UBX message. Right now, the mutex marks a 
     * complete UBX message, not just finished data, although they will probably
     * be identicaly
     */
    uint8_t until_offset;

    /* How many bytes left to read for the ISR before finishing the UBX message
     * and giving the mutex.
     */
    uint16_t remaining;

    uint8_t running_checksum[2];

    /* The UBX message reading state that the ISR can be in */
    enum {
      USART_ISR_STATE_WAITING,
      USART_ISR_STATE_READ_SYNC1,
      USART_ISR_STATE_READ_SYNC2,
      USART_ISR_STATE_READ_CLASS,
      USART_ISR_STATE_READ_ID,
      USART_ISR_STATE_READ_LSB_LEN,
      USART_ISR_STATE_READING,
      USART_ISR_STATE_CK1,
      USART_ISR_STATE_CK2,

    } read_state;

  } usart_isr_state;
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

#endif
