

###############################################################################
# Project-specific paths
OBJ_DIR		?= build/obj
LIB_DIR		?= build/lib
SUPPORT_DIR     ?= support/
OUTPUT_DIRS     ?= $(OBJ_DIR) $(LIB_DIR)
TO_CLEAN        = $(OBJ_DIR)/*
LDSCRIPT	= $(SUPPORT_DIR)/stm32l15xxb.ld
DONT_ARCHIVE    ?= 

###############################################################################
# Executables

ifneq ($(TESTING),1)
PREFIX		?= arm-none-eabi-
CC		:= $(PREFIX)gcc
CXX		:= $(PREFIX)g++
else
PREFIX		?= 
CC              := clang
CXX		:= clang
CFLAGS          := --analyze
endif

CXX		:= $(PREFIX)g++
LD		:= $(PREFIX)gcc
AR		:= $(PREFIX)ar
AS		:= $(PREFIX)as
SIZE		:= $(PREFIX)size
OBJCOPY		:= $(PREFIX)objcopy
OBJDUMP		:= $(PREFIX)objdump
GDB		:= $(PREFIX)gdb-py
STFLASH		= $(shell which st-flash)

###############################################################################
# C flags

ifeq ($(SMALL),)
OPT_FLAGS	+= -g
else
OPT_FLAGS	+= -O3 -Os
endif

CFLAGS		+= $(OPT_FLAGS)
CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS          += -Wno-unused-function
CFLAGS		+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C++ flags

CXXFLAGS	+= $(OPT_FLAGS)
CXXFLAGS	+= -Wextra -Wshadow -Wredundant-decls  -Weffc++
CXXFLAGS	+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C & C++ preprocessor common flags

CPPFLAGS	+= -MD
CPPFLAGS	+= -Wall -Wundef -Werror
CPPFLAGS        += $(USER_CPPFLAGS)
CPPFLAGS	+= -Iinclude
CPPFLAGS	+= $(DEFS)

###############################################################################
# Linker flags

ifneq ($(TESTING),1)
LDFLAGS		+= --static -nostartfiles
LDFLAGS		+= -T$(LDSCRIPT)
LDFLAGS		+= -Wl,-Map=$(OBJ_DIR)/$(*).map
LDFLAGS		+= -Wl,--gc-sections
LDFLAGS		+= -Wl,--start-group
LDFLAGS         += -lm -lgcc -lnosys
LDFLAGS		+= -Wl,--end-group
ifeq ($(V),99)
LDFLAGS		+= -Wl,--print-gc-sections
endif
endif

# Need libdirs even when testing
LDFLAGS		+= -L$(LIB_DIR)
LDFLAGS		+= -L$(SUPPORT_DIR)
ifneq ($(MISC_LIB_DIR),)
LDFLAGS         += -L$(MISC_LIB_DIR)
endif

###############################################################################
# Semihosting support
GDB_CMDS = support/gdb_commands
ifeq ($(ENABLE_SEMIHOSTING), 1)
  LDFLAGS += --specs=rdimon.specs -lc -lrdimon
  LDSYSLIBS += rdimon
  CPPFLAGS += -DENABLE_SEMIHOSTING=1
  $(shell grep -q semihosting $(GDB_CMDS) || echo 'mon arm semihosting enable' >> $(GDB_CMDS))
else
  $(shell [ -f $(GDB_CMDS) ] && sed -i '' '/mon arm semihosting/d' $(GDB_CMDS))
endif # ENABLE_SEMIHOSTING
