
/**
 * Copyright 2015 Yan Ivnitskiy
 */


#if defined(CONFIG_USE_GPS) && ( CONFIG_USE_GPS == 1 )

#ifndef __TASK_GPS_H
#define __TASK_GPS_H

void config_gps(void);

void task_gps(void *p);


#endif // __TASK_GPS_H
#endif // CONFIG_USE_GPS
