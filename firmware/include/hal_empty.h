
#ifndef __HAL_EMPTY
#define __HAL_EMPTY

#include <stdint.h>

typedef uint32_t pin_t;
typedef uint32_t gpio_t;
typedef uint32_t spi_t;
typedef uint32_t pwm_timer_t;
typedef uint32_t usart_t;

#define mem_manage_handler mem_manage_handler
#define bus_fault_handler bus_fault_handler
#define hard_fault_handler hard_fault_handler
#define usage_fault_handler usage_fault_handler

#endif // __HAL_EMPTY
