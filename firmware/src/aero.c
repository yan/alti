
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <events.h>
#include <util.h>
#include <config.h>
#include <globals.h>

#include <task_main.h>
#include <task_ble.h>
#include <task_alert.h>
#include <task_sensor.h>

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

static void config_tasks(void);
static void config_main_task(void);
static void config_alert_task(void);
static void config_ble_task(void);
static void config_sensor_task(void);

static void config_tasks(void)
{
  config_main_task();
  config_alert_task();
  config_ble_task();
  config_sensor_task();
}


static void config_ble_task(void)
{
  BaseType_t status;
  TaskHandle_t ble_handle;
  g.ble_data_g = pvPortMalloc(sizeof(struct ble_task_data_s));

  assert(g.ble_data_g != NULL);

  g.ble_data_g->in  = xQueueCreate(CONFIG_TASK_BLE_QUEUE_LEN, sizeof(void*));
  g.ble_data_g->semphr = xSemaphoreCreateBinary();

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

  status = xTaskCreate(task_sensor, "sensor", CONFIG_TASK_SENSOR_STACK_DEPTH,
      g.sensor_queue_g, CONFIG_TASK_SENSOR_PRIORITY, &sensor_handle);

  configASSERT(status == pdPASS);
}

int
main(void)
{

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
  initialise_monitor_handles();
#endif

  config_nvic();
  config_clock();
  config_globals();
  config_io();
  config_tasks();
  
  vTaskStartScheduler();

  /* NOTREACHED */
  for (;;) { }

  return 0;
}
