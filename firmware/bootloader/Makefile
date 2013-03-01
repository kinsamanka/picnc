H		= .
M		= .
S		= $(H)

vpath %.c $(M):$(S)

ifndef GCCPREFIX
    GCCPREFIX   = /usr/local/pic32-tools/bin/pic32-
endif


DEFS		+= -I. -I$(H)

LDRFILE		= 32mx764.ld

DEPFLAGS	= -MT $@ -MD -MP -MF .deps/$*.dep
CFLAGS		= -O $(DEFS) $(DEPFLAGS)
ASFLAGS		= $(DEFS) $(DEPFLAGS)

DEVICE	= 32MX764F128H

CC		= $(GCCPREFIX)gcc -EL -g -mips16 -mprocessor=$(DEVICE) -Os
CC		+= -Wextra -Wall -fno-exceptions -ffunction-sections
CC		+= -fdata-sections -mno-smart-io 
AS		= $(GCCPREFIX)gcc -c
#LDFLAGS         += -g -nostartfiles -T $(LDRFILE) -lc -lm -Wl,-Map=bootloader.map,--gc-sections
LDFLAGS         += -g -T $(LDRFILE) -lc -lm -Wl,-Map=bootloader.map,--gc-sections
#LDFLAGS         += --defsym=_min_heap_size=16000 --defsym=_min_stack_size=2048
SIZE		= $(GCCPREFIX)size
OBJDUMP		= $(GCCPREFIX)objdump
OBJCOPY		= $(GCCPREFIX)objcopy
BIN2HEX		= $(GCCPREFIX)bin2hex

SRCOBJ2	= test.o 
#SRCOBJ	= Uart.o NVMem.o Framework.o BootLoader.o crt0.o crti.o
SRCOBJ	= Uart.o NVMem.o Framework.o BootLoader.o

.SUFFIXES:

.SUFFIXES:	.hex .dis .c .elf .o .S

all:		.deps bootloader.elf bootloader.hex
		$(SIZE) bootloader.elf
clean:
		rm -rf .deps *.o *.elf *.bin *.dis *.map *.hex *.dep

.deps:
		mkdir .deps

%.o: %.S
		$(AS) $<

bootloader.elf:	$(SRCOBJ) $(LDRFILE)
		$(CC) $(LDFLAGS) $(SRCOBJ) -o $@
		chmod -x $@
		$(OBJDUMP) -d -S $@ > bootloader.dis
		$(BIN2HEX) $@

load:           all
		pic32prog -p bootloader.hex
.o.dis:
		$(OBJDUMP) -d -z -S $< > $@

ifeq (.deps, $(wildcard .deps))
-include .deps/*.dep
endif
