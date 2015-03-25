
BINARY = aero
OPENCM3_DIR = ./libopencm3

LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/l1/stm32l15xxb.ld

ENABLE_SEMIHOSTING ?= 0

TOPLEVEL_LIB_DIR = build/lib

LIBS := opencm3 freertos

PID_DIR := build/pid
OBJ_DIR := build/obj
SRC_DIR := src/

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=%.o)

#$(info [${SRCS}])
#$(info [${OBJS}])

LDFLAGS += -L$(TOPLEVEL_LIB_DIR)
LDLIBS += -lm -lfreertos

FREERTOS_DIR = freertos/FreeRTOS/Source/
CPPFLAGS += -Iinclude -I$(FREERTOS_DIR)/include -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3

GDB_CMDS = support/gdb_commands
OPENOCD_CFG = /usr/local/share/openocd/scripts/board/stm32ldiscovery.cfg

ifeq ($(ENABLE_SEMIHOSTING), 1)
  LDFLAGS += --specs=rdimon.specs
  LDLIBS += -lrdimon
  CPPFLAGS += -DENABLE_SEMIHOSTING=1
  $(shell grep -q semihosting $(GDB_CMDS) || echo 'mon arm semihosting enable' >> $(GDB_CMDS))
else
  $(shell [ -f $(GDB_CMDS) ] && sed -i '' '/mon arm semihosting/d' $(GDB_CMDS))
endif


include build/include.mk

$(LIBS:%=$(TOPLEVEL_LIB_DIR)/lib%.a):
	make -f build/Makefile.$(*F)

libs: $(OBJ_DIR) $(TOPLEVEL_LIB_DIR) $(LIBS:%=$(TOPLEVEL_LIB_DIR)/lib%.a)

$(TOPLEVEL_LIB_DIR):
	mkdir -p $(TOPLEVEL_LIB_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

cleanlib:
	make -f build/Makefile.libfreertos clean
	make -f build/Makefile.libopencm3 clean

run: elf
	mkdir -p $(PID_DIR)
	killall openocd || true
	@openocd -f $(OPENOCD_CFG) -l "${PID_DIR}/openocd.log" & echo "$$!" > "${PID_DIR}/openocd.pid"
	@$(PREFIX)-gdb --command=$(GDB_CMDS) $(OBJ_DIR)/$(BINARY).elf
	@kill `cat $(PID_DIR)/openocd.pid`
