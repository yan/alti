
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <events.h>
#include <util.h>
#include <config.h>
#include <task_main.h>
#include <task_status_led.h>

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif
static void config_main_task(void);
static void config_status_task(void);
void config_tasks(void);

void config_tasks(void)
{
  config_main_task();
  config_status_task();
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
                       (void*) status_queue_g,
                       CONFIG_TASK_STATUS_PRIORITY,
                       &status_handle);
  assert(status == pdPASS);
}

static void config_main_task(void)
{
  BaseType_t status;
  QueueHandle_t main_queue;
  TaskHandle_t main_handle;

  main_queue = xQueueCreate(CONFIG_TASK_MAIN_QUEUE_LEN,
                            sizeof(struct global_event_s));
  assert(main_queue != NULL);

  status = xTaskCreate(task_main, 
                       "task_main", 
                       CONFIG_TASK_MAIN_STACK_DEPTH,
                       (void*) main_queue,
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
