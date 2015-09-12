
BINARY = aero


LIBS := freertos opencm3_stm32l1

PID_DIR := build/pid

SRC_DIR = src/

# Generate with: COLUMNS=100 ls -x src/ |sed 's/^/SRC   += /g'
SRC	= aero.c			bmx055.c			config.c
SRC	+= fault_handlers.c		filter.c			flash.c
SRC	+= globals.c			hal_stm32_libopencm3.c		hal_stm32_stdperiphlib.c
SRC	+= hooks.c			logger.c			ms5611.c
SRC	+= nrf8001.c			spi.c				task_alert.c
SRC	+= task_ble.c			task_main.c			task_sensor.c
SRC	+= util.c

LDLIBS += freertos opencm3_stm32l1

include build/include.mk


run: elf
	mkdir -p $(PID_DIR)
	killall openocd || true
	openocd $(OPENOCD_CFG) -l "${PID_DIR}/openocd.log" & echo "$$!" > "${PID_DIR}/openocd.pid"
	$(PREFIX)-gdb --command=$(GDB_CMDS) $(OBJ_DIR)/$(BINARY).elf
	@kill `cat $(PID_DIR)/openocd.pid`

