
BINARY = aero

OPENCM3_DIR = ./libopencm3

LDSCRIPT = support/stm32l15xxb.ld
LDFLAGS += -Lsupport

ENABLE_SEMIHOSTING ?= 0

TOPLEVEL_LIB_DIR = build/lib

LIBS := freertos stm32l1_stdperiphlib opencm3
# opencm3

PID_DIR := build/pid
# OBJ_DIR := build/obj
SRC_DIR := ./src

SRC = $(wildcard $(SRC_DIR)/*.c)
SRC := $(SRC:$(SRC_DIR)/%=%)


LDFLAGS += -L$(TOPLEVEL_LIB_DIR)
LDLIBS += -lm -lfreertos -lopencm3_stm32l1


OPENOCD_CFG = -f /usr/local/share/openocd/scripts/interface/stlink-v2.cfg    \
              -f /usr/local/share/openocd/scripts/target/stm32lx_stlink.cfg


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
	make -f build/Makefile.libstm32l1_stdperiphlib

run: elf
	mkdir -p $(PID_DIR)
	killall openocd || true
	openocd $(OPENOCD_CFG) -l "${PID_DIR}/openocd.log" & echo "$$!" > "${PID_DIR}/openocd.pid"
	$(PREFIX)-gdb --command=$(GDB_CMDS) $(OBJ_DIR)/$(BINARY).elf
	@kill `cat $(PID_DIR)/openocd.pid`

