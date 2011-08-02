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
#include <omap4/omap4_rom.h>

static unsigned MSG = 0xaabbccdd;

struct usb usb;

unsigned cfg_machine_type = 2791;


int load_image(unsigned device, unsigned start, unsigned count, void *data)
{
	int (*rom_get_mem_driver)(struct mem_driver **io, u32 type);
	struct mem_driver *io = 0;
	struct mem_device local_md_device, *md = 0;
	struct read_desc rd;
	u16 options;
	u32 base;
	int z;

	if (get_omap_rev() >= OMAP4460_ES1_0)
		base = PUBLIC_API_BASE_4460;
	else
		base = PUBLIC_API_BASE_4430;

	rom_get_mem_driver = API(base + PUBLIC_GET_DRIVER_MEM_OFFSET);
	z = rom_get_mem_driver(&io, device);
	if (z)
		return -1;

	md = &local_md_device;
	memset(md, 0, sizeof(struct mem_device));
	options = 0; // 1 = init phoenix pmic?
	md->initialized   = 0;
	md->device_type   = device;
	md->xip_device    = 0;
	md->search_size   = 0;
	md->base_address  = 0;
	md->hs_toc_mask   = 0;
	md->gp_toc_mask   = 0;
	md->boot_options  = &options;
	md->device_data   = (void*) 0x80000000;
	memset(md->device_data, 0, 2500);

	z = io->init(md);
	if (z)
		return -1;

	rd.sector_start = start;
	rd.sector_count = count;
	rd.destination = data;
	z = io->read(md, &rd);

	return 0;
}

int load_from_mmc(unsigned device, unsigned *len)
{
	load_image(device, 512, 512, (void*) CONFIG_ADDR_DOWNLOAD);
	*len = 256 * 1024;
	return 0;
}

int load_from_usb(unsigned *_len)
{
	unsigned len, n;
	enable_irqs();

	if (usb_open(&usb))
		return -1;

	usb_queue_read(&usb, &len, 4);
	usb_write(&usb, &MSG, 4);
	n = usb_wait_read(&usb);
	if (n)
		return -1;

	if (usb_read(&usb, (void*) CONFIG_ADDR_DOWNLOAD, len))
		return -1;

	usb_close(&usb);

	disable_irqs();
	*_len = len;
	return 0;
}

void aboot(unsigned *info)
{
	unsigned bootdevice, n, len;

	board_mux_init();
	sdelay(100);

	scale_vcores();

	prcm_init();
	board_ddr_init();
	gpmc_init();

	board_late_init();

	serial_init();
	serial_puts("\n[ aboot second-stage loader ]\n\n");

	if (info) {
		bootdevice = info[2] & 0xFF;
	} else {
		bootdevice = 0x45;
	}

	switch (bootdevice) {
	case 0x45: /* USB */
		serial_puts("boot device: USB\n\n");
		n = load_from_usb(&len);
		break;
	case 0x05:
	case 0x06:
		serial_puts("boot device: MMC\n\n");
		n = load_from_mmc(bootdevice, &len);
		break;
	default:
		serial_puts("boot device: unknown\n");
		for (;;) ;
	}

	if (n) {
		serial_puts("io error\n");
	} else {
		boot_image(cfg_machine_type, CONFIG_ADDR_DOWNLOAD, len);
		serial_puts("invalid image\n");
	}
}

