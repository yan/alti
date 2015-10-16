/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <FreeRTOS.h>
#include <queue.h>

#include <events.h>
#include <globals.h>
#include <task_sensor.h>
#include <util.h>
#include <logger.h>

#include <ms5611.h>
#include <bmx055.h>
#include <ublox.h>
/**
 *
 */
void config_sensor(void)
{
  bmx055_init();
  ms5611_init();
  ublox_init();
}

/**
 *
 */
void task_sensor(void *p)
{
  (void) p;
  uint32_t pressure;
  struct global_event_s evt;
  BaseType_t sleep_period = portMAX_DELAY, status;

  config_sensor();

  for (;;) {
    status = xQueueReceive(g.sensor_queue_g, &sleep_period, sleep_period);

    if (status != pdPASS) {
      //
    }

    pressure = ms5611_get_mbarc(4);

    evt.type = GLOBAL_EVT_AIR_PRESSURE;
    evt.payload = (event_payload_t) (intptr_t) pressure;

    //dbg_print("Baro: got pressure: %x\n", (unsigned int)pressure);

    xQueueSend(g.main_queue_g, &evt, 0);
  }
}
