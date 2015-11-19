/*
 *
 *
 *
 *
 */


#include <stdint.h>
#include <inttypes.h>
#include <stddef.h>
#include <ublox.h>
#include <pins.h>
#include <util.h>

#if defined(STM32L1) || defined(TESTING)
#  include <hal.h>
#endif

#define MAX_RECEIVED 1024

#define MAX_RETRIES (10)

static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest);
static int    ublox_expect_response(usart_t port, uint8_t msg_class, uint8_t msg_id, struct ubx_header_s *header);
static int    ublox_update_port_settings(usart_t port, uint32_t baud);
static void   ublox_send(uint8_t class_id, uint8_t msg_id, uint8_t *buf, size_t length);
static size_t usart_recv_buf(usart_t port, uint8_t *dest, size_t length);
static void   usart_send_buf(usart_t port, uint8_t *buf, size_t length);

#define ublox_poll(class_id, msg_id) ublox_send(class_id, msg_id, NULL, 0)

/**
 * Calculate using the 8-Bit Fletcher algorithm, as per UBX docs.
 *
 * @param in Pointer to the buffer
 * @param length The length of the buffer
 * @param dest Where to produce a checksum. This is also used as a checksum seed
 *   for calculating checksums in multiple steps.
 */
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

/**
 * Send a UBX message, with calculated checksum.
 *
 * @brief class_id The class id of the message
 * @brief msg_id The message is
 * @brief buf The data buffer of the body of the message
 * @brief length The length of the buffer
 */
static void ublox_send(uint8_t class_id, uint8_t msg_id, uint8_t *buf, size_t length)
{
  uint8_t sync_bytes[2] = { UBX_SYNC_BYTE_1, UBX_SYNC_BYTE_2 };
  union {
    struct ubx_header_s msg;
    uint8_t msg_buf[sizeof(struct ubx_header_s)];
  } u;

  uint16_t checksum = 0;

  u.msg.msg_class = class_id;
  u.msg.msg_id = msg_id;
  u.msg.length = length;

  // TODO: Try commenting this out
  usart_wait_send_ready(UBLOX_UART);

  usart_send_buf(UBLOX_UART, sync_bytes, sizeof sync_bytes);
  usart_send_buf(UBLOX_UART, u.msg_buf, sizeof u.msg_buf);
  calculateCheckSum(u.msg_buf, sizeof u.msg_buf, (uint8_t*)&checksum);
  if (length > 0) {
    usart_send_buf(UBLOX_UART, buf, length);
    calculateCheckSum(buf, length, (uint8_t*)&checksum);
  }
  usart_send_buf(UBLOX_UART, (uint8_t*)&checksum, sizeof(checksum));
}


/**
 * @brief Read a UBX message header
 *
 * @return 1 on success, 0 on failure
 */
