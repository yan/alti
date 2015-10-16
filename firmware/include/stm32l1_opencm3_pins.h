/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __STM32L1_PINS_H
#define __STM32L1_PINS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/usart.h>

#define PIN(x)              GPIO ## x
#define PIN_SRC(x)
#define GPIO(x)             GPIO ## x

#endif // __STM32L1_PINS_H

