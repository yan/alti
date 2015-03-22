
#ifndef __NRF8001_H
#define __NRF8001_H

#include <stdint.h>

struct nrf8001_cmd_s {
  uint8_t length;
  uint8_t opcode;
  uint8_t data[30];
};

#endif
