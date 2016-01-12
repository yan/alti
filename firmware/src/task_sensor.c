/**
 * Copyright 2015 Yan Ivnitskiy
 */


#include <rtos.h>
#include <events.h>
#include <globals.h>
#include <task_sensor.h>
#include <util.h>
#include <logger.h>
#include <features.h>

#include <ms5611.h>
#include <bmx055.h>

/**
 * All the sensors in this task share the same SPI bus, otherwise we'd need 
 * extra locking.
 */
void config_sensor(void)
{
#if CONFIG_USE_ACCEL || CONFIG_USE_GYRO || CONFIG_USE_MAG
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
      sleep_period = MS_TO_TICKS(50);
    }

    if (received_event & SENSOR_REQUEST_AIR_PRESSURE) {
      result = ms5611_get_mbarc(4);

      evt.type = GLOBAL_EVT_SENSOR_BARO;
      evt.payload.baro_sample.mbarc = result;

      xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
    }

#if CONFIG_USE_ACCEL
    if (received_event & SENSOR_REQUEST_ACCEL) {
      // save result
      evt.type = GLOBAL_EVT_SENSOR_ACCEL;
      evt.payload.accel_sample.accel[0] = bmx055_read(BMX055_ACCEL, DIR_X);
      evt.payload.accel_sample.accel[1] = bmx055_read(BMX055_ACCEL, DIR_Y);
      evt.payload.accel_sample.accel[2] = bmx055_read(BMX055_ACCEL, DIR_Z);

      xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
    }
#endif // CONFIG_USE_ACCEL

#if CONFIG_USE_GYRO
    if (received_event & SENSOR_REQUEST_GYRO) {
      // save result
      evt.type = GLOBAL_EVT_SENSOR_GYRO;
      evt.payload.gyro_sample.gyro[0] = bmx055_read(BMX055_GYRO, DIR_X);
      evt.payload.gyro_sample.gyro[1] = bmx055_read(BMX055_GYRO, DIR_Y);
      evt.payload.gyro_sample.gyro[2] = bmx055_read(BMX055_GYRO, DIR_Z);

      xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
    }
#endif // CONFIG_USE_GYRO

#if CONFIG_USE_MAG
    if (received_event & SENSOR_REQUEST_MAG) {
      // save result
      evt.type = GLOBAL_EVT_SENSOR_ACCEL;
      evt.payload.mag_sample.mag[0] = bmx055_read(BMX055_MAG, DIR_X);
      evt.payload.mag_sample.mag[1] = bmx055_read(BMX055_MAG, DIR_Y);
      evt.payload.mag_sample.mag[2] = bmx055_read(BMX055_MAG, DIR_Z);

      xQueueSend(g.main_queue_g, &evt, portMAX_DELAY);
    }
#endif
    // Go back to sleeping
    if (received_event & SENSOR_REQUEST_STOP) {
      sleep_period = portMAX_DELAY;
    }

    // respond with a result
  }
}
