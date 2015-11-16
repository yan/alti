
USE_ST_LIB ?= 0

ifneq ($(TESTING),1)
  ifeq ($(USE_ST_LIB),1)
    LDLIBS += stm32l1_stdperiphlib
    include build/Makefile.libstm32l1_stdperiphlib
  else
    LDLIBS += opencm3_stm32l1
    include build/Makefile.libopencm3_stm32l1
  endif
endif
