
PROJECT = gsm-doorbell
PORT    = ttyACM0

ARDUINO_DIR = /home/yus/.arduino15/packages/arduino
ARMGCC_DIR  = $(ARDUINO_DIR)/tools/arm-none-eabi-gcc/4.8.3-2014q1
CMSIS_DIR   = $(ARDUINO_DIR)/tools/CMSIS/4.5.0
SAMD_DIR    = $(ARDUINO_DIR)/hardware/samd/1.6.16
CORE_DIR    = $(SAMD_DIR)/cores/arduino
VARIANT_DIR = $(SAMD_DIR)/variants/arduino_zero
BOSSA_DIR   = $(ARDUINO_DIR)/tools/bossac/1.7.0


CXX = $(ARMGCC_DIR)/bin/arm-none-eabi-g++
CC  = $(ARMGCC_DIR)/bin/arm-none-eabi-gcc
AS  = $(CC) -g -x assembler-with-cpp
AR  = $(ARMGCC_DIR)/bin/arm-none-eabi-ar
LD  = $(CC)
OBJCOPY = $(ARMGCC_DIR)/bin/arm-none-eabi-objcopy
BOSSAC  = $(BOSSA_DIR)/bossac


LINKER_SCRIPT = $(VARIANT_DIR)/linker_scripts/gcc/flash_with_bootloader.ld

USER_OBJECTS = $(PROJECT).o utils.o fona.o

VARIANT_OBJECT = $(VARIANT_DIR)/variant.o

CORE_OBJECTS = $(CORE_DIR)/pulse_asm.o \
		$(CORE_DIR)/WInterrupts.o \
		$(CORE_DIR)/cortex_handlers.o \
		$(CORE_DIR)/delay.o \
		$(CORE_DIR)/hooks.o \
		$(CORE_DIR)/itoa.o \
		$(CORE_DIR)/pulse.o \
		$(CORE_DIR)/startup.o \
		$(CORE_DIR)/wiring.o \
		$(CORE_DIR)/wiring_analog.o \
		$(CORE_DIR)/wiring_digital.o \
		$(CORE_DIR)/wiring_private.o \
		$(CORE_DIR)/wiring_shift.o \
		$(CORE_DIR)/USB/samd21_host.o \
		$(CORE_DIR)/avr/dtostrf.o \
		$(CORE_DIR)/IPAddress.o \
		$(CORE_DIR)/Print.o \
		$(CORE_DIR)/Reset.o \
		$(CORE_DIR)/RingBuffer.o \
		$(CORE_DIR)/SERCOM.o \
		$(CORE_DIR)/Stream.o \
		$(CORE_DIR)/Tone.o \
		$(CORE_DIR)/Uart.o \
		$(CORE_DIR)/WMath.o \
		$(CORE_DIR)/WString.o \
		$(CORE_DIR)/abi.o \
		$(CORE_DIR)/main.o \
		$(CORE_DIR)/new.o \
		$(CORE_DIR)/USB/CDC.o \
		$(CORE_DIR)/USB/PluggableUSB.o \
		$(CORE_DIR)/USB/USBCore.o

CORE_LIB = core.a

DEFINES  = -DF_CPU=48000000L -DARDUINO=10805 \
		-DARDUINO_SAMD_ZERO -DARDUINO_ARCH_SAMD  -D__SAMD21G18A__ \
		-DUSB_VID=0x2341 -DUSB_PID=0x804d -DUSBCON '-DUSB_MANUFACTURER="Arduino LLC"'

INCLUDES = -I$(CMSIS_DIR)/CMSIS/Include \
		-I$(ARDUINO_DIR)/tools/CMSIS-Atmel/1.1.0/CMSIS/Device/ATMEL \
		-I$(CORE_DIR)  -I$(VARIANT_DIR)

COMMON_CFLAGS = -mcpu=cortex-m0plus -mthumb \
		-g -Os  $(INCLUDES) $(DEFINES) -Wall -Wextra \
		-ffunction-sections -fdata-sections \
		-nostdlib --param max-inline-insns-single=500 -MMD 

CFLAGS   = $(COMMON_CFLAGS) -std=gnu11
CPPFLAGS = $(COMMON_CFLAGS) -fno-threadsafe-statics  -fno-rtti -fno-exceptions -std=gnu++11

LDFLAGS  = -Os -Wl,--gc-sections -save-temps -T$(LINKER_SCRIPT) -Wl,-Map,$(PROJECT).map \
		--specs=nano.specs --specs=nosys.specs -mcpu=cortex-m0plus -mthumb \
		-Wl,--cref -Wl,--check-sections -Wl,--gc-sections \
		-Wl,--unresolved-symbols=report-all -Wl,--warn-common \
		-Wl,--warn-section-align $(USER_OBJECTS) $(VARIANT_OBJECT) \
		-Wl,--start-group -L$(CMSIS_DIR)/CMSIS/Lib/GCC -larm_cortexM0l_math -lm $(CORE_LIB) -Wl,--end-group


all: $(USER_OBJECTS) $(VARIANT_OBJECT) $(CORE_LIB)
	$(LD) $(LDFLAGS) -o $(PROJECT).elf
	$(OBJCOPY) -O binary  $(PROJECT).elf $(PROJECT).bin

clean:
	rm -rf $(USER_OBJECTS) $(VARIANT_OBJECT) $(CORE_OBJECTS) $(CORE_LIB) $(PROJECT).elf $(PROJECT).map $(PROJECT).bin

bootload:
	@stty -F /dev/$(PORT) 1200 && sleep 2
	$(BOSSAC) --port=$(PORT) -U true -i -e -w -v $(PROJECT).bin -R

monitor:
	sleep 2 && picocom /dev/$(PORT) -b 115200

$(CORE_LIB): $(CORE_OBJECTS)
	$(AR) cr $@ $^


.SUFFIXES: .c .cpp .o .S .ino

.c.o :
	$(CC) $(CFLAGS) -c -o $@ $<

.S.o :
	$(AS) $(DEFINES) $(INCLUDES) -c -o $@ $<

.ino.o :
	$(CXX) -x c++ $(CPPFLAGS) -c -o $@ $<
