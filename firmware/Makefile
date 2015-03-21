
BINARY = aero

OPENCM3_DIR = libopencm3

LDSCRIPT = $(OPENCM3_DIR)/lib/stm32/l1/stm32l15xxb.ld

ENABLE_SEMIHOSTING ?= 1

LIB_DIR = build/lib
LIBS = freertos 

OBJS = main.o
LDFLAGS += -L$(LIB_DIR)

$(LIBS:%=$(LIB_DIR)/lib%.a):
	make -f build/Makefile.$(*F)

all: $(LIBS:%=$(LIB_DIR)/lib%.a)

clean:
	make -f build/Makefile.libfreertos clean

.PHONY = clean