static int ublox_get_next_header(usart_t port, struct ubx_header_s *head)
{
  uint8_t received;
  if (head == NULL) {
    return 0;
  }

  do {
    received = arch_usart_recv(port);
    if (received != UBX_SYNC_BYTE_1) {
      continue;
    }

    received = arch_usart_recv(port);
    if (received != UBX_SYNC_BYTE_2) {
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

/**
 * @brief Keep receiving UBX messages until we get one with correct msg_class
 * and msg_id.
 */
static int ublox_expect_response(usart_t port, uint8_t msg_class, uint8_t msg_id, struct ubx_header_s *header)
{
  int attempts;
  struct ubx_header_s h;

  for (attempts = MAX_RETRIES; attempts > 0; attempts--) {
    if (!ublox_get_next_header(port, &h)) {
      continue;
    }

    if (h.msg_class != msg_class || h.msg_id != msg_id) {
      continue;
    }

    break;
  }

  if (header != NULL) {
    *header = h;
  }

  return !!attempts;
}

/**
 * @brief Send a ping to a ublox module, to ensure it's alive.
 *
 * @return 1 if successfully pinged, 0 if not.
 */
static int ublox_ping(void)
{
  struct ubx_header_s header;
  int received = 0;

  ublox_poll(MSG_CLASS_MON, MSG_ID_MON_VER);

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
int ublox_init(uint32_t baudRate)
{
  int attempts = 0;
  int status = 0;

  ublox_reset();

  delay_ms(300);

  status = ublox_update_port_settings(UBLOX_MAX7_BUS, baudRate);

  if (status == 0) {
   return 0;
  }

  delay_ms(20);

  for (attempts = MAX_RETRIES; attempts > 0; attempts--) {
    if (ublox_ping()) {
      return 1;
    }
  }

  return 0;
}

/**
 * @brief Ask module to start sending navigational messages.
 * 
 * @return 1 on success, 0 on failure.
 */
int ublox_start_updates(int rate)
{
  uint8_t request[3] = {
    MSG_CLASS_NAV,
    MSG_ID_NAV_PVT,
    rate
  };

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_MSG, request, sizeof(request));

  return ublox_expect_response(UBLOX_UART, MSG_CLASS_ACK, MSG_ID_ACK_ACK, NULL);
}

int ublox_get_rate(void)
{
  int status;
  struct ubx_header_s head;
  uint16_t response[3];

  ublox_poll(MSG_CLASS_CFG, MSG_ID_CFG_RATE);

  status = ublox_expect_response(UBLOX_UART, MSG_CLASS_CFG, MSG_ID_CFG_RATE, &head);

  if (!status) {
    return status;
  }

  if (head.length != sizeof(response)) {
    return 0;
  }

  assert(head.length == sizeof(response));

  usart_recv_buf(UBLOX_UART, (uint8_t*)response, sizeof(response));

  dbg_print("Measuing rate is: %d ms, %d cycles, (time ref = %d)", 
      response[0], response[1], response[2]);

  return 1;
}

/**
 * @brief Receiving a navigation solution (blocking)
 */
int ublox_get(struct gps_sample_s *sample)
{
  struct ubx_header_s head;
  struct ubx_nav_pvt_solution_s body;

  assert(sample != NULL);

  if (!ublox_expect_response(UBLOX_UART, MSG_CLASS_NAV, MSG_ID_NAV_PVT, &head)) {
    return 0;
  }

  assert(sizeof(body) == head.length);

  usart_recv_buf(UBLOX_UART, (uint8_t*)&body, sizeof(body));

  sample->lat = body.lat;
  sample->lon =  body.lon; 
  sample->ground_speed = body.gSpeed;
  sample->heading = body.heading;
  sample->accuracy = body.pDOP;

  dbg_print("(%i %i ft: %i): %d {%d, %d}",
      head.msg_class, head.msg_id, body.fixType,
      body.year,
      (int) body.lat/*1e7f*/, (int) body.lon/*1e7f*/);

  return 1;
}

int ublox_reset(void)
{
#if defined(SOFT_RESET)

  struct {
    uint16_t navBbrMask;
    uint8_t resetMode;
    uint8_t reserved;
  } __attribute__((packed)) request = {
    0xFFFF,
    0x01,
    0x00
  };

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_RST, (uint8_t*) &request, sizeof(request));

#else

  pin_set(UBLOX_MAX7_RESET_GPIO, UBLOX_MAX7_RESET);
  pin_clear(UBLOX_MAX7_RESET_GPIO, UBLOX_MAX7_RESET);
  delay_ms(1);
  pin_set(UBLOX_MAX7_RESET_GPIO, UBLOX_MAX7_RESET);

#endif

  return 1;
}

/**
 * @brief Set the measuring rate of measurements.
 *
 * @brief ms The rate of measurement, in milliseconds
 */
int ublox_set_measuring_rate(uint16_t ms)
{
  uint16_t request[3] = {
    ms,
    1, /* Navitagion rate -- can not be changed, must be 1 */
    0 /* Alignment, 0 = UTC, 1 = GPS time (??) */
  };

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_RATE, (uint8_t*)request, sizeof(request));

  // XXX Do we get acks?
  return ublox_expect_response(UBLOX_UART, MSG_CLASS_ACK, MSG_ID_ACK_ACK, NULL);
}

static int ublox_get_power_mgmt(usart_t port)
{
  struct ubx_header_s head;
  struct ublox_cfg_pm2_s body;
  int status;

  ublox_poll(MSG_CLASS_CFG, MSG_ID_CFG_PM2);

  status = ublox_expect_response(port, MSG_CLASS_CFG, MSG_ID_CFG_PM2, &head);

  if (head.length != sizeof(body)) {
    return 0;
  }

  status = usart_recv_buf(port, (uint8_t*)&body, sizeof(body));

  return status;
}

static int ublox_update_port_settings(usart_t port, uint32_t baud)
{
  struct ubx_header_s head;
  struct ublox_prt_cfg_s config;
  uint8_t portConfig = 1;
  int status;


  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_PRT, &portConfig, sizeof(portConfig));

  status = ublox_expect_response(port, MSG_CLASS_CFG, MSG_ID_CFG_PRT, &head);

  if (head.length != sizeof(config)) {
    return 0;
  }

  status = usart_recv_buf(port, (uint8_t*)&config, sizeof(config));

  if (status == 0) {
    return 0;
  }

  /** TODO: Document getting the config via passing a 0 baud */
  if (baud == 0) {
    return 1;
  }

  config.outProtoMask = PROTO_UBX;
  config.baudRate = baud;

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_PRT, (uint8_t*)&config, sizeof(config));

  /** */
  while ((USART_SR(port) & USART_SR_TC) == 0)
    ;

  if (baud != 0) {
    usart_set_baudrate(port, baud);
  }

  return ublox_expect_response(UBLOX_UART, MSG_CLASS_ACK, MSG_ID_ACK_ACK, NULL);
}

/**
 * TODO: This also needs to use CFG-PM@ message.
 */
int ublox_sleep(void)
{
  uint8_t request[2] = {
    0,
    4 /* power save mode */
  };

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_RXM, (uint8_t*)request, sizeof(request));

  // XXX Do we get acks?
  return ublox_expect_response(UBLOX_UART, MSG_CLASS_ACK, MSG_ID_ACK_ACK, NULL);
}
