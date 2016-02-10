#include <hal.h>
#include <pins.h>
#include <config.h>
#include <ublox.h>

#include <rtos.h>
#include <ublox_isr.h>
#include <task_gps.h>

#if CONFIG_USE_USART_ISR

extern void usart1_isr(void);

/**
 * @brief ISR that's hit when there's new USART data waiting to be read. Reads 
 * a byte into a global buffer. This unfortunately has some parsing logic of 
 * UBX message types such that it only gives the semaphore after it received a
 * full message. This is done in order to minimize power consumption.
 *
 * TODO: Update this message and figure out how to not overwrite this buffer
 * before the receiving code finished reading it.
 *
 * XXX: Make sure the recv port is correct
 */
void usart1_isr(void)
{
  struct usart_isr_state_s *s = &g.usart_isr_state;
  uint8_t value;

  /* Do nothing if there's an error condition */
  if (!usart_can_recv(UBLOX_UART)) {
    return;
  }

  value = arch_usart_recv(UBLOX_UART);

#define RESET_STATE do {                          \
  s->state = USART_ISR_STATE_WAITING;             \
  s->read_offset = s->write_offset;               \
} while(0)

  switch (s->state) {
    case USART_ISR_STATE_WAITING:
      if (value == UBX_SYNC_BYTE_1) {
        /**
         * XXX: Consider resetting the read state here as well
         */
        s->state = USART_ISR_STATE_READ_SYNC1;
      } else {
        /* If we're waiting for the first sync byte, and it's not it, just
         * ignore the character
         * */
      }
      return; /** Don't save the sync byte */

    case USART_ISR_STATE_READ_SYNC1:
      if (value == UBX_SYNC_BYTE_2) {
        s->state++;
      } else {
        s->state = USART_ISR_STATE_WAITING;
        /* Failed two sync bytes, again, ignore the character */
      }
      return; /** Don't save the sync byte */

    case USART_ISR_STATE_READ_SYNC2:
      //Checksum only covers msg_class and onwards
      s->running_checksum[0] = 0;
      s->running_checksum[1] = 0;
      /** 
       * Reset the read state here as well as we're reading a new message. We're
       * assuming a single message in flight at a time
       */
      s->read_offset = 0;
      s->write_offset = 0;
      s->state++;
      break;
    case USART_ISR_STATE_READ_CLASS:
      s->state++;
      break;
    case USART_ISR_STATE_READ_ID:
      s->remaining = value;
      s->state++;
      break;
    case USART_ISR_STATE_READ_LSB_LEN:
      s->remaining += value << 8;

      /* Cap the message size we're capable of reading to the buffer size.
       * This is a good place to put a breakpoint in case messages of that size
       * actually happen
       */
      if (s->remaining > USART_ISR_BUFFER_LEN) {
        RESET_STATE;
        return;
      }

      s->state++;
      break;

    case USART_ISR_STATE_READING: 
        s->remaining--;
        if (s->remaining == 0) {
          s->state++;
        }
        break;
    case USART_ISR_STATE_CK1:
        if (s->running_checksum[0] == value) {
          s->state++;
        } else {
          RESET_STATE;
          return;
        }
        return;
    case USART_ISR_STATE_CK2: {
        BaseType_t higher_awoken, event;
        if (s->running_checksum[1] == value) { 
          // we're done reading, reset state, counters, and give semaphore
          s->state = USART_ISR_STATE_WAITING;
          // xSemaphoreGiveFromISR(g.usart_mutex_g, &higher_awoken);
          event = EVT_GPS_UBX_WAITING;
          xQueueSendToFrontFromISR(g.gps_queue_g, &event, &higher_awoken);
          portYIELD_FROM_ISR(higher_awoken);
        } else {
          RESET_STATE;
        }
      }
      return;
  }

  s->running_checksum[0] = s->running_checksum[0] + value; 
  s->running_checksum[1] = s->running_checksum[1] + s->running_checksum[0]; 

  if (s->write_offset < USART_ISR_BUFFER_LEN) {
    s->buffer[s->write_offset] = value; 
    s->write_offset++;
  }
}

/**
 * @brief Return the next message from the wire. Can not return null
 */
struct ubx_header_s *ublox_wait_for_message(void)
{
  BaseType_t event, status;

  /* We must enter a critical section to make sure no new events get pushed
   * between peek and receive.
   *
   * The GPS queue is likely empty when peek is called, so it should block
   * for the next message to arrive.
   * */

  for (;;)
  {
    taskDISABLE_INTERRUPTS();

    status = xQueuePeek(g.gps_queue_g, &event, portMAX_DELAY);

    if (status == pdPASS && event == EVT_GPS_UBX_WAITING) {
      break;
    }

    taskENABLE_INTERRUPTS();

    delay_ms(5);
  }

  status = xQueueReceive(g.gps_queue_g, &event, portMAX_DELAY);

  assert(status == pdPASS);

  taskENABLE_INTERRUPTS();

  return ublox_get_incoming_message();
}

/**
 * 
 */
struct ubx_header_s *ublox_get_incoming_message(void)
{
  return (struct ubx_header_s *) g.usart_isr_state.buffer;
}
 
#if 0
uint16_t ublox_wait_for_message(usart_t port)
{
  uint16_t value;
#if CONFIG_USE_USART_ISR
  (void) port;
  // BaseType_t status;

#if 0
  if (g.usart_isr_state.read_offset == g.usart_isr_state.until_offset) {
    // Blocking here should fill up the buffer again
    status = xSemaphoreTake(g.usart_mutex_g, portMAX_DELAY);
    if (status != pdPASS) {
      // TODO: Is this reachable with portMAX_DELAY? What do we do here?
    }
  }
#endif

  /* Make sure we filled the receive buffer */
  // assert(g.usart_isr_state.read_offset != g.usart_isr_state.write_offset);
    
  portENTER_CRITICAL();

  value = (uint8_t) g.usart_isr_state.buffer[g.usart_isr_state.read_offset++];
  g.usart_isr_state.read_offset %= USART_ISR_BUFFER_LEN; 

  portEXIT_CRITICAL();

#else
  value = usart_recv_blocking(port);
#endif
  return value;
}
#endif

#endif // CONFIG_USE_USART_ISR
