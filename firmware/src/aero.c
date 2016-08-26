/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <aero.h>

#include <string.h>

#include <rtos.h>
#include <hal.h>
#include <events.h>
#include <util.h>
#include <config.h>
#include <globals.h>
#include <flash.h>
#include <periph.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_alert.h>
#include <task_sensor.h>
#include <task_gps.h>


#if ENABLE_SEMIHOSTING
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

#if configSUPPORT_STATIC_ALLOCATION
/* static memory allocation for the IDLE task */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];
 
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize) {
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];
 
/* If static allocation is supported then the application must provide the
   following callback function - which enables the application to optionally
   provide the memory that will be used by the timer task as the task's stack
   and TCB. */
void vApplicationGetTimerTaskMemory(StaticTask_t  **ppxTimerTaskTCBBuffer,
                                      StackType_t **ppxTimerTaskStackBuffer,
                                      uint32_t     *pulTimerTaskStackSize)
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif

static void config_ble_task(void)
{
  BaseType_t status;
  TaskHandle_t ble_handle;

  g.ble_data_g.in  = xQueueCreate(CONFIG_TASK_BLE_QUEUE_LEN,
      sizeof(struct nrf8001_cmd_s));
  g.ble_data_g.semphr = xSemaphoreCreateBinary();
  g.ble_data_g.credits = xSemaphoreCreateCounting(4, 0);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.ble_data_g.in, "ble");
  vQueueAddToRegistry(g.ble_data_g.semphr, "ble_mutex");
  vQueueAddToRegistry(g.ble_data_g.credits, "credits");
#endif

  assert(g.ble_data_g.in != NULL);
  assert(g.ble_data_g.semphr != NULL);

  status = xTaskCreate(task_ble, "ble", CONFIG_TASK_BLE_STACK_DEPTH,
                   (void*) &g.ble_data_g, CONFIG_TASK_BLE_PRIORITY, &ble_handle);

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

  assert(g.sensor_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.sensor_queue_g, "sensor");
#endif

  status = xTaskCreate(task_sensor, "sensor", CONFIG_TASK_SENSOR_STACK_DEPTH,
      g.sensor_queue_g, CONFIG_TASK_SENSOR_PRIORITY, &sensor_handle);

  assert(status == pdPASS);
}

#if CONFIG_USE_GPS

#if configSUPPORT_STATIC_ALLOCATION
uint8_t kGpsQueueStorage[CONFIG_TASK_GPS_QUEUE_LEN * sizeof(BaseType_t)];
StaticQueue_t kGpsQueue;
#endif
static void config_gps_task(void)
{
  BaseType_t status;
  TaskHandle_t gps_handle;

#if configSUPPORT_STATIC_ALLOCATION
  g.gps_queue_g = xQueueCreateStatic(CONFIG_TASK_GPS_QUEUE_LEN,
      sizeof(BaseType_t), kGpsQueueStorage, &kGpsQueue);
#else
  g.gps_queue_g = xQueueCreate(CONFIG_TASK_BLE_QUEUE_LEN, sizeof(BaseType_t));
#endif

  // g.gps_queue_g = xQueueCreate(1, sizeof(BaseType_t));
  // g.gps_isr_semphr_g = xSemaphoreCreateBinary();

  assert(g.gps_queue_g != NULL);
  // assert(g.gps_isr_queue_g != NULL);

#if ( configQUEUE_REGISTRY_SIZE > 0 )
  vQueueAddToRegistry(g.gps_queue_g, "gps");
#endif

  status = xTaskCreate(task_gps, "gps", CONFIG_TASK_GPS_STACK_DEPTH,
      g.gps_queue_g, CONFIG_TASK_GPS_PRIORITY, &gps_handle);

  assert(status == pdPASS);
}
#endif // CONFIG_USE_GPS

int
aero_main(int argc, char *argv[])
{

  (void) argc;
  (void) argv;

#if ENABLE_SEMIHOSTING
  initialise_monitor_handles();
#endif

  arch_config_clocks();
  arch_config_nvic();
  arch_config_io();

  config_globals();
  config_flash();
  config_tasks();
  config_load_persistent();

  vTaskStartScheduler();

  /* NOTREACHED */
  for (;;) { }

  return 0;
}
