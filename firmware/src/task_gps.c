

#include <rtos.h>
#include <config.h>

#if CONFIG_USE_GPS

#include <semphr.h>

#include <events.h>
#include <globals.h>
#include <task_gps.h>
#include <util.h>
#include <logger.h>
#include <hal.h>
#include <pins.h>

#include <ublox.h>
BaseType_t usart_given = 0;

struct ubx_state_s {
  usart_t port;
  struct {
    uint32_t waiting_for_ack : 1;
  } flags;
  uint8_t class_and_id[2];
};


static void handle_ubx_message(struct ubx_state_s *state);

static void handle_ubx_message(struct ubx_state_s *state)
{
  struct ubx_header_s *head;
  uint8_t *content;

  assert(state != NULL);

  /* If we're here, the message is already in the receiving buffer */
  head = ublox_get_incoming_message();
  content = (uint8_t*) (head + 1);


#define IS(class, id) ((head->msg_class == (class) && head->msg_id == (id)))

  /**
   * We received a standard acknowledgement to a previous message
   */
  if (IS(MSG_CLASS_ACK, MSG_ID_ACK_ACK)) {
    if (!state->flags.waiting_for_ack) {
      return;
    }

    if (content[0] != state->class_and_id[0] ||
        content[1] != state->class_and_id[1]) {
      /*
       * Here is where we'd note that we received an out of order
       * acknowledgement.
       */
    } else {
      state->flags.waiting_for_ack = 0;
    }
  }

  /**
   * We received a negative acknowledgement. TODO: Retransmit last message or  
   * mark an error condition.
   */
  if (IS(MSG_CLASS_ACK, MSG_ID_ACK_NAK)) {

  }

  /**
   * Response to a request asking what the measuring rate is
   */
  if (IS(MSG_CLASS_CFG, MSG_ID_CFG_RATE)) {
    uint16_t *response = (uint16_t*) content;
    
    dbg_print("Measuing rate is: %d ms, %d cycles, (time ref = %d)", 
        response[0], response[1], response[2]);
    (void) response;
  }

  /**
   * A PVT nav solution
   */
  if (IS(MSG_CLASS_NAV, MSG_ID_NAV_PVT)) {
    struct ubx_nav_pvt_solution_s *body = (struct ubx_nav_pvt_solution_s*)content;;
    struct global_event_s evt;
    // struct gps_sample_s *sample;
    
    assert(sizeof(*body) == head->length);

    evt.type = GLOBAL_EVT_SENSOR_GPS;

    evt.payload.gps_sample.lat = body->lat;
    evt.payload.gps_sample.lon = body->lon;
    evt.payload.gps_sample.ground_speed = body->gSpeed;
    evt.payload.gps_sample.heading = body->heading;
    evt.payload.gps_sample.accuracy = body->pDOP;

    xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
  }
}
/**
 *
 */
void config_gps(void)
{
  ublox_init(CONFIG_UBLOX_BAUD_RATE);
  // ublox_set_measuring_rate(200);
  // ublox_start_updates(1);
  // delay_ms(1000);
  // ublox_sleep();
}

/**
 *
 */
void task_gps(void *p)
{
  (void) p;

  enum { STARTING, SLEEP, RUN } state = STARTING;

  enum gps_event_t event;
  BaseType_t status;
  struct ubx_state_s ubx_state = {
    .port = UBLOX_UART
  };

  config_gps();
  // ublox_reset();
  // ublox_set_measuring_rate(200);

  for (;;) {
    status = xQueueReceive(g.gps_queue_g, &event, portMAX_DELAY);

    if (status != pdPASS) {
      continue;
    } 

    switch (event) {
      case EVT_GPS_RESET:
        ublox_reset();
        break;

      case EVT_GPS_START:
        // ubx_state.flags.waiting_for_ack = !!ublox_set_measuring_rate(200);
#if 0
        status = xQueueReceive(g.gps_queue_g, &event, MS_TO_TICKS(1000));
        /* XXX Send a 'failed to start message to main queue here? */
        if (status != pdPASS) {
          event = EVT_GPS_RESET;
          xQueueSend(g.gps_queue_g, &event, portMAX_DELAY);
          continue;
        } 
#endif
        if (state == RUN) {
          break;
        }

        ublox_set_measuring_rate(200);
        ublox_start_updates(1);
        delay_ms(1000);
        // ublox_wake();

        state = RUN;

        break;

      case EVT_GPS_SLEEP:
        if (state != RUN) {
          break;
        }

        ublox_sleep();
        state = SLEEP;
        break;

      case EVT_GPS_CFG:
        // ublox_update_port_settings
        break;

      case EVT_GPS_UPDATE_RTC:
        // do that
        break;

      case EVT_GPS_UBX_WAITING:
        handle_ubx_message(&ubx_state);
        break;

      default:
        assert(0);
        break;
    }
  }
  (void) state;
}
#endif // CONFIG_USE_GPS
