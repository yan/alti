
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
## CFLAGS          += $(OPT_FLAGS) -std=c11
## CFLAGS          += -Wextra -Wshadow -Wimplicit-function-declaration
## CFLAGS          += -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
## CFLAGS          += -Wno-unused-function
## CFLAGS          += -fno-common -ffunction-sections -fdata-sections
## 
## ###############################################################################
## # C++ flags
## 
## CXXFLAGS        += $(OPT_FLAGS)
## CXXFLAGS        += -Wextra -Wshadow -Wredundant-decls  -Weffc++
## CXXFLAGS        += -fno-common -ffunction-sections -fdata-sections
## 
## ###############################################################################
## # C & C++ preprocessor common flags
##       
## CPPFLAGS        += -MD 
## CPPFLAGS        += -Wall -Wundef -Werror
## CPPFLAGS        += $(USER_CPPFLAGS)
## CPPFLAGS        += -Iinclude
## CPPFLAGS  += -DENABLE_SEMIHOSTING=$(ENABLE_SEMIHOSTING)
## CPPFLAGS        += $(DEFS)

set (CPPFLAGS "-MD -Wall -Wundef -Werror -fno-builtin -Wextra -Wshadow -Wno-unused-function -DTESTING=0 -DENABLE_SEMIHOSTING=0")
set (ARCHFLAGS "-mthumb -mcpu=cortex-m3")
set (CMAKE_C_FLAGS "${ARCHFLAGS} ${CPPFLAGS} -std=c11 -Wimplicit-function-declaration" CACHE INTERNAL "c compiler flags")
set (CMAKE_CXX_FLAGS "${ARCHFLAGS} ${CPPFLAGS} -std=c++11" CACHE INTERNAL "cxx compiler flags")

set (CMAKE_EXE_LINKER_FLAGS "--static -nostartfiles -Wl,--start-group -lm -lgcc -lnosys -Wl,--end-group -Wl,--gc-sections" CACHE INTERNAL "executable linker flags")
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

