
#ifndef __RTOS_H
#define __RTOS_H

#if !TESTING
#  include <FreeRTOS.h>
#  include <task.h>
#  include <queue.h>
#  include <semphr.h>
#  include <queue.h>

#else // TESTING

#if __cplusplus
extern "C" {
#endif

#include <stdint.h>

// ¯\_(ツ)_/¯ 
#define pdTRUE   (1)
#define pdFALSE  (0)
#define pdPASS   (1)
#define pdFAIL   (0)

#define portMAX_DELAY ((uint32_t)-1)

#define xSemaphoreTake(a...) 1
#define xSemaphoreGive(a...) 1

//#define xQueueSend(a...) 1
int xQueueSend(void *queue, void *cmd, int delay);
int xQueueReceive(void *queue, void *dest, int delay);

typedef unsigned int BaseType_t;

#if __cplusplus
}
#endif // __cplusplus

#endif // TESTING

#endif // __RTOS_H
