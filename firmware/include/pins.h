/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __PINS_H
#define __PINS_H

#if defined(STM32L1)
#  include <stm32l1_pins.h>
#else
#  error "Unsupprted architecture"
#endif

#endif //  __PINS_H

