
#ifndef __TASK_BLE_H
#define __TASK_BLE_H

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

typedef struct ble_task_data_s {
  QueueHandle_t in, out;
  SemaphoreHandle_t semphr;
} ble_task_data_t;

void task_ble(void *p);


#endif
