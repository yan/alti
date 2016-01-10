/*
 *
 *
 *
 */

#include <periph.h>
#include <hal.h>
#include <pins.h>

void arch_config_io(void)
{
  /* Configure SPI for nrf8001 and flash */
  pin_config(BT_STORE_GPIO, BT_STORE_PINS, PINMODE_AF_5);
  arch_spi_config(BT_STORE);

  /* Configure SPI for ms5611 and bmx055 */
  pin_config(SENSORS_GPIO, SENSORS_PINS, PINMODE_AF_5);
  arch_spi_config(SENSORS);
  
  /* Configure UART for ublox GPS */
  arch_config_uart(UBLOX_MAX7_BUS, 9600);

  /* Configure all enable pins */
#define __PIN(name) { name ## _GPIO, name }
  struct {
    gpio_t port;
    pin_t pin;
  } init_pins[] = {
    __PIN(MS5611_EN),
    __PIN(BMX055_EN_ACC),
    __PIN(BMX055_EN_GYRO),
    __PIN(BMX055_EN_MAG),
    __PIN(ADESTO_FLASH_CS),
    __PIN(STATUS_LED),
    __PIN(UBLOX_MAX7_RESET),
    __PIN(WARN_LED_A),
    __PIN(WARN_LED_B),
    __PIN(PIEZO_EN),
    // __PIN(PIEZO_OUT),
  };
#undef __PIN

  for (uint16_t i = 0; i < sizeof(init_pins)/sizeof(init_pins[0]); i++) {
    pin_config(init_pins[i].port, init_pins[i].pin, PINMODE_OUTPUT);
    pin_set(init_pins[i].port, init_pins[i].pin);
  }

}

void enable_piezo(void)
{

  pin_config(PIEZO_EN_GPIO, PIEZO_EN, PINMODE_OUTPUT);
  pin_set(PIEZO_EN_GPIO, PIEZO_EN);

  pin_config(PIEZO_OUT_GPIO, PIEZO_OUT, PINMODE_AF_2);

  pin_clear(PIEZO_OUT_GPIO, PIEZO_OUT);

  timer_enable(PIEZO_OUT_TIMER);
}

void disable_piezo(void)
{
  timer_disable(PIEZO_OUT_TIMER);

  pin_config(PIEZO_EN_GPIO, PIEZO_EN, PINMODE_OUTPUT);
  pin_clear(PIEZO_EN_GPIO, PIEZO_EN);
  pin_clear(PIEZO_OUT_GPIO, PIEZO_OUT);
}

void enable_pulse(void)
{
  pin_config(STATUS_LED_GPIO, STATUS_LED, PINMODE_AF_1);

  timer_enable(STATUS_LED_TIMER);
}


void disable_pulse(void)
{
  timer_disable(STATUS_LED_TIMER);
  pin_config(STATUS_LED_GPIO, STATUS_LED, PINMODE_OUTPUT);
}
