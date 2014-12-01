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

what_to_build:: all

-include local.mk

TOOLCHAIN ?= arm-none-eabi-

BOARD ?= panda

TARGET_CC := $(TOOLCHAIN)gcc
TARGET_LD := $(TOOLCHAIN)ld
TARGET_OBJCOPY := $(TOOLCHAIN)objcopy
TARGET_OBJDUMP := $(TOOLCHAIN)objdump

TARGET_CFLAGS := -g -Os  -Wall
TARGET_CFLAGS +=  -march=armv7-a -fno-builtin -ffreestanding
TARGET_CFLAGS += -I. -Iinclude
TARGET_CFLAGS += -include config_$(BOARD).h

TARGET_LIBGCC := $(shell $(TARGET_CC) $(TARGET_CFLAGS) -print-libgcc-file-name)

HOST_CFLAGS := -g -O2 -Wall
HOST_CFLAGS += -Itools

OUT := out/$(BOARD)
OUT_HOST_OBJ := $(OUT)/host-obj
OUT_TARGET_OBJ := $(OUT)/target-obj

ALL :=

include build/rules.mk

M_NAME := usbboot
M_OBJS := tools/usbboot.o
M_OBJS += tools/usb_linux.o
M_OBJS += 2ndstage.o
include build/host-executable.mk

M_NAME := mkheader
M_OBJS := tools/mkheader.o
include build/host-executable.mk

M_NAME := bin2c
M_OBJS := tools/bin2c.o
include build/host-executable.mk

M_NAME := aboot
M_BASE := 0x40309000
M_OBJS := arch/omap4/start.o
M_OBJS += arch/omap4/serial.o 
M_OBJS += arch/omap4/clock.o
M_OBJS += arch/omap4/sdram.o
M_OBJS += arch/omap4/gpmc.o
M_OBJS += arch/omap4/gpio.o
M_OBJS += arch/omap4/id.o
M_OBJS += arch/omap4/rom_usb.o
M_OBJS += board_$(BOARD).o
M_OBJS += libc/printf.o 
M_OBJS += libc/strlen.o libc/memset.o libc/memcpy.o
M_OBJS += libc/raise.o
M_OBJS += aboot.o 
M_OBJS += trusted.o
M_OBJS += boot.o
M_OBJS += misc.o
M_LIBS := $(TARGET_LIBGCC)
include build/target-executable.mk

M_NAME := agent
M_BASE := 0x82000000
M_OBJS := arch/common/start.o
M_OBJS += agent.o
M_OBJS += arch/omap4/serial.o

include build/target-executable.mk

$(OUT)/aboot.ift: $(OUT)/aboot.bin $(OUT)/mkheader
	@echo generate $@
	@./$(OUT)/mkheader $(TEXT_BASE) `wc -c $(OUT)/aboot.bin` > $@
	@cat $(OUT)/aboot.bin >> $@
ALL += $(OUT)/aboot.ift

$(OUT_HOST_OBJ)/2ndstage.o: $(OUT)/aboot.bin $(OUT)/bin2c
	@echo generate $@
	$(QUIET)./$(OUT)/bin2c aboot < $(OUT)/aboot.bin > $(OUT)/2ndstage.c
	gcc -c -o $@ $(OUT)/2ndstage.c

clean::
	@echo clean
	@rm -rf $(OUT)

all:: $(ALL)

# we generate .d as a side-effect of compiling. override generic rule:
%.d:
-include $(DEPS)
