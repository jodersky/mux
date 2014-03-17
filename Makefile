# CPU model
MCU=atmega2560

# CPU Frequency
F_CPU=16000000L

# Name of image to produce (can be arbitrary)
TARGET=firmware

# Serial port used for uploading
SERIAL=/dev/ttyACM0
BAUD=115200

# Modules to include in kernel
MODULES=bug collection task tshield time

# Toolchain flags
CC=avr-gcc
CFLAGS= -std=gnu99 -O2 -Wall -finline-functions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mmcu=$(MCU) -DF_CPU=$(F_CPU)
LD=avr-gcc
LDFLAGS= -Wl,--gc-sections,--relax -mmcu=$(MCU)
AR=avr-ar
AS=avr-as
ASFLAGS=-mmcu=$(MCU)
OC=avr-objcopy
OCFLAGS=-O ihex -R .eeprom
AVRDUDE=avrdude
AVRDUDEFLAGS= -DV -p $(MCU) -P $(SERIAL) -c stk500v2 -b 115200
AVRSIZE=avr-size
CPP=avr-g++
CPPFLAGS= -O2 -Wall -fno-exceptions -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -mmcu=$(MCU) -DF_CPU=$(F_CPU)
GDBINITFILE=gdb.conf

# Derived variables
SOURCES=\
	$(foreach module,$(MODULES),$(wildcard kernel/$(module)/*.c)) \
	$(foreach module,$(MODULES),$(wildcard kernel/$(module)/mcu/$(MCU)/*.S)) \
	$(foreach module,$(MODULES),$(wildcard kernel/$(module)/mcu/$(MCU)/*.c)) \
	$(wildcard arduino/*.s) $(wildcard arduino/*.c) $(wildcard arduino/*.cpp) \
	$(wildcard *.s) $(wildcard *.c) $(wildcard *.cpp)
	
OBJECTS=$(addsuffix .o, $(basename $(SOURCES)))

INCLUDES=\
	$(foreach module, $(MODULES), kernel/$(module)/include) \
	$(foreach module, $(MODULES), kernel/$(module)/mcu/$(MCU)/include)

# Rules
all: target

target: $(TARGET).hex

$(TARGET).hex: $(TARGET).elf
	$(OC) $(OCFLAGS) $< $@

$(TARGET).elf: $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^
	
# Kernel sources
kernel/%.o: kernel/%.S
	$(CC) $(CFLAGS) -o $@ -c $<
	
kernel/%.o: kernel/%.c
	$(CC) $(CFLAGS) -I$(dir $<) $(addprefix -I, $(INCLUDES)) -o $@ -c $<
	
# Local sources
%.o: %.s
	$(CC) $(CFLAGS) -I$(dir $<) $(addprefix -I, $(INCLUDES)) -o $@ -c $<
	
%.o: %.c
	$(CC) $(CFLAGS) -I$(dir $<) $(addprefix -I, $(INCLUDES)) -o $@ -c $<
	
%.o: %.cpp
	$(CPP) $(CPPFLAGS) -I$(dir $<) $(addprefix -I, $(INCLUDES)) -o $@ -c $<

# Utility rules
size: target
	$(AVRSIZE) --format=avr --mcu=$(MCU) $(TARGET).elf

upload: target
	$(AVRDUDE) $(AVRDUDEFLAGS) -U flash:w:$(TARGET).hex:i
	
monitor:
	cu -l $(SERIAL) -s $(BAUD) --parity=none -h

cycle:
	@make clean && \
	make > /dev/null && \
	make size && \
	read -p "Press enter to proceed with upload..." nothing && \
	make upload

clean:
	@rm -f *.o
	@for module in $(MODULES); do \
		rm -f kernel/$$module/*.o; \
		rm -f kernel/$$module/mcu/$(MCU)/*.o; \
	done
	@rm -f arduino/*.o
	@rm -f $(TARGET).hex
	@rm -f $(TARGET).elf
	@rm -f $(GDBINITFILE)
	
# Debugging
ddd: gdbinit
	ddd --debugger "avr-gdb -x $(GDBINITFILE)"

gdbserver: gdbinit
#	run_avr -g -m $(MCU) -f $(F_CPU)
	simulavr -p 1234 -d $(MCU) -F 16000000 -f $(TARGET).elf -g

gdbinit: $(GDBINITFILE)

$(GDBINITFILE): $(TARGET).elf
	@echo "file $(TARGET).elf" > $(GDBINITFILE)
	@echo "target remote localhost:1234" >> $(GDBINITFILE)
	@echo "load" >> $(GDBINITFILE) 
	@echo "break main.c:main" >> $(GDBINITFILE)
	@echo
	@echo "Use 'avr-gdb -x $(GDBINITFILE)'"

	
.PHONY: clean cycle
