cmake_minimum_required (VERSION 2.6)
project (Alti)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable (Alti src/main.c)
