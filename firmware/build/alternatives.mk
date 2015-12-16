
USE_ST_LIB ?= 1

ifneq ($(TESTING),1)
  ifeq ($(USE_ST_LIB),1)
    LIB := stm32l1_stdperiphlib
  else
    LIB := opencm3_stm32l1
  endif

  LDLIBS += $(LIB)
  include build/Makefile.lib$(LIB)
endif

