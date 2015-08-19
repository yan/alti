/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __PINS_H
#define __PINS_H

#if defined(STM32L1) && defined(OPENCM3)
#  include <stm32l1_opencm3_pins.h>
#elif defined(STM32L1) &&  defined(STM32_STDPERIPH_LIB)
#  include <stm32l1_stdperiphlib_pins.h>
#else
#  error "Unsupprted architecture"
#endif

#endif //  __PINS_H

