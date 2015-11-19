

#include <FreeRTOS.h>
#include <queue.h>

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
/**
 *
 */
void config_gps(void)
{
  ublox_init(CONFIG_UBLOX_BAUD_RATE);
  ublox_set_measuring_rate(200);
  ublox_start_updates(1);
  delay_ms(1000);
  ublox_sleep();
}

/**
 *
 */
void task_gps(void *p)
{
  (void) p;

  enum {
    SLEEP,
    RUN
  } state = RUN;

  BaseType_t received;
  BaseType_t sleep_period = MS_TO_TICKS(100);
  BaseType_t status;
  struct gps_sample_s sample;

  config_gps();

  for (;;) {
    status = xQueueReceive(g.gps_queue_g, &received, sleep_period);

    // TODO: Determine the wait characteristics of having xQueueReceive block and
    // ublox_get block, via usart recv
    if (status != pdPASS) {
      if (state == RUN) {

        ublox_get(&sample);

        // send to main queue ??
        //
      }
      continue; // ??
    }

    switch (received) {
      case EVT_GPS_START:
        ublox_start_updates(1);
        state = RUN;
        break;

      case EVT_GPS_SLEEP:
        ublox_sleep();
        state = SLEEP;
        break;

      case EVT_GPS_UPDATE_RTC:
        // do that
        break;

      default:
        assert(0);
        break;
    }
  }
}
#endif // CONFIG_USE_GPS
