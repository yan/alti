
#ifndef __NRF8001_H
#define __NRF8001_H

#include <stdint.h>

struct nrf8001_cmd_s {
  uint8_t length;
  uint8_t opcode;
  uint8_t data[30];
} __attribute__((packed));

#define NRF8001_MAX_CMD_LENGTH  (sizeof(((struct nrf8001_cmd_s*)0)->data) + 1)

#define NRF8001_SPI    SPI2

void nrf8001_exchange_cmds(struct nrf8001_cmd_s *out, struct nrf8001_cmd_s *in);
void nrf8001_handle_event(struct nrf8001_cmd_s *event);

typedef enum nrf8001_cmd_e {
  NRF8001_CMD_SETUP                       = 0x06,
  NRF8001_CMD_CONNECT                     = 0x0f

} nrf8001_cmd_t;

typedef enum nrf8001_evt_type_e {
  /** Invalid event code */
  NRF8001_EVT_INVALID                     = 0x00,

  /** Sent every time the device starts */
  NRF8001_EVT_DEVICE_STARTED              = 0x81,

  /** Mirrors the CMD_ECHO */
  NRF8001_EVT_ECHO                        = 0x82,

  /** Asynchronous hardware error event */
  NRF8001_EVT_HW_ERROR                  = 0x83,

  /** Event opcode used as a event response for all commands */
  NRF8001_EVT_CMD_RSP                     = 0x84,

  /** Link connected */
  NRF8001_EVT_CONNECTED                   = 0x85,

  /** Link disconnected */
  NRF8001_EVT_DISCONNECTED                = 0x86,
  /** Bond completion result */
  NRF8001_EVT_BOND_STATUS                 = 0x87,

  /** Pipe bitmap for available pipes */
  NRF8001_EVT_PIPE_STATUS             = 0x88,

  /** Sent to the application when the radio enters a connected state  or when
   * the timing of the radio connection changes */
  NRF8001_EVT_TIMING                      = 0x89,

  /** Notification to the application that transmit credits are available */
  NRF8001_EVT_DATA_CREDIT                 = 0x8A,

  /** Data acknowledgement event */
  NRF8001_EVT_DATA_ACK                    = 0x8B,

  /** Data received notification event */
  NRF8001_EVT_DATA_RECEIVED               = 0x8C,

  /** Error notification event */
  NRF8001_EVT_PIPE_ERROR                  = 0x8D,

  /** Display Passkey Event */
  NRF8001_EVT_DISPLAY_PASSKEY             = 0x8E,

  /** Security Key request */
  NRF8001_EVT_KEY_REQUEST                 = 0x8F

} nrf8001_evt_type_t;

#endif
