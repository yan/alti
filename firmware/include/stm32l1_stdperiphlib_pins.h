/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __STM32L1_PINS_H
#define __STM32L1_PINS_H

#include "stm32l1xx.h"


#define PIN(x)              GPIO_Pin_ ## x
#define PIN_SRC(x)          GPIO_PinSource ## x
#define GPIO(x)             GPIO ## x
#define GPIO_AF(x)          ((uint8_t)x)
#define TIM_OC(x)           (x)


#endif // __STM32L1_PINS_H

