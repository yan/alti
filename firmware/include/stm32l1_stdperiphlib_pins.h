/**
 * Copyright 2015 Yan Ivnitskiy
 */

#ifndef __STM32L1_PINS_H
#define __STM32L1_PINS_H

#include "stm32l1xx.h"


#define PIN(x)              GPIO_Pin_ ## x
#define PIN_SRC(x)          GPIO_PinSource ## x
#define GPIO(x)             GPIO ## x

#if 0
/** Status LED */
#define STATUS_LED          GPIO_Pin_11
#define STATUS_GPIO         GPIOB

/** Piezo enable and out pinz */
#define PIEZO_GPIO          GPIOB
#define PIEZO_EN            GPIO_Pin_6
#define PIEZO_OUT           GPIO_Pin_7

#define PIEZO_OUT_TIMER     TIM4
#define PIEZO_OUT_CHANNEL   TIM_OC2
#define PIEZO_OUT_AF        GPIO_AF2

#define STATUS_LED_TIMER    TIM2
#define STATUS_LED_CHANNEL  TIM_OC4
#define STATUS_LED_AF       GPIO_AF1

#define LSE_TIMER           TIM9
#define LSE_TIMER_CHANNEL   TIM_OC1

/** nRF8001 BLE and adesto flash memory */
#define NRF8001_PORT        SPI1
#define FLASH_PORT          SPI1
#define SPI1_GPIO           GPIOA
#define NRF8001_GPIO        SPI1_GPIO
#define FLASH_GPIO          SPI1_GPIO
#define FLASH_nRESET        GPIO_Pin_0
#define FLASH_nCS           GPIO_Pin_1
#define NRF8001_RST         GPIO_Pin_2
#define NRF8001_RDYN        GPIO_Pin_3
#define NRF8001_REQN        GPIO_Pin_4
#define SPI1_SCK            GPIO_Pin_5
#define SPI1_MISO           GPIO_Pin_6
#define SPI1_MOSI           GPIO_Pin_7
#define SPI1_SCK_SRC        GPIO_PinSource5
#define SPI1_MISO_SRC       GPIO_PinSource6
#define SPI1_MOSI_SRC       GPIO_PinSource7
// #define SPI1_PINS           (SPI1_SCK |  SPI1_MOSI | SPI1_MISO) // NRF8001_REQN

/** MS5611 barometric sensor and bmx055 9-axis */
#define BMX055_EN_GPIO      GPIOA
#define BMX055_EN_MAG       GPIO_Pin_8
#define BMX055_EN_ACC       GPIO_Pin_9
#define BMX055_EN_GYR       GPIO_Pin_10
#define BMX055_EN_PINS      (BMX055_EN_MAG | BMX055_EN_ACC | BMX055_EN_GYR)

#define MS5611_PORT         SPI2
#define BMX055_PORT         SPI2
#define SPI2_GPIO           GPIOB
#define MS5611_GPIO         SPI2_GPIO
#define BMX055_GPIO         SPI2_GPIO
#define MS5611_EN           GPIO_Pin_12
#define SPI2_SCK            GPIO_Pin_13
#define SPI2_MISO           GPIO_Pin_14
#define SPI2_MOSI           GPIO_Pin_15
#define SPI2_SCK_SRC        GPIO_PinSource13
#define SPI2_MISO_SRC       GPIO_PinSource14
#define SPI2_MOSI_SRC       GPIO_PinSource15
#define SPI2_PINS          (SPI2_SCK | SPI2_MOSI | SPI2_MISO)

#define BMX055_INT_GPIO     GPIOB
#define BMX055_INT_ACC      GPIO_Pin_4

/** Fre-fall alert LEDs */
#define ALERT_HIGH_SELECT
#define ALERT_HIGH_ENABLE

#endif


#endif // __STM32L1_PINS_H

