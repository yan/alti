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

/** Status LED */
#define STATUS_LED          PIN(11)
#define STATUS_GPIO         GPIO(B)

/** Piezo enable and out pinz */
#define PIEZO_GPIO          GPIO(B)
#define PIEZO_EN            PIN(6)
#define PIEZO_OUT           PIN(7)

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
#define SPI1_GPIO           GPIO(A)
#define NRF8001_GPIO        SPI1_GPIO
#define FLASH_GPIO          SPI1_GPIO
#define FLASH_nRESET        PIN(0)
#define FLASH_nCS           PIN(1)
#define NRF8001_RST         PIN(2)
#define NRF8001_RDYN        PIN(3)
#define NRF8001_REQN        PIN(4)
#define SPI1_SCK            PIN(5)
#define SPI1_MISO           PIN(6)
#define SPI1_MOSI           PIN(7)
#define SPI1_SCK_SRC        PIN_SRC(5)
#define SPI1_MISO_SRC       PIN_SRC(6)
#define SPI1_MOSI_SRC       PIN_SRC(7)
#define SPI1_PINS           (SPI1_SCK |  SPI1_MOSI | SPI1_MISO) // NRF8001_REQN

/** MS5611 barometric sensor and bmx055 9-axis */
#define BMX055_EN_GPIO      GPIO(A)
#define BMX055_EN_MAG       PIN(8)
#define BMX055_EN_ACC       PIN(9)
#define BMX055_EN_GYR       PIN(10)
#define BMX055_EN_PINS      (BMX055_EN_MAG | BMX055_EN_ACC | BMX055_EN_GYR)

#define MS5611_PORT         SPI2
#define BMX055_PORT         SPI2
#define SPI2_GPIO           GPIO(B)
#define MS5611_GPIO         SPI2_GPIO
#define BMX055_GPIO         SPI2_GPIO
#define MS5611_EN           PIN(12)
#define SPI2_SCK            PIN(13)
#define SPI2_MISO           PIN(14)
#define SPI2_MOSI           PIN(15)
#define SPI2_SCK_SRC        GPIO_PinSource13
#define SPI2_MISO_SRC       GPIO_PinSource14
#define SPI2_MOSI_SRC       GPIO_PinSource15
#define SPI2_PINS          (SPI2_SCK | SPI2_MOSI | SPI2_MISO)

#define BMX055_INT_GPIO     GPIO(B)
#define BMX055_INT_ACC      PIN(4)

/** Fre-fall alert LEDs */
#define ALERT_HIGH_SELECT
#define ALERT_HIGH_ENABLE


#endif //  __PINS_H

