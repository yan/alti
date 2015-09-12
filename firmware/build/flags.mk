

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
# C flags

CFLAGS		+= -g # -Os
CFLAGS		+= -Wextra -Wshadow -Wimplicit-function-declaration
CFLAGS		+= -Wredundant-decls -Wmissing-prototypes -Wstrict-prototypes
CFLAGS          += -Wno-unused-function
CFLAGS		+= -fno-common -ffunction-sections -fdata-sections

###############################################################################
# C++ flags

CXXFLAGS	+= -g # -Os
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

LDFLAGS		+= --static -nostartfiles
LDFLAGS		+= -L$(LIB_DIR)
LDFLAGS         += -L$(MISC_LIB_DIR)
LDFLAGS		+= -L$(SUPPORT_DIR)
LDFLAGS		+= -T$(LDSCRIPT)
LDFLAGS		+= -Wl,-Map=$(OBJ_DIR)/$(*).map
LDFLAGS		+= -Wl,--gc-sections
LDFLAGS		+= -Wl,--start-group
LDFLAGS         += -lc -lm -lgcc -lnosys
LDFLAGS		+= -Wl,--end-group
ifeq ($(V),99)
LDFLAGS		+= -Wl,--print-gc-sections
endif

###############################################################################
# Semihosting support
GDB_CMDS = support/gdb_commands
ifeq ($(ENABLE_SEMIHOSTING), 1)
  LDFLAGS += --specs=rdimon.specs
  LDLIBS += rdimon
  CPPFLAGS += -DENABLE_SEMIHOSTING=1
  $(shell grep -q semihosting $(GDB_CMDS) || echo 'mon arm semihosting enable' >> $(GDB_CMDS))
else
  $(shell [ -f $(GDB_CMDS) ] && sed -i '' '/mon arm semihosting/d' $(GDB_CMDS))
endif # ENABLE_SEMIHOSTING
