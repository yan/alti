/**
 * Copyright 2015 Yan Ivnitskiy
 */

#include <rtos.h>

/**
 * These hooks get called by FreeRTOS if it detects a stack overflow or if we
 * run out of heap space.
 */

extern void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName);
extern void vApplicationMallocFailedHook(void);


/**
 * XXX: Determine what the best course of action here is.
 */ 
void vApplicationStackOverflowHook(TaskHandle_t *pxTask, signed char *pcTaskName)
{
  (void) pxTask;
  (void) pcTaskName;

  for (;;);
}

void vApplicationMallocFailedHook(void)
{
  for (;;);
}
