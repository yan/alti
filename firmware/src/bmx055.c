/**
 * Copyright 2015 Yan Ivnitskiy
 */
#include <stddef.h>
#include <stdint.h>

#include <bmx055.h>
#include <hal.h>
#include <pins.h>
#include <util.h>


enum operation_e {
  OP_READ,
  OP_WRITE
};

enum sensor_e {
  ACCEL,
  MAG,
  GYRO
};

#define bmx055_get(sensor, reg) bmx055_xfer(OP_READ, sensor, reg, NULL)

#define bmx055_write(sensor, reg, val) bmx055_xfer(OP_WRITE, sensor, reg, val)

static uint8_t
bmx055_xfer(enum operation_e op, enum sensor_e sensor, uint8_t reg, uint8_t *val)
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

  if (op == OP_READ) {
    reg |= 0x80;
  }

  pin_clear(BMX055_EN_ACC_GPIO, cs);

  /* Wait after selecting device (resuse data var for this) */
  for(data = 0; data < 20; data++)
    ;

  /* Send the reg address */
  arch_spi_xfer(BMX055_BUS, reg);

  
  if (op == OP_WRITE) {
    assert(val != NULL);

    data = *val;
  } else {
    data = 0;
  }

  result = arch_spi_xfer(BMX055_BUS, data);

  pin_set(BMX055_EN_ACC_GPIO, cs);

  return result;
}


int16_t bmx055_read(enum bmx055_device_e device, enum direction_e direction)
{
  uint16_t result;
  uint8_t byte, address;

  assert(device == BMX055_ACCEL);

  switch (direction) {
    case DIR_X:
      address = BMX055_ACC_X_LSB_ADDR;
      break;
    case DIR_Y:
      address = BMX055_ACC_Y_LSB_ADDR;
      break;
    case DIR_Z:
      address = BMX055_ACC_Z_LSB_ADDR;
      break;
    default:
      assert(0);
      break;
  }

  byte = bmx055_get(ACCEL, address);
  result = byte;

  byte = bmx055_get(ACCEL, address + 1);
  result |= (uint16_t) byte  << 8;

  return (int16_t) result;
}

/**
 * Note: Current hardware is not working, so tabling this until a new board is 
 * made
 */
int bmx055_reset(void)
{
  //int response = 1;

  if (bmx055_get(ACCEL, 0x00) != 0xFA) {
    return 0;
  }

  return 1;
}

void bmx055_init(void)
{
  bmx055_reset();


}
