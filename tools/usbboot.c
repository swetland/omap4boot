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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>

#include "usb.h"

typedef struct tocentry {
	unsigned offset;
	unsigned length;
	unsigned flags;
	unsigned align;
	unsigned spare;
	char name[12];
} tocentry;

#define USE_TOC 0

int usb_boot(usb_handle *usb,
	     void *data, unsigned sz, 
	     void *data2, unsigned sz2)
{
	uint32_t msg_boot = 0xF0030002;
	uint32_t msg_getid = 0xF0030003;
	uint32_t msg_size = sz;
	unsigned char asic_id[1 + 7 + 4 + 23 + 35 + 11];
	tocentry TOC[32];
	unsigned test = 0xeeeeeeee;
	int n = 0;

#if USE_TOC
	TOC[0].offset = 0x200; //0x40;
	TOC[0].length = sz;
	TOC[0].flags = 0;
	TOC[0].align = 0;
	TOC[0].spare = 0;
	memcpy(TOC[0].name, "2ND", 3);
	memset(TOC + 1, 0xff, 32);
	msg_size += sizeof(TOC);
#endif

#if 0
	fprintf(stderr,"get asic id?\n");
	usb_write(usb, &msg_getid, sizeof(msg_getid));
	usb_read(usb, asic_id, sizeof(asic_id));
	fprintf(stderr,"%02x %02x %02x %02x %02x %02x %02x %02x \n",
		asic_id[0], asic_id[1], asic_id[2], asic_id[3], 
		asic_id[4], asic_id[5], asic_id[6], asic_id[7]);
	fprintf(stderr,"OMAP%02x%02x\n", asic_id[4], asic_id[5]);
#endif

	fprintf(stderr,"boot!\n");
	usb_write(usb, &msg_boot, sizeof(msg_boot));
	fprintf(stderr,"sz %d\n", msg_size);
	usb_write(usb, &msg_size, sizeof(msg_size));
#if USE_TOC
	usb_write(usb, TOC, sizeof(TOC));
#endif
	usb_write(usb, data, sz);

	if (data2) {
		usb_read(usb, &msg_size, sizeof(msg_size));
		if (msg_size != 0xaabbccdd) {
			fprintf(stderr,"unexpected response\n");
			return -1;
		}
		msg_size = sz2;
		usb_write(usb, &msg_size, sizeof(msg_size));
		usb_write(usb, data2, sz2);
	}
	
	return 0;
}

int match_omap4_bootloader(usb_ifc_info *ifc)
{
	if (ifc->dev_vendor != 0x0451)
		return -1;
	if (ifc->dev_product != 0xd00f)
		return -1;
	return 0;
}

void *load_file(const char *file, unsigned *sz)
{
	void *data;
	struct stat s;
	int fd;
	
	fd = open(file, O_RDONLY);
	if (fd < 0)
		return 0;
	
	if (fstat(fd, &s))
		goto fail;
	
	data = malloc(s.st_size);
	if (!data)
		goto fail;
	
	if (read(fd, data, s.st_size) != s.st_size) {
		free(data);
		goto fail;
	}
	
	close(fd);
	*sz = s.st_size;
	return data;
	
fail:
	close(fd);
	return 0;
}

int main(int argc, char **argv)
{
	void *data, *data2;
	unsigned sz, sz2;
	usb_handle *usb;
	
	if (argc < 2)
		return 0;

	data = load_file(argv[1], &sz);
	if (data == 0) {
		fprintf(stderr,"cannot load '%s'\n", argv[1]);
		return -1;
	}
	
	if (argc > 2) {
		data2 = load_file(argv[2], &sz2);
		if (data2 == 0) {
			fprintf(stderr,"cannot load '%s'\n", argv[2]);
			return -1;
		}
	} else {
		data2 = 0;
		sz2 = 0;
	}

	for (;;) {
		usb = usb_open(match_omap4_bootloader);
		if (usb)
			return usb_boot(usb, data, sz, data2, sz2);
		usleep(250);
	}
	
	return -1;    
}
