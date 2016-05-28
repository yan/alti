
/**
 * Copyright 2015 Yan Ivnitskiy
 */


#if CONFIG_USE_GPS

#ifndef __TASK_GPS_H
#define __TASK_GPS_H

enum gps_event_t {
  EVT_GPS_RESET,
  EVT_GPS_UBX_WAITING,
  EVT_GPS_CFG,
  EVT_GPS_START,
  EVT_GPS_SLEEP,
  EVT_GPS_UPDATE_RTC
};

void config_gps(void);

void task_gps(void *p);

extern uint32_t g_ticks;

#endif // __TASK_GPS_H
#endif // CONFIG_USE_GPS
