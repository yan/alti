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
#include <ublox_isr.h>

//#if defined(STM32L1) || defined(TESTING)
#  include <hal.h>
//#endif

#define MAX_RECEIVED 1024

#define MAX_RETRIES (10)

/**
 * @brief Defined in ublox_isr.c
 */

static void calculateCheckSum(uint8_t *in, size_t length, uint8_t* dest);
static struct ubx_header_s *ublox_expect_response(uint8_t msg_class, uint8_t msg_id);
static int    ublox_update_port_settings(usart_t port, uint32_t baud);
static void   ublox_send(uint8_t class_id, uint8_t msg_id, uint8_t *buf, size_t length);
static void   usart_send_buf(usart_t port, const uint8_t *buf, size_t length);

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


static void usart_send_buf(usart_t port, const uint8_t *buf, size_t length)
{
  unsigned i = 0;
  for (i = 0; i < length; i++ ) {
    arch_usart_send(port, buf[i]);
  }
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
  const uint8_t sync_bytes[2] = { UBX_SYNC_BYTE_1, UBX_SYNC_BYTE_2 };
  struct ubx_header_s msg;

  uint8_t checksum[2] = {0};

  msg.msg_class = class_id;
  msg.msg_id = msg_id;
  msg.length = length;

  // TODO: Try commenting this out
  //     usart_wait_send_ready(UBLOX_UART);

  usart_send_buf(UBLOX_UART, (uint8_t*)sync_bytes, sizeof sync_bytes);
  usart_send_buf(UBLOX_UART, (uint8_t*)&msg, sizeof msg);
  calculateCheckSum((uint8_t*)&msg, sizeof msg, checksum);
  if (length > 0) {
    usart_send_buf(UBLOX_UART, buf, length);
    calculateCheckSum(buf, length, checksum);
  }
  usart_send_buf(UBLOX_UART, checksum, sizeof(checksum));
}



/**
 * @brief Keep receiving UBX messages until we get one with correct msg_class
 * and msg_id.
 */
static struct ubx_header_s *ublox_expect_response(uint8_t msg_class, uint8_t msg_id)
{
  int attempts;
  struct ubx_header_s *h = NULL;

  for (attempts = MAX_RETRIES; attempts > 0; attempts--) {
    h = ublox_wait_for_message();

    if (h->msg_class != msg_class || h->msg_id != msg_id) {
      continue;
    }

    break;
  }

  return h;
}

/**
 * @brief Send a ping to a ublox module, to ensure it's alive.
 *
 * @return 1 if successfully pinged, 0 if not.
 */
static int ublox_ping(void)
{
  struct ubx_header_s *header;
  uint8_t *data;
  int received = 0;

  ublox_poll(MSG_CLASS_MON, MSG_ID_MON_VER);

  header = ublox_wait_for_message();
  data = (uint8_t*) (header + 1);

  if (header->msg_class != MSG_CLASS_MON) {
    return 0;
  }

  if (header->msg_id != MSG_ID_MON_VER) {
    return 0;
  }

#define GET_ZT_STRING(bytes)                                   \
  {                                                            \
    volatile int i, j, appending;                              \
    for (appending = 1, i = 0; i < bytes; i++, received++) {   \
      j = *data++;                                             \
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
  while (received < header->length) {
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

  ublox_expect_response(MSG_CLASS_ACK, MSG_ID_ACK_ACK);

  return 1;
}

int ublox_get_rate(void)
{
  int status = 0;
  struct ubx_header_s *head;
  uint16_t *response;

  ublox_poll(MSG_CLASS_CFG, MSG_ID_CFG_RATE);

  head = ublox_expect_response(MSG_CLASS_CFG, MSG_ID_CFG_RATE);

  if (head == NULL) {
    return status;
  }

  if (!status) {
    return status;
  }

  assert(head->length == sizeof(response));

  response = (uint16_t*) (head + 1);

  dbg_print("Measuing rate is: %d ms, %d cycles, (time ref = %d)", 
      response[0], response[1], response[2]);

  return 1;
}

/**
 * @brief Receiving a navigation solution (blocking)
 */
int ublox_get(struct gps_sample_s *sample)
{
  struct ubx_header_s *head;
  struct ubx_nav_pvt_solution_s *body;

  assert(sample != NULL);

  head = ublox_expect_response(MSG_CLASS_NAV, MSG_ID_NAV_PVT);

  if (head == NULL) {
    return 0;
  }
  assert(sizeof(*body) == head->length);

  body = (struct ubx_nav_pvt_solution_s *) (head + 1);


  sample->lat = body->lat;
  sample->lon =  body->lon; 
  sample->ground_speed = body->gSpeed;
  sample->heading = body->heading;
  sample->accuracy = body->pDOP;

  dbg_print("(%i %i ft: %i): %d {%d, %d}",
      head->msg_class, head->msg_id, body->fixType,
      body->year,
      (int) body->lat/*1e7f*/, (int) body->lon/*1e7f*/);

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

  ublox_expect_response(MSG_CLASS_ACK, MSG_ID_ACK_ACK);

  return 1;
}

static struct ublox_cfg_pm2_s *ublox_get_power_mgmt(void)
{
  struct ubx_header_s *head;
  struct ublox_cfg_pm2_s *body;

  ublox_poll(MSG_CLASS_CFG, MSG_ID_CFG_PM2);

  head = ublox_expect_response(MSG_CLASS_CFG, MSG_ID_CFG_PM2);

  if (head == NULL) {
    return 0;
  }

  if (head->length != sizeof(*body)) {
    return 0;
  }

  body = (struct ublox_cfg_pm2_s *) (head + 1);

  return body;
}

static int ublox_update_port_settings(usart_t port, uint32_t baud)
{
  struct ubx_header_s *head;
  struct ublox_prt_cfg_s *config;
  uint8_t portConfig = 1;

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_PRT, &portConfig, sizeof(portConfig));

  head = ublox_expect_response(MSG_CLASS_CFG, MSG_ID_CFG_PRT);

  if (head == NULL) {
    return 0;
  }
  if (head->length != sizeof(config)) {
    return 0;
  }

  config = (struct ublox_prt_cfg_s *)(head + 1);

  /** TODO: Document getting the config via passing a 0 baud */
  if (baud == 0) {
    return 1;
  }

  config->outProtoMask = PROTO_UBX;
  config->baudRate = baud;

  ublox_send(MSG_CLASS_CFG, MSG_ID_CFG_PRT, (uint8_t*)config, sizeof(*config));

#if 0
  /** */
  while ((USART_SR(port) & USART_SR_TC) == 0)
    ;
#endif

  if (baud != 0) {
    arch_usart_set_baud(port, baud);
  }

  ublox_expect_response(MSG_CLASS_ACK, MSG_ID_ACK_ACK);

  return 1;
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
  ublox_expect_response(MSG_CLASS_ACK, MSG_ID_ACK_ACK);

  return 1;
}
