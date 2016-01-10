
cmake_minimum_required (VERSION 2.6)
project (Alti C CXX ASM)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

option (test "Build a testing image" OFF)
option (stlib "Use the ST stm32 peripheral library" ON)

set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PROJECT_SOURCE_DIR}/support -T${PROJECT_SOURCE_DIR}/support/stm32l15xxb.ld")

set (STM32_DEV_TYPE STM32L1XX_HD)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")#  -Os -O2")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${STM32_DEV_TYPE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING=0 -DENABLE_SEMIHOSTING=0")

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/support/cmake")

add_executable (alti.elf ${AERO_SRC} src/main.c src/hooks.c)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

include (FreeRTOS)

if (stlib)
  include (Stm32PeriphLib)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32_STDPERIPH_LIB")
  set (LIBRARY stm32periph)
else () 
  include (Libopencm3)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOPENCM3")
  set (LIBRARY opencm3_stm32l1)
endif ()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

target_link_libraries (alti.elf aero ${LIBRARY})

if (test)
  add_subdirectory ("support/gtest")
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  #find_package (GTest REQUIRED)
  
  #add_executable (alti.elf ${AERO_SRC} src/main.c src/hooks.c)
else()
  if (NOT CMAKE_CROSSCOMPILING)
    message (SEND_ERROR "You need to set a toolchain file when building a firmware image (check support/cmake/).")
  endif ()
endif ()

