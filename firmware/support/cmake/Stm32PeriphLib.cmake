

set (STM32_HOME "stm32l1_stdperiphlib/Libraries")
set (STM32_LIB ${STM32_HOME}/STM32L1xx_StdPeriph_Driver)
set (STM32_CMSIS ${STM32_HOME}/CMSIS)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -include stm32l1xx_conf.h")

include_directories (${STM32_CMSIS}/Device/ST/STM32L1xx/Include)
include_directories (${STM32_CMSIS}/Include)
include_directories (${STM32_LIB}/inc)

file (GLOB STM32_SRC "${STM32_LIB}/src/*.c")

add_library (stm32periph ${STM32_SRC})
