##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
##
## This library is free software: you can redistribute it and/or modify
## it under the terms of the GNU Lesser General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## This library is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU Lesser General Public License for more details.
##
## You should have received a copy of the GNU Lesser General Public License
## along with this library.  If not, see <http://www.gnu.org/licenses/>.
##

ifneq ($(TESTING),1)
DEFS		+= -DSTM32L1  -DTESTING=0

#FP_FLAGS	?= -msoft-float
FP_FLAGS	?= -mfloat-abi=soft
ARCH_FLAGS	?= -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -mfix-cortex-m3-ldrd
#ARCH_FLAGS	?= -target arm-none-eabi -mthumb -mcpu=cortex-m3 $(FP_FLAGS) -nostdlib -ffreestanding
else
DEFS            = -DTESTING=1
endif


################################################################################
# OpenOCD specific variables

OOCD		?= openocd
OOCD_INTERFACE	?= stlink-v2
OOCD_TARGET	?= stm32lx_stlink

################################################################################
# texane/stlink specific variables
#STLINK_PORT	?= :4242


include build/rules.mk
#include build/paths.mk
