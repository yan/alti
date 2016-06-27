include_directories(../include)

set (AERO_SRC
    globals.c
    logger.c
    buffered_io.c
)

if (NOT test)
  set (AERO_SRC
    ${AERO_SRC}
    util.c
    config.c
    filter.c
    hal.c
    init.c
    settings.c
  )
  
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
    hal_stm32_libopencm3.c
    hal_stm32_stdperiphlib.c
    periph.c
    spi.c
    #startup_stm32l1xx_hd.s
    )

  set (AERO_SRC
    ${AERO_SRC}
    fault_handlers.c
    hooks.c)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32L1")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__NVIC_PRIO_BITS=4")
else ()
  file (GLOB TEST_SRC test/*)
  set (AERO_SRC
    ${AERO_SRC}
    ${TEST_SRC}
  )
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING=1")

endif ()

add_library (aero ${AERO_SRC})

if (test)
  add_dependencies(aero gtest)
endif ()
