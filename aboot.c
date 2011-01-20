/*
 * Copyright (C) 2010 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the 
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <aboot/aboot.h>
#include <aboot/io.h>
#include <omap4/mux.h>
#include <omap4/hw.h>
#include <omap4/rom_usb.h>

static unsigned MSG = 0xaabbccdd;

struct usb usb;

#define DOWNLOAD_ADDR 0x80E80000

void aboot(void)
{
	unsigned n;
	unsigned len;

	mux_config();
	sdelay(100);

	scale_vcores();

	prcm_init();
  	ddr_init();
	gpmc_init();

	serial_init();
	serial_puts("\n[ aboot second-stage loader ]\n\n");

	if (usb_open(&usb))
		goto fail;

	usb_queue_read(&usb, &len, 4);
	usb_write(&usb, &MSG, 4);
	n = usb_wait_read(&usb);
	if (n)
		goto fail;

	if (usb_read(&usb, (void*) DOWNLOAD_ADDR, len))
		goto fail;

	usb_close(&usb);
	serial_puts("booting....\n");

	{
		void (*entry)(unsigned, unsigned, unsigned) = (void*) DOWNLOAD_ADDR;
		entry(0, 2791, 0x80000100);
		for (;;);
	}

fail:
	serial_puts("io error\n");
	for (;;) ;
}

