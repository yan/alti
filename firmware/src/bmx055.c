/**
 * Copyright 2015 Yan Ivnitskiy
 */
#include <bmx055.h>
#include <hal.h>

#include <pins.h>
#include <util.h>

#define SPI_PORT BMX055_PORT
#define BYTEORDER BYTEORDER_MSB
#define USE_SPI

#include <transfer_macros.h>


/**
 * Note: Current hardware is not working, so tabling this until a new board is 
 * made
 */
void bmx055_reset(void)
{
  int response;

  spi_send_msb_first(BMX055_PORT);

  pin_clear(BMX055_EN_GPIO, BMX055_EN_GYR);

  send_byte(0x00); // BMX055_ACC_WHOAMI

  response = read8();

  pin_set(BMX055_EN_GPIO, BMX055_EN_ACC);

  dbg_print("WHOAMI? %d\n", response);
  (void) response;
}

void bmx055_init(void)
{
  bmx055_reset();


}
