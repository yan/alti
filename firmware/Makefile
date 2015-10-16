
BINARY = aero

PID_DIR := build/pid

SRC_DIR  = src/

SRC     = main.c hooks.c fault_handlers.c

LDLIBS += aero

TESTING ?= 0

ifneq ($(TESTING),1)
LDLIBS += freertos 
endif

include build/alternatives.mk
include build/Makefile.libfreertos
include build/include.mk


