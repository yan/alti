include (ExternalProject)

ExternalProject_Add(libopencm3
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND make TARGETS=stm32/l1
  BUILD_BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3/lib/libopencm3_stm32l1.a
  INSTALL_COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/libopencm3/lib/libopencm3_stm32l1.a ${PROJECT_BINARY_DIR}
  BUILD_IN_SOURCE 1
)

add_dependencies(alti.elf libopencm3)

include_directories(${CMAKE_SOURCE_DIR}/libopencm3/include)

link_directories(${PROJECT_SOURCE_DIR}/libopencm3/lib)

add_library(opencm3_stm32l1 STATIC IMPORTED)
set_property(TARGET opencm3_stm32l1
             PROPERTY IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/libopencm3/lib/libopencm3_stm32l1.a)
add_dependencies (opencm3_stm32l1 libopencm3)

