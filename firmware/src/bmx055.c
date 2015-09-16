/**
 * Copyright 2015 Yan Ivnitskiy
 */
#include <bmx055.h>
#include <hal.h>
#include <pins.h>
#include <util.h>


/**
 * Note: Current hardware is not working, so tabling this until a new board is 
 * made
 */
void bmx055_reset(void)
{
  int response;

  spi_set_msb(BMX055_BUS);

  pin_clear(BMX055_EN_ACC_GPIO, BMX055_EN_ACC);

  arch_spi_xfer(BMX055_BUS, 0x00); // BMX055_ACC_WHOAMI

  response = arch_spi_xfer(BMX055_BUS, 1);

  pin_set(BMX055_EN_ACC_GPIO, BMX055_EN_ACC);

  dbg_print("WHOAMI? %d\n", response);
  (void) response;
}

void bmx055_init(void)
{
  bmx055_reset();


}
