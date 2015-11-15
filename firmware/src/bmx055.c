/**
 * Copyright 2015 Yan Ivnitskiy
 */
#include <bmx055.h>
#include <hal.h>
#include <pins.h>
#include <util.h>


enum direction_e {
  DIR_READ,
  DIR_WRITE
};

enum sensor_e {
  ACCEL,
  MAG,
  GYRO
};

#define bmx055_read(sensor, reg, val) bmx055_xfer(DIR_READ, sensor, reg, val)

#define bmx055_write(sensor, reg, val) bmx055_xfer(DIR_WRITE, sensor, reg, val)

static uint8_t
bmx055_xfer(enum direction_e dir, enum sensor_e sensor, uint8_t reg, uint8_t *val)
{
  uint8_t result, data;
  pin_t cs;

  spi_set_msb(BMX055_BUS);

  switch (sensor) {
    case ACCEL:
      cs = BMX055_EN_ACC;
      break;
    case MAG:
      cs = BMX055_EN_MAG;
      break;
    case GYRO:
      cs = BMX055_EN_GYRO;
      break;
  }

  if (dir == DIR_READ) {
    reg |= 0x80;
  }

  pin_clear(BMX055_EN_ACC_GPIO, cs);

  /* Wait after selecting device (resuse data var for this) */
  for(data = 0; data < 20; data++)
    ;

  /* Send the reg address */
  arch_spi_xfer(BMX055_BUS, reg);

  
  if (dir == DIR_WRITE) {
    data = *val;
  } else {
    data = 0;
  }

  result = arch_spi_xfer(BMX055_BUS, data);

  pin_set(BMX055_EN_ACC_GPIO, cs);

  return result;
}


/**
 * Note: Current hardware is not working, so tabling this until a new board is 
 * made
 */
int bmx055_reset(void)
{
  //int response = 1;

  if (bmx055_read(ACCEL, 0x00, 0x00) != 0xFA) {
    return 0;
  }

  return 1;
}

void bmx055_init(void)
{
  bmx055_reset();


}
