
#include <FreeRTOS.h>
#include <task.h>

#include <util.h>


void delay_ms(uint32_t ms)
{
  vTaskDelay(ms / portTICK_PERIOD_MS);
}
