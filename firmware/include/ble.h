
#ifndef __BLE_H
#define __BLE_H

#include <FreeRTOS.h>
//#include <task.h>

void config_ble(void);
void ble_isr(BaseType_t *higher);

#define PIPE_OPEN(p) \
   (g.pipes_open[(p)/8] & (1 << (p % 8)))

#define PIPE_CLOSED(p) \
   (g.pipes_closed[(p)/8] & (1 << (p % 8)))

#endif
