# Generic AVR make file, should be configured
# Make's default rules work on most cases, but just mess stuff up on avr.
.SUFFIXES:
# Settings for hardware and interface
TARGET?=atmega328p
PROGRAMER?=avrisp2
F_CPU?=1000000
FLASHOP?=

# All object files in program
OBJ?=
# Prebuild object files
LIBS?=

# Stuff no one worres about until it is a problem
CFLAGS?=-Os -mcall-prologues -Wall -DF_CPU=$(F_CPU) -mmcu=$(TARGET)
ASFLAGS?=--defsym F_CPU=$(C_CPU) -mmcu=$(TARGET)
CC=avr-gcc
AS=avr-as
AVRDUDE=avrdude -c $(PROGRAMER) -p $(TARGET)
SIZE=avr-size -C

# Default target, build and print flash usage
.DEFAULT_GOAL := default
default: all.elf size

# Generic rule for compiling ASM
%.asm.o: %.asm
	$(AS) $(ASFLAGS) -c $< -o $@

# Generic rule for compiling C files
%.c.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link all objects into final binary
all.elf: $(OBJ)
	$(CC) $(CFLAGS) -o all.elf $(OBJ) $(LIBS)

# Remove build artifacts
clean:
	rm all.elf $(OBJ)

# Display rom ussage
size: all.elf
	$(SIZE) --mcu=$(TARGET) all.elf

# Shortcuts for flashing rom
flash: all.elf
	$(AVRDUDE) $(FLASHOP) -U flash:w:all.elf -e

flashkeep: all.elf
	$(AVRDUDE)  -U flash:r:dump.hex
	$(AVRDUDE) $(FLASHOP) -U flash:w:all.elf -U eeprom:w:dump.hex

erase:
	$(AVRDUDE) -e

avrdude:
	$(AVRDUDE) -t
