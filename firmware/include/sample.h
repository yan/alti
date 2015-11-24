


#ifndef __SAMPLE_H
#define __SAMPLE_H

#include <bmx055.h>
#include <ms5611.h>
#include <config.h>

struct gps_sample_s {
  int32_t lat, lon;
  int32_t ground_speed;
  int32_t heading;
  int32_t accuracy;
} __attribute__((packed));

struct accel_sample_s {
  int16_t accel[3];
}__attribute__((packed));

struct baro_sample_s {
  ms5611_mbarc_t mbarc;
}__attribute__((packed));

struct sensor_packet_s {
  uint32_t ticks;

  ms5611_mbarc_t mbarc;

#if CONFIG_USE_GPS
  struct gps_sample_s gps_sample;
#endif

#if CONFIG_USE_ACCEL
  struct accel_sample_s accel_sample;
#endif

};

#endif // __SAMPLE_H
