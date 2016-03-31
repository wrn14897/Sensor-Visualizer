ARMGNU ?= arm-none-eabi

CC = $(ARMGNU)-gcc
LD = $(ARMGNU)-ld
AS = $(ARMGNU)-as
OBJCOPY = $(ARMGNU)-objcopy
OBJDUMP = $(ARMGNU)-objdump
STRIP = $(ARMGNU)-strip

CPPFLAGS = -Iinclude/
CFLAGS = -std=c99 -Wall -Og -g -ffreestanding $(CPPFLAGS)
ASFLAGS =
LDFLAGS  = -nostdlib -T memmap -Llib/
LDLIBS = -lpi -lgcc

.SUFFIXES:

NAME = main
C_SRCS = $(NAME).c keyboard.c console.c circular.c cstart.c lib/strtol.c malloc.c gl.c fb.c timer.c printf.c timer-int.c gprof.c adc.c visualizer.c sine.c
S_SRCS = start.s

all : $(NAME).bin

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM $(CFLAGS) -c $< > $*.d

%.exe: $(S_SRCS:.s=.o) $(C_SRCS:.c=.o)
	$(LD) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.bin: %.exe
	$(OBJCOPY) $< -O binary $@

%.list: %.o
	$(OBJDUMP) -d --no-show-raw-insn $< > $@

clean :
	rm -f *.bin *.exe *.o *.d *.list
	rm -f lib/*.bin lib/*.exe lib/*.o lib/*.d lib/*.list

install: $(NAME).bin
	rpi-install.py $(NAME).bin

.PHONY: all clean

.PRECIOUS: %.exe %.o

# include dependency info
#  "-" suppresses errors
-include $(SRCS:.c=.d)
