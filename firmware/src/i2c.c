
#include <errno.h>
#include <unistd.h>

#include <FreeRTOS.h>
#include <semphr.h>

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

// #include "aero.h"
#include "i2c.h"
#include "ms5611.h"
#include "util.h"
#include "pins.h"


static void i2c_prepare_to_read(uint32_t i2c);


/** @brief ...
 */
void i2c_config(uint32_t i2c)
{
  rcc_periph_clock_enable(RCC_I2C1);

  i2c_reset(i2c);
  i2c_peripheral_disable(i2c);

  // i2c_enable_ack(i2c);
  i2c_set_dutycycle(i2c, I2C_CCR_DUTY_DIV2); /* Default */

#define FREQ (I2C_CR2_FREQ_12MHZ)
  i2c_set_clock_frequency(i2c, FREQ);
  i2c_set_ccr(i2c, (FREQ*1000000) / (100000 * 2)); /* clock / (100khz * 2) */
  i2c_set_trise(i2c, 25);
#undef FREQ

  i2c_peripheral_enable(i2c);
}


void i2c_enable(void)
{
  rcc_periph_clock_enable(RCC_I2C1);
  i2c_peripheral_enable(I2C1);
}

/** @brief
 */
void i2c_disable(void)
{
  rcc_periph_clock_disable(RCC_I2C1);
  i2c_peripheral_disable(RCC_I2C1);
}

/**
 * @brief
 */
void i2c_send_cmd(uint32_t i2c, uint8_t cmd)
{
  i2c_send_buf(i2c, &cmd, sizeof(cmd));
}

/**
 * @brief
 */
void i2c_send_buf(uint32_t i2c, uint8_t *buf, uint32_t length)
{
  while ((I2C_SR2(i2c) & I2C_SR2_BUSY));

  i2c_send_start(i2c);

  /* Wait for master mode selected */
  while (!((I2C_SR1(i2c) & I2C_SR1_SB)
    & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

  i2c_send_7bit_address(i2c, MS5611_ADDR, I2C_WRITE);

  /* Waiting for address is transferred. */
  while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

  /* Cleaning ADDR condition sequence. */
  uint32_t reg32 = I2C_SR2(i2c);
  (void) reg32; /* unused */

  size_t i;
  for (i = 0; i < length; i++) {
    i2c_send_data(i2c, buf[i]);
    while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
  }

  i2c_send_stop(i2c);
}

/**
 * @brief
 */
static void i2c_prepare_to_read(uint32_t i2c) {
  i2c_send_start(i2c);
  i2c_enable_ack(i2c);

  /* Wait for master mode selected */
  while (!((I2C_SR1(i2c) & I2C_SR1_SB)
    & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

  i2c_send_7bit_address(i2c, MS5611_ADDR, I2C_READ);

  /* Waiting for address is transferred. */
  while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
  /* Cleaning ADDR condition sequence. */
  uint32_t reg32 = I2C_SR2(i2c);
  (void) reg32; /* unused */
}

/**
 * @brief
 */
void i2c_read_data(uint32_t i2c, uint8_t *data, uint32_t length)
{
  i2c_prepare_to_read(i2c);

  uint32_t i = 0;
  for (i = 0; i < length; ++i) {
    if(i == length - 1) {
      i2c_disable_ack(i2c);
    }
    while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
    data[i] = i2c_get_data(i2c);
  }

  i2c_send_stop(i2c);
}

/**
 * @brief
 */
uint32_t i2c_read_octets(uint32_t i2c, unsigned octets)
{
  uint32_t value = 0;

  assert(octets > 0 && octets <= 4);

  i2c_prepare_to_read(i2c);

  do {
    if (--octets == 0) {
      i2c_disable_ack(i2c);
    }

    while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
    value |= i2c_get_data(i2c) << (octets * 8);

  } while (octets);

  i2c_send_stop(i2c);

  return value;
}


