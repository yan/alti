/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __PINS_H
#define __PINS_H

#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>

/** Status LED */
#define STATUS_LED          GPIO11
#define STATUS_GPIO         GPIOB

/** Piezo enable and out pinz */
#define PIEZO_GPIO          GPIOB
#define PIEZO_EN            GPIO6
#define PIEZO_OUT           GPIO7

#define PIEZO_OUT_TIMER     TIM4
#define PIEZO_OUT_CHANNEL   TIM_OC2
#define PIEZO_OUT_AF        GPIO_AF2

#define STATUS_LED_TIMER    TIM2
#define STATUS_LED_CHANNEL  TIM_OC4
#define STATUS_LED_AF       GPIO_AF1

#define LSE_TIMER           TIM9
#define LSE_TIMER_CHANNEL   TIM_OC1

/** nRF8001 BLE and adesto flash memory */
#define NRF8001_PORT   SPI1
#define FLASH_PORT     SPI1
#define SPI1_GPIO      GPIOA
#define NRF8001_GPIO   SPI1_GPIO
#define FLASH_GPIO     SPI1_GPIO
#define FLASH_nRESET   GPIO0
#define FLASH_nCS      GPIO1
#define NRF8001_RST    GPIO2
#define NRF8001_RDYN   GPIO3
#define NRF8001_REQN   GPIO4
#define SPI1_SCK       GPIO5
#define SPI1_MISO      GPIO6
#define SPI1_MOSI      GPIO7
#define SPI1_PINS      (SPI1_SCK |  SPI1_MOSI | SPI1_MISO) // NRF8001_REQN

/** MS5611 barometric sensor and bmx055 9-axis */
#define BMX055_EN_GPIO GPIOA
#define BMX055_EN_MAG  GPIO8
#define BMX055_EN_ACC  GPIO9
#define BMX055_EN_GYR  GPIO10
#define BMX055_EN_PINS (BMX055_EN_MAG | BMX055_EN_ACC | BMX055_EN_GYR)

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

#define BMX055_INT_GPIO   GPIOB
#define BMX055_INT_ACC    GPIO4

/** Fre-fall alert LEDs */
#define ALERT_HIGH_SELECT
#define ALERT_HIGH_ENABLE


#endif // __PINS_H

