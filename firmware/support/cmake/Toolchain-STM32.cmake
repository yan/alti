
# the name of the target operating system
#
set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_VERSION   1)
set(CMAKE_SYSTEM_PROCESSOR arm-eabi)

# which compilers to use for C and C++
#
set(CMAKE_C_COMPILER       arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER     arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER     arm-none-eabi-as)
set(CMAKE_OBJCOPY          arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP          arm-none-eabi-objdump)

set(TOOCHAIN_LIB_DIR "/Users/user/tools/gcc-arm-none-eabi-4_9-2015q3/lib")

set (CPPFLAGS "-MD")
set (CPPFLAGS "${CPPFLAGS} -Wall -Wundef -Werror -Wshadow -Wno-unused-function")
set (CPPFLAGS "${CPPFLAGS} -ffunction-sections -fdata-sections -fno-common")
set (CPPFLAGS "${CPPFLAGS} -DSTM32L1")
set (CPPFLAGS "${CPPFLAGS} -D__NVIC_PRIO_BITS=4")

set (ARCHFLAGS "-mthumb -mcpu=cortex-m3")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARCHFLAGS} ${CPPFLAGS} -std=c11 -Wstrict-prototypes" CACHE INTERNAL "c compiler flags")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARCHFLAGS} ${CPPFLAGS} -std=c++11" CACHE INTERNAL "cxx compiler flags")


set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --static -nostartfiles ")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--start-group -lm -lgcc -lnosys -Wl,--end-group")
set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections" CACHE INTERNAL "exe link flags")

set (CMAKE_MODULE_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "module linker flags")
set (CMAKE_SHARED_LINKER_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "shared linker flags")

# --static -nostartfiles -Tsupport//stm32l15xxb.ld -Wl,-Map=build/obj/aero.map -Wl,--gc-sections -Wl,--start-group -lm -lgcc -lnosys -Wl,--end-group -Lbuild/lib -Lsupport/ -mthumb -mcpu=cortex-m3 -mfloat-abi=soft -mfix-cortex-m3-ldrd


SET(CMAKE_ASM_FLAGS "-mthumb -mcpu=cortex-m3" CACHE INTERNAL "asm compiler flags")

# SET(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Wl,--gc-sections -mthumb -mcpu=cortex-m4" CACHE INTERNAL "exe link flags")

#link_directories(${TOOCHAIN_LIB_DIR})

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment,
# search programs in the host environment
#
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

