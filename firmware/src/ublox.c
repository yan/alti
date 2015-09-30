
#include <util.h>
#include <stddef.h>
#include <hal.h>
#include <pins.h>
#include <ublox.h>

#include <libopencm3/stm32/usart.h>


static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest);

static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest) {
  uint8_t a = 0;
  uint8_t b = 0;
  uint8_t i = 0;

  for (i = 0; i < length; i++) {
    a = a + in[i];
    b = b + a;
  }

  dest[0] = (a & 0xFF);
  dest[1] = (b & 0xFF);
}

static void ublox_poll(uint8_t class_id, uint8_t msg_id)
{
  union {
    struct ubx_header_s msg;
    uint8_t msg_buf[8];
  } u;

  unsigned int i;

  u.msg_buf[0] = UBX_SYNC_BYTE_1;
  u.msg_buf[1] = UBX_SYNC_BYTE_2;
  u.msg_buf[2] = class_id;
  u.msg_buf[3] = msg_id;
  u.msg_buf[4] = 0;
  u.msg_buf[5] = 0;
  /*
  u.msg.sync1 = UBX_SYNC_BYTE_1;
  u.msg.sync2 = UBX_SYNC_BYTE_2;
  u.msg.msg_class = class_id;
  u.msg.msg_id = msg_id;
  u.msg.length = 0;
  */

  calculateCheckSum(&u.msg_buf[2], 4, &u.msg_buf[6]);

  for (i = 0; i < sizeof(u.msg_buf); i++) {
    usart_send_blocking(UBLOX_UART, u.msg_buf[i]);
  }

}

static void ublox_ping(void)
{
  volatile int i, j ;


  dbg_print("ublox\n");
  ublox_poll(MSG_CLASS_MON, MSG_ID_MON_VER);

  dbg_print("sent ping\n");
  // delay_ms(200);


  for (i = 0; i < 8; i++) {
    j = usart_recv_blocking(UBLOX_UART);
    //dbg_print("Received: %d\n", );
  }
  i = j;

}

void ublox_init(void)
{
  dbg_print("configuring ublox\n");
  delay_ms(200);
  ublox_ping();
}

