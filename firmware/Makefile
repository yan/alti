
ENABLE_SEMIHOSTING ?= 1

LIB_DIR = build/lib

LIBS := opencm3 freertos

LDFLAGS += -L$(LIB_DIR)

$(LIBS:%=$(LIB_DIR)/lib%.a):
	make -f build/Makefile.$(*F)

all: $(LIBS:%=$(LIB_DIR)/lib%.a)

clean:
	make -f build/Makefile.libfreertos clean
	make -f build/Makefile.libopencm3 clean

.PHONY = clean

