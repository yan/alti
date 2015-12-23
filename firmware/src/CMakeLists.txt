set (AERO_SRC
  util.c
  config.c
  filter.c
  hal.c
  init.c
  logger.c)

set (AERO_SRC
  ${AERO_SRC}
  bmx055.c
  ublox.c
  ublox_isr.c
  flash.c
  ms5611.c
  nrf8001.c)

set (AERO_SRC
  ${AERO_SRC}
  task_alert.c
  task_ble.c
  task_gps.c
  task_main.c
  task_sensor.c)

set (AERO_SRC
  ${AERO_SRC}
  aero.c
  aero_test.c
  fault_handlers.c
  globals.c
  hal_stm32_libopencm3.c
  hal_stm32_stdperiphlib.c
  hal_stm32_test.c
  hooks.c
  main.c
  periph.c
  spi.c
  startup_stm32l1xx_hd.s)
