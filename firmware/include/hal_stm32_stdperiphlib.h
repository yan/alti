
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

#define mem_manage_handler mem_manage_handler
#define bus_fault_handler bus_fault_handler
#define hard_fault_handler hard_fault_handler
#define usage_fault_handler usage_fault_handler

#endif // __HAL_STM32_LIBOPENCM
