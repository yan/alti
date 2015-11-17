

#include <FreeRTOS.h>
#include <queue.h>

#include <config.h>

#if CONFIG_USE_GPS

#include <events.h>
#include <globals.h>
#include <task_gps.h>
#include <util.h>
#include <logger.h>

#include <ublox.h>
/**
 *
 */
void config_gps(void)
{
  ublox_init();
  // ublox_start_updates(1);
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
  } state = SLEEP;

  BaseType_t received;
  BaseType_t sleep_period = portMAX_DELAY;
  BaseType_t status;

  config_gps();

  for (;;) {
    status = xQueueReceive(g.gps_queue_g, &received, sleep_period);

    if (status != pdPASS) {
      if (state == RUN) {
        ublox_get();
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
