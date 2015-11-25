
#ifndef __HAL_STM32_LIBOPENCM3
#define __HAL_STM32_LIBOPENCM3

#include <stdint.h>

typedef uint32_t pin_t;
typedef uint32_t gpio_t;
typedef uint32_t spi_t;
typedef uint32_t pwm_timer_t;
typedef uint32_t usart_t;
typedef uint32_t gpio_af_mode_t;

#if 0
#define mem_manage_handler mem_manage_handler
#define bus_fault_handler bus_fault_handler
#define hard_fault_handler hard_fault_handler
#define usage_fault_handler usage_fault_handler
#endif
#define usart_isr usart1_isr

#define usart_can_recv(port) ((USART_SR(port) & USART_SR_RXNE) != 0)

#endif // __HAL_STM32_LIBOPENCM
