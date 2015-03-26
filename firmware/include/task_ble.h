
#ifndef __TASK_BLE_H
#define __TASK_BLE_H

#include <FreeRTOS.h>
#include <semphr.h>
#include <queue.h>

#include <nrf8001.h>

typedef struct ble_task_data_s {
  QueueHandle_t in;
  SemaphoreHandle_t semphr;
} ble_task_data_t;

void task_ble(void *p);
void ble_send_cmd(struct nrf8001_cmd_s *cmd);

#endif
