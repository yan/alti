/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __TASK_BLE_H
#define __TASK_BLE_H

#include <nrf8001.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int  ble_send_cmd(struct nrf8001_cmd_s *cmd);
void ble_tx(uint8_t pipe, uint8_t *data, size_t length);
void ble_tx_head(uint8_t pipe, uint8_t head, uint8_t *data, size_t length);
void task_ble(void *p);

#ifdef __cplusplus
}
#endif

#endif // __TASK_BLE_H
