/**
 * Copyright 2015 Yan Ivnitskiy
 */


#ifndef __NRF8001_H
#define __NRF8001_H

#include <stdint.h>

struct nrf8001_cmd_s {
  uint8_t length;
  uint8_t opcode;
  uint8_t data[30];
} __attribute__((packed));

/** @brief Connect interval, in 0.625ms */
#define NRF8001_CONNECT_INTERVAL         255 // 160 ms
/** @brief Connect timeout, in seconds */
#define NRF8001_CONNECT_TIMEOUT          0

#define NRF8001_MAX_CMD_LENGTH  (sizeof(((struct nrf8001_cmd_s*)0)->data) + 1)

/** XXX: Is this right? */
#define NRF8001_MAX_DATA_LENGTH   20

#define PIPE_OPEN(p) (g.pipes_open[(p)/8] & (1 << (p % 8)))

#define PIPE_CLOSED(p) (g.pipes_closed[(p)/8] & (1 << (p % 8)))

void config_nrf8001(void);

void nrf8001_isr(void);

void nrf8001_exchange_cmds(struct nrf8001_cmd_s *out, struct nrf8001_cmd_s *in);

void nrf8001_handle_event(struct nrf8001_cmd_s *event);

#endif // __NRF8001_H
