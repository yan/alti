
#include <FreeRTOS.h>
#include <queue.h>

#include <events.h>
#include <globals.h>
#include <task_baro.h>
#include <util.h>

#include <ms5611.h>
#include <bmx055.h>
/**
 *
 */
void config_baro(void)
{
  // bmx055_init();
  ms5611_init();
}

/**
 *
 */
void task_baro(void *p)
{
  (void) p;
  uint32_t pressure;
  struct global_event_s evt;
  BaseType_t sleep_period = portMAX_DELAY, status;

  config_baro();

  dbg_print("Baro: started\n");

  for (;;) {
    status = xQueueReceive(g.baro_queue_g, &sleep_period, sleep_period);
    dbg_print("Baro: got status: %d\n", (int)status);

    if (status != pdPASS) {
      //
    }

    pressure = ms5611_get_mbarc(4);

    evt.type = GLOBAL_EVT_AIR_PRESSURE;
    evt.payload = (void*) pressure;

    dbg_print("Baro: got pressure: %x\n", (unsigned int)pressure);

    xQueueSend(g.main_queue_g, &evt, 0);
  }
}
