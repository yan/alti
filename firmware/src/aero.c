
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>

#include <events.h>
#include <util.h>
#include <config.h>
#include <task_main.h>
#include <task_ble.h>
#include <task_status_led.h>

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif

static void config_main_task(void);
static void config_status_task(void);
static void config_ble_task(void);
void config_tasks(void);

void config_tasks(void)
{
  config_main_task();
  config_status_task();
  config_ble_task();
}

static void config_ble_task(void)
{
  BaseType_t status;
  TaskHandle_t ble_handle;
  ble_data_g = pvPortMalloc(sizeof(struct ble_task_data_s));

  assert(ble_data_g != NULL);

  ble_data_g->in  = xQueueCreate(CONFIG_TASK_BLE_QUEUE_LEN, sizeof(void*));
  ble_data_g->semphr = xSemaphoreCreateBinary();

  assert(ble_data_g->in != NULL);
  assert(ble_data_g->semphr != NULL);

  status = xTaskCreate(task_ble,
                       "task_ble",
                       CONFIG_TASK_BLE_STACK_DEPTH,
                       (void*) ble_data_g,
                       CONFIG_TASK_BLE_PRIORITY,
                       &ble_handle);

  assert(status == pdPASS);


}
static void config_status_task(void)
{
  BaseType_t status;
  TaskHandle_t status_handle;

  status_queue_g = xQueueCreate(CONFIG_TASK_STATUS_QUEUE_LEN,
                                sizeof(enum task_status_event_e));
  assert(status_queue_g != NULL);

  status = xTaskCreate(task_status_led,
                       "task_stat",
                       CONFIG_TASK_STATUS_STACK_DEPTH,
                       status_queue_g,
                       CONFIG_TASK_STATUS_PRIORITY,
                       &status_handle);
  assert(status == pdPASS);
}

static void config_main_task(void)
{
  BaseType_t status;
  TaskHandle_t main_handle;

  main_queue_g = xQueueCreate(CONFIG_TASK_MAIN_QUEUE_LEN,
                            sizeof(struct global_event_s));
  assert(main_queue_g != NULL);

  status = xTaskCreate(task_main, 
                       "task_main", 
                       CONFIG_TASK_MAIN_STACK_DEPTH,
                       main_queue_g,
                       CONFIG_TASK_MAIN_PRIORITY,
                       &main_handle);
  assert(status == pdPASS);
}

int
main(void)
{

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
  initialise_monitor_handles();
#endif

  config_clock();
  config_tasks();
  
  vTaskStartScheduler();

  /* NOTREACHED */
  for (;;) { }

  return 0;
}
