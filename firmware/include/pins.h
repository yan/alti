/**
 *
 *
 */

#ifndef __PINS_H
#define __PINS_H

#include <libopencm3/stm32/gpio.h>

/** GPIOB */
#define BLUE_LED        GPIO6
#define GREEN_LED       GPIO7

#define MS5611_SCL_PIN  GPIO8
#define MS5611_SDA_PIN  GPIO9

#define NRF8001_RST     GPIO10
#define NRF8001_RDYN    GPIO11
#define NRF8001_REQN    GPIO12
#define NRF8001_SCK     GPIO13
#define NRF8001_MISO    GPIO14
#define NRF8001_MOSI    GPIO15

#define MS5611_PINS     (MS5611_SDA_PIN | MS5611_SCL_PIN)
#define NRF8001_PINS    (NRF8001_SCK | NRF8001_REQN | NRF8001_MOSI | NRF8001_MISO)


#endif // __PINS_H

