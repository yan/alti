
#include <stdint.h>
#include <stddef.h>
#include <ublox.h>
#include <pins.h>
#include <util.h>

#if defined(STM32L1)
#  include <hal.h>
#endif

#define MAX_RECEIVED 1024

#define MAX_RETRIES (5)

static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest);
static int    ublox_expect_response(usart_t port, uint8_t msg_class, uint8_t msg_id);
static void   ublox_send(uint8_t class_id, uint8_t msg_id, uint8_t *buf, size_t length);
static size_t usart_recv_buf(usart_t port, uint8_t *dest, size_t length);
static void   usart_send_buf(usart_t port, uint8_t *buf, size_t length);

static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest) {
  uint8_t a = dest[0];
  uint8_t b = dest[1];
  uint8_t i = 0;

  assert(dest != NULL);

  for (i = 0; i < length; i++) {
    a = a + in[i];
    b = b + a;
  }

  dest[0] = (a & 0xFF);
  dest[1] = (b & 0xFF);
}

static void usart_send_buf(usart_t port, uint8_t *buf, size_t length)
{
  unsigned i = 0;
  for (i = 0; i < length; i++ ) {
    arch_usart_send(port, buf[i]);
  }
}

static size_t usart_recv_buf(usart_t port, uint8_t *dest, size_t length)
{
  unsigned i = 0;

  if (dest == NULL) {
    return 0;
  }

  for (i = 0; i < length; i++) {
    dest[i] = arch_usart_recv(port);
  }

  return length;
}

static void ublox_send(uint8_t class_id, uint8_t msg_id, uint8_t *buf, size_t length)
{
  union {
    struct ubx_header_s msg;
    uint8_t msg_buf[6];
  } u;

  uint16_t checksum = 0;

  u.msg.sync1 = UBX_SYNC_BYTE_1;
  u.msg.sync2 = UBX_SYNC_BYTE_2;
  u.msg.msg_class = class_id;
  u.msg.msg_id = msg_id;
  u.msg.length = length;

  // Checksum header
  calculateCheckSum(&u.msg_buf[2], 4, (uint8_t*)&checksum);

  // Checksum buffer
  calculateCheckSum(buf, length, (uint8_t*)&checksum);

  usart_send_buf(UBLOX_UART, u.msg_buf, 6);
  usart_send_buf(UBLOX_UART, buf, length);
  usart_send_buf(UBLOX_UART, (uint8_t*)&checksum, sizeof(checksum));
}


/**
 * @brief Read a UBX message header
 *
 * @return 1 on success, 0 on failure
 */
static int ublox_get_next_header(usart_t port, struct ubx_header_s *head)
{
  if (head == NULL) {
    return 0;
  }

  do {
    head->sync1 = arch_usart_recv(port);
    if (head->sync1 != UBX_SYNC_BYTE_1) {
      continue;
    }

    head->sync2 = arch_usart_recv(port);
    if (head->sync2 != UBX_SYNC_BYTE_2) {
      continue;
    }

    break;
  } while (1);

  head->msg_class = arch_usart_recv(port);
  head->msg_id = arch_usart_recv(port);

  /* Then, get the length of the payload */
  head->length = arch_usart_recv(port);
  head->length += arch_usart_recv(port) << 8;

  return 1;
}

static int ublox_expect_response(usart_t port, uint8_t msg_class, uint8_t msg_id)
{
  int attempts;
  struct ubx_header_s header;

  for (attempts = 0; attempts < MAX_RETRIES; attempts++) {
    if (!ublox_get_next_header(port, &header)) {
      continue;
    }

    if (header.msg_class != msg_class || header.msg_id != msg_id) {
      continue;
    }

    break;
  }

  return !!attempts;
}
/**
 *
 *
 */
static int ublox_ping(void)
{
  struct ubx_header_s header;
  int received = 0;

  ublox_send(MSG_CLASS_MON, MSG_ID_MON_VER, NULL, 0);

  delay_ms(500);

  if (!ublox_get_next_header(UBLOX_UART, &header)) {
    return 0;
  }

  if (header.msg_class != MSG_CLASS_MON) {
    return 0;
  }

  if (header.msg_id != MSG_ID_MON_VER) {
    return 0;
  }

#define GET_ZT_STRING(bytes)                                   \
  {                                                            \
    volatile int i, j, appending;                              \
    for (appending = 1, i = 0; i < bytes; i++, received++) {   \
      j = arch_usart_recv(UBLOX_UART);                         \
      if (j == 0) {                                            \
        appending = 0;                                         \
      }                                                        \
      if (appending) {                                         \
        /* DO SOMETHING WITH THE BYTES HERE */                 \
      }                                                        \
    }                                                          \
  }

  /* Get the software version */
  GET_ZT_STRING(30);

  /* Get the hw version */
  GET_ZT_STRING(10);

  /* Get extra fields */
  while (received < header.length) {
    GET_ZT_STRING(30);
  }
#undef GET_ZT_STRING

  return 1;
}

/**
 * @brief Try to disocver the ublox module.
 *
 * @return 1 on success, 0 if unable to ping one
 */
int ublox_init(void)
{
  int attempts = MAX_RETRIES;

  dbg_print("configuring ublox\n");
  delay_ms(200);
  while (attempts-- > 0) {
    if (ublox_ping()) {
      return 1;
    }
  }
  return 0;
}

int ublox_start_updates(int rate)
{
  uint8_t request[3] = {
    MSG_CLASS_NAV,
    MSG_ID_NAV_PVT,
    rate
  };

  dbg_print("asked to start sending updates at %d\n", rate);

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_MSG, request, sizeof(request));

  return ublox_expect_response(UBLOX_UART, MSG_CLASS_ACK, MSG_ID_ACK_ACK);
}

int ublox_get(void)
{
  struct ubx_header_s head;
  struct ubx_nav_pvt_solution_s body;

  if (!ublox_get_next_header(UBLOX_UART, &head)) {
    return 0;
  }

  usart_recv_buf(UBLOX_UART, (uint8_t*)&body, sizeof(body));

  dbg_print("(%d %d): %d {%f, %f} (accuracy: %f) %zu\n", head.msg_class, head.msg_id, body.year, body.lat/1e7f, body.lon/1e7f, body.pDOP*0.01f, sizeof(body));

  return 1;
}
