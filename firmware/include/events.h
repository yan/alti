/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __EVENTS_H
#define __EVENTS_H

#include <stdint.h>
#include <sample.h>
#include <nrf8001.h>

/**
 * @brief All possible events that can be pushed to the main "thread"
 */
typedef enum event_type_e {
  GLOBAL_EVT_RESET,

  /** @brief Marks that we should sample sensors and log data */
  GLOBAL_EVT_SHOULD_SAMPLE,

  GLOBAL_EVT_SENSOR_ACCEL,
  GLOBAL_EVT_SENSOR_GYRO,
  GLOBAL_EVT_SENSOR_MAG,
  GLOBAL_EVT_SENSOR_BARO,
  GLOBAL_EVT_SENSOR_GPS,
  GLOBAL_EVT_SENSOR_COMPLETE,

  GLOBAL_EVT_NRF8001_FIRST,
  GLOBAL_EVT_NRF8001_RDY,
  GLOBAL_EVT_NRF8001_EVENT,
  GLOBAL_EVT_NRF8001_DATA_RECEIVED,
  GLOBAL_EVT_NRF8001_PIPES_CHANGED,
  GLOBAL_EVT_NRF8001_LAST,

  GLOBAL_EVT_GPS_START,
  GLOBAL_EVT_GPS_STARTED,
  GLOBAL_EVT_GPS_FINISH,

  GLOBAL_EVT_LAST
} event_type_t;


typedef union {
#if CONFIG_USE_ACCEL
  /** @brief Accelerometer result data */
  struct accel_sample_s accel_sample;
#endif

#if CONFIG_USE_GYRO
  struct gyro_sample_s gyro_sample;
#endif

#if CONFIG_USE_MAG
  struct mag_sample_s mag_sample;
#endif

  /** @brief Barometric sensor data */
  struct baro_sample_s baro_sample;

#if CONFIG_USE_GPS
  /** @brief A sample of GPS position data */
  struct gps_sample_s gps_sample;
#endif

  /** @brief BLE packet */
  struct nrf8001_cmd_s nrf8001_cmd;

  /** @brief Can be anything */
  void *data;
} event_payload_t;

#if 0
#if TESTING
// typedef uint32_t event_payload_t;
typedef void *event_payload_t;
#else
typedef void *event_payload_t;
#endif
#endif

/**
 *
 */
struct global_event_s {
  enum event_type_e type;
  event_payload_t payload;
};

#endif
