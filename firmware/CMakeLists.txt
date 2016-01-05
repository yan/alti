
cmake_minimum_required (VERSION 2.6)
project (Alti C CXX ASM)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

option (test "Build a testing image" OFF)

set (CMAKE_EXE_LINKER_FLAGS "-L${PROJECT_SOURCE_DIR}/support -T${PROJECT_SOURCE_DIR}/support/stm32l15xxb.ld ${CMAKE_EXE_LINKER_FLAGS}")
#set (CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "module linker flags")
#set (CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "shared linker flags")

set (STM32_DEV_TYPE STM32L1XX_HD)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Os -O2")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${STM32_DEV_TYPE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32_STDPERIPH_LIB")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING=0 -DENABLE_SEMIHOSTING=0")

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/support/cmake")

include (Stm32PeriphLib)
include (FreeRTOS)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

if (test)
  add_subdirectory ("support/gtest")
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  #find_package (GTest REQUIRED)
  message (STATUS ${GTEST_INCLUDE_DIRS})
  
  #add_executable (alti.elf ${AERO_SRC} src/main.c src/hooks.c)
endif ()

add_executable (alti.elf ${AERO_SRC} src/main.c src/hooks.c)

target_link_libraries (alti.elf aero stm32periph)

