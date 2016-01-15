
#ifndef __RTOS_H
#define __RTOS_H

#if !TESTING
#  include <FreeRTOS.h>
#  include <task.h>
#  include <queue.h>
#  include <semphr.h>
#  include <queue.h>
#else
// ¯\_(ツ)_/¯ 
#define pdTRUE   (1)
#define pdFALSE  (0)
#define pdPASS   (1)
#define pdFAIL   (0)
#endif

#endif // __RTOS_H
