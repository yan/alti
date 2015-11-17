/**
 * Copyright 2015 Yan Ivnitskiy
 */
#ifndef __LOGGER_H
#define __LOGGER_H

#include <FreeRTOS.h>
#include <task.h>
#include <ms5611.h>
#include <config.h>


struct sensor_packet_s {
  uint32_t ticks;
  ms5611_mbarc_t mbarc;

#if CONFIG_USE_GPS
  int32_t ecef_x;
  int32_t ecef_y;
  int32_t ecef_z;
#endif

#if USE_ACCEL
  int16_t accel;
  int16_t mag;
  int16_t gy;
#endif

};

void write_sensor_packet(struct sensor_packet_s *packet);



#endif // __LOGGER_H
