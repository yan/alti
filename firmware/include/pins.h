/**
 *
 *
 */

#ifndef __PINS_H
#define __PINS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

/** GPIOB */
#define BLUE_LED          GPIO6
#define GREEN_LED         GPIO7

#define BLUE_LED_TIMER    TIM4
#define BLUE_LED_CHANNEL  TIM_OC1
#define BLUE_LED_AF       GPIO_AF2

/** nRF8001 BLE and adesto flash memory */
#define NRF8001_PORT   SPI1
#define FLASH_PORT     SPI1
#define SPI1_GPIO      GPIOA
#define FLASH_nRESET   GPIO0
#define FLASH_nCS      GPIO1
#define NRF8001_GPIO   SPI1_GPIO
#define FLASH_GPIO     SPI1_GPIO
#define NRF8001_RST    GPIO2
#define NRF8001_RDYN   GPIO3
#define NRF8001_REQN   GPIO4
#define SPI1_SCK       GPIO5
#define SPI1_MISO      GPIO6
#define SPI1_MOSI      GPIO7
#define SPI1_PINS      (SPI1_SCK |  SPI1_MOSI | SPI1_MISO) // NRF8001_REQN

/** MS5611 barometric sensor and bmx055 9-axis */
#define MS5611_PORT   SPI2
#define BMX055_PORT   SPI2
#define SPI2_GPIO     GPIOB
#define MS5611_GPIO   SPI2_GPIO
#define BMX055_GPIO   SPI2_GPIO
#define MS5611_EN     GPIO12
#define SPI2_SCK      GPIO13
#define SPI2_MISO     GPIO14
#define SPI2_MOSI     GPIO15
#define SPI2_PINS    (SPI2_SCK | SPI2_MOSI | SPI2_MISO)

#define BMX055_EN_GPIO GPIOA
#define BMX055_EN_MAG  GPIO8
#define BMX055_EN_ACC  GPIO9
#define BMX055_EN_GYR  GPIO10
#define BMX055_EN_PINS (BMX055_EN_MAG | BMX055_EN_ACC | BMX055_EN_GYR)

/** Fre-fall alert LEDs */
#define ALERT_HIGH_SELECT
#define ALERT_HIGH_ENABLE


#endif // __PINS_H

