

#include <FreeRTOS.h>
#include <queue.h>

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
  BaseType_t received;
  struct global_event_s evt;
  BaseType_t sleep_period = portMAX_DELAY, status;

  config_gps();

  for (;;) {
    status = xQueueReceive(g.gps_queue_g, &received, sleep_period);

    if (status != pdPASS) {
      //
    }

    switch (received) {
      case GLOBAL_EVT_GPS_START:
        ublox_start_updates(1);
        break;
    }



    evt.type = GLOBAL_EVT_GPS_STARTED;
    evt.payload = (event_payload_t) 0;

    //dbg_print("Baro: got pressure: %x\n", (unsigned int)pressure);

    xQueueSend(g.main_queue_g, &evt, 0);
  }
}
