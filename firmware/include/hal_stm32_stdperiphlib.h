
#ifndef __HAL_STM32_STDPERIPHLIB
#define __HAL_STM32_STDPERIPHLIB

#include <stm32l1xx_conf.h>
#include <stdint.h>

typedef GPIO_TypeDef  *gpio_t;
typedef uint16_t       pin_t;
typedef SPI_TypeDef   *spi_t;
typedef TIM_TypeDef   *pwm_timer_t;
typedef USART_TypeDef *usart_t;
typedef uint8_t       gpio_af_mode_t;


#define usart_can_recv(port) ((port->SR & USART_SR_RXNE) != RESET)
// #define usart_can_recv(port) (USART_GetITStatus(port, USART_IT_RXNE) == SET)

#endif // __HAL_STM32_LIBOPENCM
