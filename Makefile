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

-include local.mk

TEXT_BASE ?= 0x40300000

TOOLCHAIN ?= arm-eabi-

TARGET_CC := $(TOOLCHAIN)gcc
TARGET_LD := $(TOOLCHAIN)ld
TARGET_OBJCOPY := $(TOOLCHAIN)objcopy
TARGET_OBJDUMP := $(TOOLCHAIN)objdump

TARGET_CFLAGS := -g -Os  -Wall
TARGET_CFLAGS +=  -march=armv7-a -fno-builtin -ffreestanding
TARGET_CFLAGS += -I. -Iinclude

HOST_CFLAGS := -g -O2 -Wall

LIBGCC := $(shell $(TARGET_CC) $(TARGET_CFLAGS) -print-libgcc-file-name)

OBJS := arch/omap4/start.o
OBJS +=	arch/omap4/serial.o 
OBJS += arch/omap4/clock.o
OBJS += arch/omap4/sdram.o
OBJS += arch/omap4/gpmc.o
OBJS += arch/omap4/rom_usb.o
OBJS += libc/printf.o 
OBJS += libc/strlen.o libc/memset.o libc/memcpy.o
OBJS += libc/raise.o
OBJS += aboot.o 
OBJS += misc.o

LIBS := $(LIBGCC)

OUT := out

all: $(OUT)/aboot.bin $(OUT)/aboot.lst $(OUT)/aboot.ift $(OUT)/usbboot

HOSTOBJ := $(OUT)/host-obj
TARGETOBJ := $(OUT)/target-obj

OBJS := $(addprefix $(TARGETOBJ)/,$(OBJS))
DEPS += $(OBJS:%o=%d)

MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi

QUIET ?= @

$(HOSTOBJ)/%.o: %.c
	@$(MKDIR)
	@echo compile $<
	$(QUIET)$(CC) $(HOST_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)
$(HOSTOBJ)/%.o: %.S
	@$(MKDIR)
	@echo assemble $<
	$(QUIET)$(CC) $(HOST_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)
$(TARGETOBJ)/%.o: %.c
	@$(MKDIR)
	@echo compile $<
	$(QUIET)$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)
$(TARGETOBJ)/%.o: %.S
	@$(MKDIR)
	@echo assemble $<
	$(QUIET)$(TARGET_CC) $(TARGET_CFLAGS) -c $< -o $@ -MD -MT $@ -MF $(@:%o=%d)

$(OUT)/2ndstage.o: $(OUT)/mkheader $(OUT)/aboot.bin
	@echo generate $@
	objcopy --input-target binary --output-target `objdump -f $(OUT)/mkheader | grep "file format" | sed 's/.*format //g'` --binary-architecture `objdump -f $(OUT)/mkheader | grep "architecture" | sed 's/architecture: //g' | sed 's/,.*//g'` $(OUT)/aboot.bin $(OUT)/2ndstage.o

$(OUT)/usbboot: tools/usbboot.c tools/usb_linux.c $(OUT)/2ndstage.o
	@echo build $@
	$(QUIET)gcc $(HOST_CFLAGS) -Itools -o $@ tools/usbboot.c tools/usb_linux.c $(OUT)/2ndstage.o

$(OUT)/mkheader: tools/mkheader.c
	@echo build $@
	$(QUIET)gcc $(HOST_CFLAGS) -Itools -o $@ tools/mkheader.c

$(OUT)/aboot.bin: $(OUT)/aboot
	@echo create $@
	$(QUIET)$(TARGET_OBJCOPY) --gap-fill=0xee -O binary $(OUT)/aboot $@

$(OUT)/aboot.ift: $(OUT)/aboot.bin $(OUT)/mkheader
	@echo generate $@
	@./$(OUT)/mkheader $(TEXT_BASE) `wc -c $(OUT)/aboot.bin` > $@
	@cat $(OUT)/aboot.bin >> $@

$(OUT)/aboot.lst: $(OUT)/aboot
	@echo create $@
	$(QUIET)$(TARGET_OBJDUMP) -D $(OUT)/aboot > $@

$(OUT)/aboot: $(OBJS)
	@echo link $@
	$(QUIET)$(TARGET_LD) -Bstatic -T aboot.lds -Ttext $(TEXT_BASE) $(OBJS) $(LIBS) -o $@

clean::
	@echo clean
	@rm -rf $(OUT)

# we generate .d as a side-effect of compiling. override generic rule:
%.d:
-include $(DEPS)
