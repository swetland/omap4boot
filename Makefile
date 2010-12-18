##
## Copyright (C) 2010 The Android Open Source Project
## All rights reserved.
##
## Redistribution and use in source and binary forms, with or without
## modification, are permitted provided that the following conditions
## are met:
##  * Redistributions of source code must retain the above copyright
##    notice, this list of conditions and the following disclaimer.
##  * Redistributions in binary form must reproduce the above copyright
##    notice, this list of conditions and the following disclaimer in
##    the documentation and/or other materials provided with the 
##    distribution.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
## "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
## LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
## COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
## INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
## BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
## OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
## AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
## OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
## OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
## SUCH DAMAGE.


TEXT_BASE := 0x40300000

#TC := /work/atc/bin/arm-linux-androideabi-
TC := arm-eabi-

CC := $(TC)gcc
LD := $(TC)ld
OBJCOPY := $(TC)objcopy
OBJDUMP := $(TC)objdump

CFLAGS := -g -Os  -Wall
CFLAGS +=  -march=armv7-a -fno-builtin -ffreestanding
CFLAGS += -I. -Iinclude

LIBGCC := $(shell $(TC)gcc $(CFLAGS) -print-libgcc-file-name)

OBJS := arch/omap4/start.o
OBJS +=	arch/omap4/serial.o 
OBJS += arch/omap4/clock.o
OBJS += arch/omap4/sdram.o
OBJS += arch/omap4/gpmc.o
OBJS += libc/printf.o 
OBJS += libc/strlen.o libc/memset.o libc/memcpy.o
OBJS += aboot.o 
OBJS += misc.o

LIBS := $(LIBGCC)

all: aboot.bin aboot.lst aboot.ift usbboot

usbboot: tools/usbboot.c tools/usb_linux.c
	gcc -O2 -Wall -Itools -o usbboot tools/usbboot.c tools/usb_linux.c

mkheader: tools/mkheader.c
	gcc -O2 -Wall -Itools -o mkheader tools/mkheader.c

aboot.bin: aboot
	$(OBJCOPY) --gap-fill=0xee -O binary aboot aboot.bin

aboot.ift: aboot.bin mkheader
	./mkheader $(TEXT_BASE) `wc -c aboot.bin` > aboot.ift
	cat aboot.bin >> aboot.ift

aboot.lst: aboot
	$(OBJDUMP) -D aboot > aboot.lst

aboot: $(OBJS)
	$(LD) -Bstatic -T aboot.lds -Ttext $(TEXT_BASE) $(OBJS) $(LIBS) -o aboot

clean::
	rm -f aboot aboot.bin aboot.ift aboot.lst mkheader usbboot
	rm -f arch/omap4/*.o arch/omap4/*~
	rm -f libc/*.o libc/*~
	rm -f *.o *~

.S.o:
	$(CC) $(CFLAGS) -o $@ -c $<
