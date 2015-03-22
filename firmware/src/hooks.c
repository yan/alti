/**
 * These hooks get called by FreeRTOS if it detects a stack overflow or if we
 * run out of heap space.
 */

void vApplicationStackOverflowHook(void);
void vApplicationMallocFailedHook(void);


/**
 * XXX: Determine what the best course of action here is.
 */ 
void vApplicationStackOverflowHook(void)
{
  for (;;);
}

void vApplicationMallocFailedHook(void)
{
  for (;;);
}
