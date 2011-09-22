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

#define WITH_MEMORY_TEST	0
#define WITH_FLASH_BOOT		0
#define WITH_SIGNATURE_CHECK	1

#if WITH_MEMORY_TEST
void memtest(void *x, unsigned count) {
	unsigned *w = x;
	unsigned n;
	count /= 4;

	printf("memtest write - %d\n",count);
	for (n = 0; n < count; n++) {
		unsigned chk = 0xa5a5a5a5 ^ n;
		w[n] = chk;
	}
	printf("memtest read\n");
	for (n = 0; n < count; n++) {
		unsigned chk = 0xa5a5a5a5 ^ n;
		if (w[n] != chk) {
			printf("ERROR @ %x (%x != %x)\n", 
				(unsigned) (w+n), w[n], chk);
			return;
		}
	}
	printf("OK!\n");
}
#endif

static unsigned MSG = 0xaabbccdd;

struct usb usb;

unsigned cfg_machine_type = 2791;

#if WITH_SIGNATURE_CHECK
unsigned call_trusted(unsigned appid, unsigned procid, unsigned flag, void *args);

int verify(void *data, unsigned len, void *signature, unsigned rights) {
	struct {
		unsigned count;
		void *data;
		unsigned len;
		void *signature;
		unsigned rights;
	} args;
	args.count = 4;
	args.data = data;
	args.len = len;
	args.signature = signature;
	args.rights = rights;
	return call_trusted(12, 0, 0, &args);
}
#endif

#if WITH_FLASH_BOOT
int load_image(unsigned device, unsigned start, unsigned count, void *data)
{
	int (*rom_get_mem_driver)(struct mem_driver **io, u32 type);
	struct mem_driver *io = 0;
	struct mem_device local_md_device, *md = 0;
	struct read_desc rd;
	u16 options;
	u32 base;
	int z;

	if (get_omap_rev() >= OMAP_4460_ES1_DOT_0)
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
#endif

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

	printf("MSV=%08x\n",*((unsigned*) 0x4A00213C));

#if WITH_MEMORY_TEST
	memtest(0x82000000, 8*1024*1024);
	memtest(0xA0208000, 8*1024*1024);
#endif

#if !WITH_FLASH_BOOT
	n = load_from_usb(&len);
#else
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
#endif

	if (n) {
		serial_puts("*** IO ERROR ***\n");
	} else {
#if WITH_SIGNATURE_CHECK
		void *data = (void*) (CONFIG_ADDR_DOWNLOAD);
		void *sign = (void*) (CONFIG_ADDR_DOWNLOAD + len - 280);
		if ((len < 281) || (len > (32*1024*1024)))
			goto fail_verify;
		len -= 280;

		n = verify(data, len, sign, 2);
		if (n != 0) {
		fail_verify:
			serial_puts("*** SIGNATURE VERIFICATION FAILED ***\n");
			for (;;) ;
		}
#endif
		boot_image(cfg_machine_type, CONFIG_ADDR_DOWNLOAD, len);
		serial_puts("*** BOOT FAILED ***\n");
	}

	for (;;) ;
}

