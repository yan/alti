
#ifndef __NRF8001_H
#define __NRF8001_H

#include <stdint.h>

struct nrf8001_cmd_s {
  uint8_t length;
  uint8_t opcode;
  uint8_t data[30];
} __attribute__((packed));

#define NRF8001_MAX_CMD_LENGTH  (sizeof(((struct nrf8001_cmd_s*)0)->data) + 1)

#define NRF8001_SPI    SPI1

void nrf8001_exchange_cmds(struct nrf8001_cmd_s *out, struct nrf8001_cmd_s *in);

void nrf8001_handle_event(struct nrf8001_cmd_s *event);

#endif
