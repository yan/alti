
#ifndef __GLOBALS_H
#define __GLOBALS_H

#include <stdint.h>

extern uint32_t rcc_clock_freq;

extern void *main_queue_g;

extern void *status_queue_g;

extern struct ble_task_data_s *ble_data_g;

#endif
