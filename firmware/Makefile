
BINARY = aero

OPENCM3_DIR = ./libopencm3

LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/l1/stm32l15xxb.ld

ENABLE_SEMIHOSTING ?= 0

TOPLEVEL_LIB_DIR = build/lib

LIBS := opencm3 freertos

OBJ_DIR := build/obj
SRC_DIR := src/

#OBJS = aero.o

LDFLAGS += -L$(TOPLEVEL_LIB_DIR)
LDLIBS += -lm -lfreertos

FREERTOS_DIR = freertos/FreeRTOS/Source/
CPPFLAGS += -Iinclude -I$(FREERTOS_DIR)/include -I$(FREERTOS_DIR)/portable/GCC/ARM_CM3

ifeq ($(ENABLE_SEMIHOSTING), 1)
  LDFLAGS += --specs=rdimon.specs
  LDLIBS += lrdimon
  CPPFLAGS += -DENABLE_SEMIHOSTING=1
  $(shell grep -q semihosting gdb_cmds || echo 'mon arm semihosting enable' >> gdb_cmds)
else
  $(shell [ -f gdb_cmds ] && sed -i '' '/mon arm semihosting/d' gdb_cmds)
endif


include build/libopencm3.include.mk

$(LIBS:%=$(TOPLEVEL_LIB_DIR)/lib%.a):
	make -f build/Makefile.$(*F)

libs: $(TOPLEVEL_LIB_DIR) $(LIBS:%=$(TOPLEVEL_LIB_DIR)/lib%.a)

$(TOPLEVEL_LIB_DIR):
	mkdir -p $(TOPLEVEL_LIB_DIR)

cleanlib:
	make -f build/Makefile.libfreertos clean
	make -f build/Makefile.libopencm3 clean
