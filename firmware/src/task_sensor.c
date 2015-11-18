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

/**
 * All the sensors in this task share the same SPI bus, otherwise we'd need 
 * extra locking.
 */
void config_sensor(void)
{
#if CONFIG_USE_ACCEL
  bmx055_init();
#endif
  ms5611_init();
}

/**
 *
 */
void task_sensor(void *p)
{
  (void) p;
  uint32_t result;
  sensor_event_t received_event;
  struct global_event_s evt;
  BaseType_t sleep_period = portMAX_DELAY, status;

  config_sensor();

  for (;;) {
    status = xQueueReceive(g.sensor_queue_g, &received_event, sleep_period);

    if (status != pdPASS) {
      //
    } else {
      // Start spamming
      sleep_period = MS_TO_TICKS(200);
    }

    if (received_event & SENSOR_REQUEST_AIR_PRESSURE) {
      result = ms5611_get_mbarc(4);

      evt.type = GLOBAL_EVT_AIR_PRESSURE;
      evt.payload = (event_payload_t) (intptr_t) result;

      xQueueSend(g.main_queue_g, &evt, 0);
    }

#if CONFIG_USE_ACCEL
    if (received_event & SENSOR_REQUEST_ACCEL) {
      // save result
      result = bmx055_read(BMX055_ACCEL, DIR_X);
      result = bmx055_read(BMX055_ACCEL, DIR_Y);
      result = bmx055_read(BMX055_ACCEL, DIR_Z);

      evt.type = GLOBAL_EVT_AIR_PRESSURE;
      evt.payload = (event_payload_t) (intptr_t) result;

      xQueueSend(g.main_queue_g, &evt, 0);
    }
#endif // CONFIG_USE_ACCEL

    // Go back to sleeping
    if (received_event & SENSOR_REQUEST_STOP) {
      sleep_period = portMAX_DELAY;
    }

    // respond with a result
  }
}
