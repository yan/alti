/**
 * Copyright 2015 Yan Ivnitskiy
 */
#include <aero.h>

#include <string.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <hal.h>
#include <events.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <flash.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_alert.h>
#include <task_sensor.h>
#include <task_gps.h>

#if CONFIG_USE_GPS
extern void initialise_monitor_handles(void);
#endif

static void config_tasks(void);
static void config_main_task(void);
static void config_alert_task(void);
static void config_ble_task(void);
static void config_sensor_task(void);
#if CONFIG_USE_GPS
static void config_gps_task(void);
#endif

static void config_tasks(void)
{
  config_main_task();
  config_alert_task();
  config_ble_task();
  config_sensor_task();
#if CONFIG_USE_GPS
  config_gps_task();
#endif
}


static void config_ble_task(void)
{
  BaseType_t status;
  TaskHandle_t ble_handle;
  g.ble_data_g = pvPortMalloc(sizeof(struct ble_task_data_s));

  assert(g.ble_data_g != NULL);

  g.ble_data_g->in  = xQueueCreate(CONFIG_TASK_BLE_QUEUE_LEN, sizeof(void*));
  g.ble_data_g->semphr = xSemaphoreCreateBinary();

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.ble_data_g->in, "ble");
#endif

  assert(g.ble_data_g->in != NULL);
  assert(g.ble_data_g->semphr != NULL);

  status = xTaskCreate(task_ble, "ble", CONFIG_TASK_BLE_STACK_DEPTH,
                       (void*) g.ble_data_g, CONFIG_TASK_BLE_PRIORITY, &ble_handle);

  assert(status == pdPASS);


}
static void config_alert_task(void)
{
  BaseType_t status;
  TaskHandle_t alert_handle;

  g.alert_queue_g = xQueueCreate(CONFIG_TASK_ALERT_QUEUE_LEN,
                                sizeof(enum task_alert_event_e));
  assert(g.alert_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.alert_queue_g, "alert");
#endif
  status = xTaskCreate(task_alert_led, "alert", CONFIG_TASK_ALERT_STACK_DEPTH,
                       g.alert_queue_g, CONFIG_TASK_ALERT_PRIORITY, &alert_handle);
  assert(status == pdPASS);
}

static void config_main_task(void)
{
  BaseType_t status;
  TaskHandle_t main_handle;

  g.main_queue_g = xQueueCreate(CONFIG_TASK_MAIN_QUEUE_LEN,
                            sizeof(struct global_event_s));
  assert(g.main_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.main_queue_g, "main");
#endif
  status = xTaskCreate(task_main, "main", CONFIG_TASK_MAIN_STACK_DEPTH,
                       g.main_queue_g, CONFIG_TASK_MAIN_PRIORITY, &main_handle);
  assert(status == pdPASS);
}

static void config_sensor_task(void)
{
  BaseType_t status;
  TaskHandle_t sensor_handle;

  g.sensor_queue_g = xQueueCreate(CONFIG_TASK_SENSOR_QUEUE_LEN,
      sizeof(BaseType_t));

  configASSERT(g.sensor_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.sensor_queue_g, "sensor");
#endif

  status = xTaskCreate(task_sensor, "sensor", CONFIG_TASK_SENSOR_STACK_DEPTH,
      g.sensor_queue_g, CONFIG_TASK_SENSOR_PRIORITY, &sensor_handle);

  configASSERT(status == pdPASS);
}

#if CONFIG_USE_GPS
static void config_gps_task(void)
{
  BaseType_t status;
  TaskHandle_t gps_handle;

  g.gps_queue_g = xQueueCreate(CONFIG_TASK_GPS_QUEUE_LEN,
      sizeof(BaseType_t));

  configASSERT(g.gps_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.gps_queue_g, "gps");
#endif

  status = xTaskCreate(task_gps, "gps", CONFIG_TASK_GPS_STACK_DEPTH,
      g.gps_queue_g, CONFIG_TASK_GPS_PRIORITY, &gps_handle);

  configASSERT(status == pdPASS);
}
#endif

int
aero_main(void)
{

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
  initialise_monitor_handles();
#endif


  arch_config_nvic();
  arch_config_clocks();
  arch_config_io();

  config_globals();
  config_tasks();
  config_flash();
  config_load_persistent();

  vTaskStartScheduler();

  /* NOTREACHED */
  for (;;) { }

  return 0;
}
