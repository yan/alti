


#ifndef __SAMPLE_H
#define __SAMPLE_H

#include <bmx055.h>
#include <ms5611.h>
#include <config.h>
#include <features.h>

#if CONFIG_USE_GPS
struct gps_sample_s {
  int32_t lat, lon;
  int32_t ground_speed;
  int32_t heading;
  int32_t accuracy;
} __attribute__((packed));
#endif

#if CONFIG_USE_ACCEL
struct accel_sample_s {
  int16_t accel[3];
} __attribute__((packed));
#endif

struct baro_sample_s {
  ms5611_mbarc_t mbarc;
} __attribute__((packed));

#if CONFIG_USE_GYRO
struct gyro_sample_s {
  int16_t gyro[3];
} __attribute__((packed));
#endif

#if CONFIG_USE_MAG
struct mag_sample_s {
  int16_t mag[3]; /* TODO: Fix naming to make sure it matches mag data */
} __attribute__((packed));
#endif

/**
 * @brief The header for an 'event'. Likely a jump.
 */
struct event_header_s {
  /** @brief The unique id of this event */
  uint32_t event_id;
  /** @brief Number of samples associated with this event */
  uint32_t samples;
  /** @brief Size of each sample */
  uint16_t sample_size;
  /** @brief Bitmask representing what features were enabled during logging */
  uint16_t features;
  /** @brief Milliseconds since Jan 1, 1970 to the start of this event */
  uint64_t rtc_start;

};

/**
 * @brief
 */
struct sensor_packet_s {
  uint32_t ticks;

  ms5611_mbarc_t mbarc;

#if CONFIG_USE_GPS
  struct gps_sample_s gps_sample;
#endif

#if CONFIG_USE_ACCEL
  struct accel_sample_s accel_sample;
#endif

#if CONFIG_USE_GYRO
  struct gyro_sample_s gyro_sample;
#endif

#if CONFIG_USE_MAG
  struct mag_sample_s mag_sample;
#endif

};

#endif // __SAMPLE_H
