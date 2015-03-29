
#ifndef __TASK_BLE_H
#define __TASK_BLE_H

#include <nrf8001.h>

typedef struct ble_task_data_s {
  void *in;
  void *semphr;
} ble_task_data_t;

void task_ble(void *p);
void ble_send_cmd(struct nrf8001_cmd_s *cmd);

#endif
