
## set (CMAKE_C_COMPILER arm-none-eabi-gcc)
## set (CMAKE_CXX_COMPILER arm-none-eabi-g++)
## set (CMAKE_ASM_COMPILER arm-none-eabi-as)
## set (CMAKE_AR arm-none-eabi-ar)
## set (CMAKE_RANLIB arm-none-eabi-ranlib)
## 
## set (can_use_assembler TRUE)

set (CMAKE_TOOLCHAIN_FILE "support/cmake/Toolchain-STM32.cmake")

cmake_minimum_required (VERSION 2.6)
project (Alti)
set (Alti_VERSION_MAJOR 0)
set (Alti_VERSION_MINOR 1)
configure_file (
  "${PROJECT_SOURCE_DIR}/include/config.h.in" "${PROJECT_BINARY_DIR}/include/config.h"
)

ENABLE_LANGUAGE(C)
ENABLE_LANGUAGE(CXX)
ENABLE_LANGUAGE(ASM)

### ifeq ($(SMALL),)
### OPT_FLAGS       += -g
### else
### OPT_FLAGS       += -O3 -Os
### endif
### 
### CFLAGS          += $(OPT_FLAGS) -std=c11
### CFLAGS          += -Wextra -Wshadow -Wimplicit-function-declaration
### CFLAGS          += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
### CFLAGS          += -Wno-unused-function
### CFLAGS          += -fno-common -ffunction-sections -fdata-sections
### 
### ###############################################################################
### # C++ flags
### 
### CXXFLAGS        += $(OPT_FLAGS)
### CXXFLAGS        += -Wextra -Wshadow -Wredundant-decls  -Weffc++
### CXXFLAGS        += -fno-common -ffunction-sections -fdata-sections
### 
### ###############################################################################
### # C & C++ preprocessor common flags
### 
### CPPFLAGS        += -MD
### CPPFLAGS        += -Wall -Wundef -Werror
### CPPFLAGS        += $(USER_CPPFLAGS)
### CPPFLAGS        += -Iinclude
### CPPFLAGS  += -DENABLE_SEMIHOSTING=$(ENABLE_SEMIHOSTING)
### CPPFLAGS        += $(DEFS)

# set (CMAKE_C_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -Werror -Wextra -std=gnu99 -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "c compiler flags")
# set (CMAKE_CXX_FLAGS "-mthumb -fno-builtin -mcpu=cortex-m3 -Wall -Werror -Wextra -std=c++11 -ffunction-sections -fdata-sections -fomit-frame-pointer -fno-unroll-loops -ffast-math -ftree-vectorize" CACHE INTERNAL "cxx compiler flags")
# set (CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "asm compiler flags")
#set (CMAKE_EXE_LINKER_FLAGS "-Wl,--gc-sections -mthumb -mcpu=cortex-m3" CACHE INTERNAL "executable linker flags")
set (CMAKE_EXE_LINKER_FLAGS "-L${PROJECT_SOURCE_DIR}/support -T${PROJECT_SOURCE_DIR}/support/stm32l15xxb.ld ${CMAKE_EXE_LINKER_FLAGS}")
set (CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "module linker flags")
set (CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "shared linker flags")


set (STM32_DEV_TYPE STM32L1XX_HD)
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Os -O3")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D${STM32_DEV_TYPE}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSTM32_STDPERIPH_LIB")

set (CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/support/cmake")

include (Stm32PeriphLib)
include (FreeRTOS)

include_directories("${PROJECT_BINARY_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/include")

add_subdirectory("${PROJECT_SOURCE_DIR}/src")

add_executable (Alti ${AERO_SRC} src/main.c src/hooks.c)

target_link_libraries (Alti aero stm32periph)

