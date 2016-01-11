
#ifndef __HAL_STM32_LIBOPENCM3
#define __HAL_STM32_LIBOPENCM3

#include <stdint.h>

typedef uint32_t pin_t;
typedef uint32_t gpio_t;
typedef uint32_t spi_t;
typedef uint32_t pwm_timer_t;
typedef uint32_t usart_t;
typedef uint32_t gpio_af_mode_t;

#define usart_can_recv(port) ((USART_SR(port) & USART_SR_RXNE) != 0)

#endif // __HAL_STM32_LIBOPENCM
