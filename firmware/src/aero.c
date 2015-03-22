
#include <libopencm3/stm32/rcc.h>

#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#include <events.h>
#include <util.h>
#include <config.h>
#include <task_main.h>

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif
void config_tasks(void);

void config_tasks(void)
{
  BaseType_t status;
  QueueHandle_t main_queue;
  TaskHandle_t main_handle;

  main_queue = xQueueCreate(CONFIG_TASK_MAIN_QUEUE_LEN, sizeof(struct global_event_s));
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
