/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __TASK_SENSOR_H
#define __TASK_SENSOR_H

#include <stdint.h>

typedef uint16_t sensor_event_t;

#define SENSOR_REQUEST_AIR_PRESSURE                                   ( 0x0001 )
#define SENSOR_REQUEST_ACCEL                                          ( 0x0002 )
#define SENSOR_REQUEST_MAG                                            ( 0x0004 )
#define SENSOR_REQUEST_GYRO                                           ( 0x0008 )
#define SENSOR_REQUEST_STOP                                           ( 0x8000 )

void config_sensor(void);

void task_sensor(void *p);


#endif
