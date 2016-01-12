
cmake_minimum_required (VERSION 2.6)
project (Alti C CXX ASM)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

option (test "Build a testing image" ON)
option (stlib "Use the ST stm32 peripheral library" ON)

set (STM32_DEV_TYPE STM32L1XX_HD)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3") 
#set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -O2")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${STM32_DEV_TYPE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DTESTING=0 -DENABLE_SEMIHOSTING=0")

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/support/cmake")

add_executable (alti.elf ${AERO_SRC} src/main.c src/hooks.c)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")


if (test)
  include (ExternalProject)
  ExternalProject_Add(
    gtest
    URL https://github.com/google/googletest/archive/release-1.7.0.tar.gz
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    INSTALL_COMMAND ""
  )

  add_library(libgtest IMPORTED STATIC GLOBAL)
  add_dependencies(libgtest gtest)

  # Set gtest properties
  ExternalProject_Get_Property(gtest source_dir binary_dir)
  set_target_properties(libgtest PROPERTIES
      "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
      "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
  #    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
  )
  # I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
  include_directories("${source_dir}/include")

  # add_subdirectory ("support/gtest")
  enable_testing()
  # include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  # find_package (GTest REQUIRED)
  target_link_libraries(alti.elf ${GTEST_LIBRARIES})

else()

  if (NOT CMAKE_CROSSCOMPILING)
    message (SEND_ERROR "You need to set a toolchain file when building a firmware image (check support/cmake/).")
  endif ()

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
endif ()

target_link_libraries (alti.elf aero ${LIBRARY})
