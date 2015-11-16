/**
 * Copyright 2015 Yan Ivnitskiy
 */

#define BMX055_ACC_X_LSB_ADDR              0x02
#define BMX055_ACC_X_MSB_ADDR              0x03
#define BMX055_ACC_Y_LSB_ADDR              0x04
#define BMX055_ACC_Y_MSB_ADDR              0x05
#define BMX055_ACC_Z_LSB_ADDR              0x06
#define BMX055_ACC_Z_MSB_ADDR              0x07

int bmx055_reset(void);

enum direction_e {
  X,
  Y,
  Z
};

int16_t bmx055_read_acc(enum direction_e direction);

void bmx055_init(void);
