
USE_BOOTLOADER	= false

H		= .
M		= .
S		= $(H)

vpath %.c $(M):$(S)

ifndef GCCPREFIX
    GCCPREFIX   = /usr/local/pic32-tools/bin/pic32-
endif

DEFS		+= -I. -I$(H)

ifeq ($(USE_BOOTLOADER),true)
LDRFILE		= bootldr.ld 
else
LDRFILE		= bare-metal.ld 
endif

DEPFLAGS	= -MT $@ -MD -MP -MF .deps/$*.dep
CFLAGS		= -O $(DEFS) $(DEPFLAGS)
ASFLAGS		= $(DEFS) $(DEPFLAGS)

DEVICE	= 32MX764F128H
#DEVICE	= 32MX795F512L
AVRDUDE = ../mpide-0023-linux-20120903/hardware/tools/avrdude
AVRDUDECONF = -C ../mpide-0023-linux-20120903/hardware/tools/avrdude.conf -c stk500v2 \
		-p pic32 -P /dev/ttyUSB0 -q -q

PIC32PROG = /usr/local/bin/pic32prog
PIC32PROGCONF = -p
#PIC32PROGCONF =

CC		= $(GCCPREFIX)gcc -EL -g -mips32r2 -mprocessor=$(DEVICE) -O3
CC		+= -Wextra -Wall -fno-exceptions -ffunction-sections
CC		+= -fdata-sections -mno-smart-io 
ifeq ($(USE_BOOTLOADER),true)
CC		+= -D__USE_BOOTLDR__
endif

LDFLAGS         += -T $(LDRFILE) -lc -lm -Wl,-Map=picnc.map,--gc-sections
#LDFLAGS         += --defsym=_min_heap_size=16000 --defsym=_min_stack_size=2048

SIZE		= $(GCCPREFIX)size
OBJDUMP		= $(GCCPREFIX)objdump
OBJCOPY		= $(GCCPREFIX)objcopy
BIN2HEX		= $(GCCPREFIX)bin2hex

SRCOBJ	= main.o stepgen.o

.SUFFIXES:

.SUFFIXES:	.hex .dis .c .elf .o

all:		.deps picnc.elf picnc.hex
		$(SIZE) picnc.elf
clean:
		rm -rf .deps *.o *.elf *.bin *.dis *.map *.hex *.dep

.deps:
		mkdir .deps


picnc.elf:	$(SRCOBJ) $(LDRFILE)
		$(CC) $(LDFLAGS) $(SRCOBJ) -o $@
		chmod -x $@
		$(OBJDUMP) -d -S $@ > picnc.dis
		$(BIN2HEX) $@

load:           .deps picnc.elf
		$(PIC32PROG) $(PIC32PROGCONF) picnc.hex

.o.dis:
		$(OBJDUMP) -d -z -S $< > $@

ifeq (.deps, $(wildcard .deps))
-include .deps/*.dep
endif
