/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __EMPTY_PINS_H
#define __EMPTY_PINS_H

#define GPIO_START          0
#define GPIO0               (GPIO_START + 0)
#define GPIO1               (GPIO_START + 1)
#define GPIO2               (GPIO_START + 2)
#define GPIO3               (GPIO_START + 3)
#define GPIO4               (GPIO_START + 4)
#define GPIO5               (GPIO_START + 5)
#define GPIO6               (GPIO_START + 6)
#define GPIO7               (GPIO_START + 7)
#define GPIO8               (GPIO_START + 8)
#define GPIO9               (GPIO_START + 9)
#define GPIO10              (GPIO_START + 10)
#define GPIO11              (GPIO_START + 11)
#define GPIO12              (GPIO_START + 12)
#define GPIO13              (GPIO_START + 13)

#define TIM_START           20
#define TIM0                (TIM_START + 0)
#define TIM1                (TIM_START + 1)
#define TIM2                (TIM_START + 2)
#define TIM3                (TIM_START + 3)

#define TIM_OC_START        30
#define TIM_OC0             (TIM_OC_START + 0)
#define TIM_OC1             (TIM_OC_START + 1)
#define TIM_OC2             (TIM_OC_START + 2)
#define TIM_OC3             (TIM_OC_START + 3)
#define TIM_OC4             (TIM_OC_START + 4)

#define SPI_START           40
#define SPI0                (SPI_START + 0)
#define SPI1                (SPI_START + 1)
#define SPI2                (SPI_START + 2)

#define GPIO_PORT_START     50
#define GPIOA               (GPIO_PORT_START + 0)
#define GPIOB               (GPIO_PORT_START + 1)
#define GPIOC               (GPIO_PORT_START + 2)
#define GPIOD               (GPIO_PORT_START + 3)
#define GPIOE               (GPIO_PORT_START + 4)
#define GPIOG               (GPIO_PORT_START + 5)
#define GPIOH               (GPIO_PORT_START + 6)

#define USART_START         60
#define USART0              (USART_START + 0)
#define USART1              (USART_START + 1)

#define PIN(x)              GPIO ## x
#define PIN_SRC(x)
#define GPIO(x)             GPIO ## x

#endif // __EMPTY_PINS_H

