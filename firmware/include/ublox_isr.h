#ifndef __UBLOX_ISR_H
#define __UBLOX_ISR_H

#define USART_ISR_BUFFER_LEN    128


#include <ublox.h>


struct ubx_header_s *ublox_wait_for_message(void);

struct ubx_header_s *ublox_get_incoming_message(void);

enum state_e {
  USART_ISR_STATE_WAITING,
  USART_ISR_STATE_READ_SYNC1,
  USART_ISR_STATE_READ_SYNC2,
  USART_ISR_STATE_READ_CLASS,
  USART_ISR_STATE_READ_ID,
  USART_ISR_STATE_READ_LSB_LEN,
  USART_ISR_STATE_READING,
  USART_ISR_STATE_CK1,
  USART_ISR_STATE_CK2,
};

struct usart_isr_state_s {
  /* The actual usart buffer */
  uint8_t buffer[USART_ISR_BUFFER_LEN];

  /* Offset where the ISR is writing to */
  uint8_t write_offset;

  /* Offset where the receiving call is reading from */
  uint8_t read_offset;

  /* How many bytes left to read for the ISR before finishing the UBX message
   * and giving the mutex.
   */
  uint16_t remaining;

  uint8_t running_checksum[2];

  /* The UBX message reading state that the ISR can be in */
  enum state_e state;
};
#endif
