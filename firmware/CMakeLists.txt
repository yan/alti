cmake_minimum_required (VERSION 2.6)
project (Alti C CXX ASM)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

option (test "Build a testing image" ON)
option (stlib "Use the ST stm32 peripheral library" ON)

set (BINARY_NAME ${PROJECT_NAME})

# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3") 
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -O2 -Wextra")

if (NOT test)
  set (STM32_DEV_TYPE STM32L1XX_HD)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${STM32_DEV_TYPE}")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DENABLE_SEMIHOSTING=0")
  set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/support/cmake")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L${PROJECT_SOURCE_DIR}/support")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${PROJECT_SOURCE_DIR}/support/stm32l15xxb.ld")
  set (BINARY_NAME "${BINARY_NAME}.elf")
endif()

add_executable (${BINARY_NAME} ${AERO_SRC} src/main.c)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")
set_property(TARGET ${BINARY_NAME} PROPERTY CXX_STANDARD 11)

set (LIBRARIES aero)

if (test)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-extensions -g3")

  include (support/cmake/gtest.cmake)

  add_definitions("-DTESTING=1")

  enable_testing()
  add_test(NAME alti COMMAND ${BINARY_NAME})

  include_directories(${GTEST_INCLUDE_DIR})
  set (LIBRARIES ${LIBRARIES} ${GTEST_LIBRARY})

  if (CMAKE_CROSSCOMPILING)
    message (SEND_ERROR "You are cross compiling in test mode; did you forget a toolchain file?")
  endif ()
else()
  #set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING=0")
  add_definitions("-DTESTING=0")

  if (NOT CMAKE_CROSSCOMPILING)
    message (SEND_ERROR "You need to set a toolchain file when building a firmware image (check support/cmake/).")
  endif ()

  if (stlib)
    include (Stm32PeriphLib)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32_STDPERIPH_LIB")
    set (LIBRARIES ${LIBRARIES} stm32periph)
  else () 
    include (Libopencm3)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DOPENCM3")
    set (LIBRARIES ${LIBRARIES} opencm3_stm32l1)
  endif ()

  include (FreeRTOS)
  set (LIBRARIES ${LIBRARIES} freertos)

  add_custom_command(
    OUTPUT size.out
    COMMAND arm-none-eabi-size ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${BINARY_NAME}
    COMMENT "Produced binary size"
    VERBATIM
  )
  add_custom_target(
    PrintSize ALL
    DEPENDS size.out
  )
    
endif ()

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

if (test)
  target_link_libraries (${BINARY_NAME} -Wl,-all_load ${LIBRARIES} )
else ()
  target_link_libraries (${BINARY_NAME} -Wl,-\( ${LIBRARIES} -Wl,-\))
endif ()

