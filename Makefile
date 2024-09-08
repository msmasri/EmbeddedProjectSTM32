############################## Toolchain definitions ###########################

TOOLCHAIN_ROOT = /usr/bin/
Compiler = $(TOOLCHAIN_ROOT)arm-none-eabi-gcc
Debuger = $(TOOLCHAIN_ROOT)gdb-multiarch

###############################################################################

############################## Some definitions ################################
ProjectName = EmbeddedProjectSTM32
###############################################################################

# Project specific
TARGET = $(ProjectName).elf
BIN_TARGET = $(ProjectName).bin
SRC_DIR = ./Core/Src/
INC_DIR = ./Core/Inc/

# Project sources
SRC_FILES = $(wildcard $(SRC_DIR)*.c) $(wildcard $(SRC_DIR)*/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)*.s) $(wildcard $(SRC_DIR)*/*.s)
LD_SCRIPT = ./STM32F446RETX_FLASH.ld 

########################## Include files #######################################
# Project includes
INCLUDES   = -I$(INC_DIR)

ASM_FILES += ./Core/Startup/startup_stm32f446retx.s
SRC_FILES += \
./Core/Tasks/Communication/WithPC/USART.c \
./Core/Tasks/MicrocontrollerOutputClock/OutputClock.c \
./Core/Tasks/MotorDriverUnit/DCMotorDriver.c \
./Core/Tasks/RealTimeUnit/RTU.c \
./Core/Tasks/SystemClockConfigurtion/ConfigSystemClock.c \
./Core/Tasks/LEDs/Led.c \
./Core/main.c \

############################ Compiler Flags ####################################
CFLAGS = -g -O0 -Wall -Wextra -Warray-bounds -Wno-unused-parameter \
		-mcpu=cortex-m4 -mthumb -mlittle-endian -mthumb-interwork \
 		-mfloat-abi=hard -mfpu=fpv4-sp-d16 \
		-DSTM32F446xx -DUSE_STM32F4_NUCLEO_F446RE \
		$(INCLUDES)
########################### Linker Flags #######################################

LFLAGS = -Wl,--gc-sections -Wl,-T$(LD_SCRIPT) --specs=rdimon.specs

################################################################################

# This does an in-source build. An out-of-source build that places all object
# files into a build directory would be a better solution, but the goal was to
# keep this file very simple.

CXX_OBJS = $(SRC_FILES:.c=.o)
ASM_OBJS = $(ASM_FILES:.s=.o)
ALL_OBJS = $(ASM_OBJS) $(CXX_OBJS)

.PHONY: clean gdb-server_stlink gdb-server_openocd gdb-client

all: $(TARGET)

# Compile
$(CXX_OBJS): %.o: %.c
$(ASM_OBJS): %.o: %.s
$(ALL_OBJS):
	@echo "[Compiler] $@"
	@$(Compiler) $(CFLAGS) -c $< -o $@

# Link
%.elf: $(ALL_OBJS)
	@echo "[Linker] $@"
	@$(Compiler) $(CFLAGS) $(LFLAGS) $(ALL_OBJS) -o $@

# Clean
clean:
	@rm -f $(ALL_OBJS) $(TARGET) $(BIN_TARGET)

####################### debugging section #####################################
gdb-server_stlink:
	st-util

gdb-server_openocd:
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
            -f /usr/share/openocd/scripts/target/stm32f4x.cfg

gdb-client: $(TARGET)
	$(Debuger) -tui $(TARGET) 

####################### flashing elf file over stlink  #########################
erase:
	st-flash --reset erase
read:
	st-flash read $(TARGET) 0x8000000 0x100

uart:
	screen /dev/ttyACM0

# should be tested flashst
flashst: all
	st-flash --reset write $(TARGET) 0x8000000

flash: all
	openocd -f  /usr/share/openocd/scripts/interface/stlink.cfg \
	-f /usr/share/openocd/scripts/target/stm32f4x.cfg \
	-c "program $(TARGET) verify reset exit"


####################### some test "it should checked"  #########################
speed: all
	openocd -f  /usr/share/openocd/scripts/tools/test_cpu_speed.tcl
	
test: all
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
	-c "transport select hla_swd" \
	-c "adapter speed 1800" \
	-f /usr/share/openocd/scripts/test/selftest.cfg

stlink: all
	openocd -f /usr/share/openocd/scripts/interface/stlink.cfg \
	-c "transport select hla_swd" \
	-c "adapter speed 1800" \
	-f /usr/share/openocd/scripts/target/stm32f4x.cfg

# does not work
debug: all
	$(Debuger) $(TARGET)
	target remote localhost:3333 ####you have to check the localhost number
	monitor reset halt

####################### For analysing elf file #################################

readelf: all
	arm-none-eabi-readelf --all $(TARGET)
objcopy: all
	arm-none-eabi-objcopy -O binary $(TARGET) $(BIN_TARGET)
size: all
	arm-none-eabi-size $(TARGET)