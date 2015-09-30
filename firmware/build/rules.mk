##
## This file is part of the libopencm3 project.
##
## Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
## Copyright (C) 2010 Piotr Esden-Tempski <piotr@esden.net>
## Copyright (C) 2013 Frantisek Burian <BuFran@seznam.cz>
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

# Be silent per default, but 'make V=1' will show all compiler calls.
ifneq ($(V),1)
Q		:= @
NULL		:= 2>/dev/null
endif


###############################################################################
# Source files

OBJS ?= $(addprefix $(OBJ_DIR)/, $(SRC:%.c=%.o))


include build/flags.mk


###############################################################################

# Only build the final binaries if we're not building a library
ifeq ($(LIBNAME),)

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: output_dirs elf

elf: $(OBJ_DIR)/$(BINARY).elf
bin: $(OBJ_DIR)/$(BINARY).bin
hex: $(OBJ_DIR)/$(BINARY).hex
srec: $(OBJ_DIR)/$(BINARY).srec
list: $(OBJ_DIR)/$(BINARY).list

images: $(OBJ_DIR)/$(BINARY).images
flash: $(OBJ_DIR)/$(BINARY).flash

size:
	$(Q)$(SIZE) $(OBJ_DIR)/$(BINARY).elf

$(OBJ_DIR)/%.images: $(OBJ_DIR)/%.bin $(OBJ_DIR)/%.hex $(OBJ_DIR)/%.srec $(OBJ_DIR)/%.list $(OBJ_DIR)/%.map
	@printf "*** $* images generated ***\n"

$(OBJ_DIR)/%.bin: $(OBJ_DIR)/%.elf
	@printf "  OBJCOPY $(*).bin\n"
	$(Q)$(OBJCOPY) -Obinary $< $@

$(OBJ_DIR)/%.hex: $(OBJ_DIR)/%.elf
	@printf "  OBJCOPY $(*).hex\n"
	$(Q)$(OBJCOPY) -Oihex $< $@

$(OBJ_DIR)/%.srec: $(OBJ_DIR)/%.elf
	@printf "  OBJCOPY $(*).srec\n"
	$(Q)$(OBJCOPY) -Osrec $< $@

$(OBJ_DIR)/%.list: $(OBJ_DIR)/%.elf
	@printf "  OBJDUMP $(*).list\n"
	$(Q)$(OBJDUMP) -S $< > $@

#
$(OBJ_DIR)/%.elf: $(OBJS) $(LDSCRIPT) $(LDLIBS:%=$(LIB_DIR)/lib%.a)
	@printf "  LD      $(*).elf ($(LDLIBS))\n"
	$(Q)$(LD) $(LDFLAGS) $(ARCH_FLAGS) $(OBJS) $(LDLIBS:%=-l%) $(LDSYSLIBS:%=-l%) -o $@
	$(Q)$(SIZE) $(OBJ_DIR)/$(BINARY).elf

$(LIB_DIR)/lib%.a: 
	@printf "BUILD $*\n"
	$(Q)make $(if $(V),,-s) -f build/Makefile.lib$* V=$(V)

.PHONY: images clean elf bin hex srec list size output_dirs

else

.PHONY: output_dirs

ifeq ($(DONT_ARCHIVE),)
$(LIB_DIR)/lib$(LIBNAME).a: output_dirs $(OBJS)
	@printf "  AR      lib$(LIBNAME).a\n"
	$(Q)$(AR) rcs "$@" $(OBJS)
else
prebuilt_lib: $(LIB_DIR)/lib$(LIBNAME).a
endif

endif

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@printf "  CC      $(*).c\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cxx
	@printf "  CXX     $(*).cxx\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc
	@printf "  CXX     $(*).cc\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@printf "  CXX     $(*).cpp\n"
	$(Q)$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(ARCH_FLAGS) -o $@ -c $<

clean:
	@printf "  CLEAN\n"
	$(Q)cd $(OBJ_DIR) && $(RM) -fr *
	$(Q)$(RM) -r $(TO_CLEAN)

cleanlib:
	$(Q)$(RM) $(LIB_DIR)/*.a

cleanall: clean cleanlib

ifneq ($(OUTPUT_DIRS),)
output_dirs: $(OUTPUT_DIRS)
$(OUTPUT_DIRS):
	$(Q)mkdir -p $@
endif

%.stlink-flash: %.bin
	@printf "  FLASH  $<\n"
	$(Q)$(STFLASH) write $(*).bin 0x8000000

%.flash: %.hex
	@printf "  FLASH   $<\n"
	@# IMPORTANT: Don't use "resume", only "reset" will work correctly!
	$(OOCD) -f interface/$(OOCD_INTERFACE).cfg \
		    -f target/$(OOCD_TARGET).cfg \
		    -c "init" -c "reset init" \
		    -c "flash write_image erase $(*).hex" \
		    -c "reset" \
		    -c "shutdown"

run: elf
	$(Q)mkdir -p $(PID_DIR)
	#$(Q)killall openocd || true
	#$(Q)openocd -f interface/$(OOCD_INTERFACE).cfg -f target/$(OOCD_TARGET).cfg -c 'mon reset init' -l "${PID_DIR}/openocd.log" & echo "$$!" > "${PID_DIR}/openocd.pid"
	$(Q)$(GDB) --command=$(GDB_CMDS) $(OBJ_DIR)/$(BINARY).elf
	#$(Q)kill `cat $(PID_DIR)/openocd.pid`


-include $(OBJS:.o=.d)
