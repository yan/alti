
#include <libopencm3/stm32/rcc.h>
#include <FreeRTOS.h>
#include <task.h>

#include <config.h>

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
extern void initialise_monitor_handles(void);
#endif


int
main(void)
{

#if defined(ENABLE_SEMIHOSTING) && ENABLE_SEMIHOSTING
  initialise_monitor_handles();
#endif

  config_clock();
  
  vTaskStartScheduler();

  /* NOTREACHED */
  for (;;) { }

  return 0;
}
