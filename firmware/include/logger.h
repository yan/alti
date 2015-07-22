/**
 * Copyright 2015 Yan Ivnitskiy
 */
#ifndef __LOGGER_H
#define __LOGGER_H

#include <FreeRTOS.h>
#include <task.h>
#include "ms5611.h"

struct sensor_packet_s {
  uint32_t ticks;
  ms5611_mbarc_t pressure;
};

void write_sensor_packet(struct sensor_packet_s *packet);



#endif // __LOGGER_H
