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

int match_omap4_bootloader(usb_ifc_info *ifc)
{
	if (ifc->dev_vendor != 0x0451)
		return -1;
	if ((ifc->dev_product != 0xd010) && (ifc->dev_product != 0xd00f))
		return -1;
	return 0;
}

int usb_boot(usb_handle *usb,
	     void *data, unsigned sz, 
	     void *data2, unsigned sz2)
{
	uint32_t msg_boot = 0xF0030002;
	uint32_t msg_getid = 0xF0030003;
	uint32_t msg_size = sz;
	uint8_t id[81];
	int i;

#define OFF_CHIP	0x04
#define OFF_ID		0x0F
#define OFF_MPKH	0x26
	memset(id, 0xee, 81);
	fprintf(stderr,"reading ASIC ID\n");
	usb_write(usb, &msg_getid, sizeof(msg_getid));
	usb_read(usb, id, sizeof(id));

	fprintf(stderr,"CHIP: %02x%02x\n", id[OFF_CHIP+0], id[OFF_CHIP+1]);
	fprintf(stderr,"IDEN: ");
	for (i = 0; i < 20; i++)
		fprintf(stderr,"%02x", id[OFF_ID+i]);
	fprintf(stderr,"\nMPKH: ");
	for (i = 0; i < 32; i++)
		fprintf(stderr,"%02x", id[OFF_MPKH+i]);
	fprintf(stderr,"\nCRC0: %02x%02x%02x%02x\n",
		id[73], id[74], id[75], id[76]);
	fprintf(stderr,"CRC1: %02x%02x%02x%02x\n",
		id[77], id[78], id[79], id[80]);

	fprintf(stderr,"sending 2ndstage to target... %08x\n",msg_boot);
	usb_write(usb, &msg_boot, sizeof(msg_boot));
	usb_write(usb, &msg_size, sizeof(msg_size));
	usb_write(usb, data, sz);
	
	
	if (data2) {
		// reopen the usb endpoint, to switch from talking
		// to the 1st stage to talk to the 2nd stage 
		usb_close(usb);
		fprintf(stderr,"waiting for 2ndstage response...\n");
		// sleeping for 2 seconds to let the 2nd stage prepare.
		// if you reopen too quickly then things break, stochastically.
		sleep(2);
		usb = usb_open(match_omap4_bootloader);
		usb_read(usb, &msg_size, sizeof(msg_size));
		// 0xaabbccdd is not any sort of message size.
		// msg_size was just a convenient piece of RAM available
		if (msg_size != 0xaabbccdd) {
			fprintf(stderr,"unexpected 2ndstage response\n");
			return -1;
		}
		msg_size = sz2;
		fprintf(stderr,"sending image to target...\n");
		usb_write(usb, &msg_size, sizeof(msg_size));
		usb_write(usb, data2, sz2);
	}
	
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

extern unsigned char aboot_data[];
extern unsigned aboot_size;

int main(int argc, char **argv)
{
	void *data, *data2;
	unsigned sz, sz2;
	usb_handle *usb;
	int once = 1;

	if (argc < 2) {
		fprintf(stderr,"usage: usbboot [ <2ndstage> ] <image>\n");
		return 0;
	}

	if (argc < 3) {
		fprintf(stderr,"using built-in 2ndstage.bin\n");
		data = aboot_data;
		sz = aboot_size;
	} else {
		data = load_file(argv[1], &sz);
		if (data == 0) {
			fprintf(stderr,"cannot load '%s'\n", argv[1]);
			return -1;
		}
		argc--;
		argv++;
	}
	
	data2 = load_file(argv[1], &sz2);
	if (data2 == 0) {
		fprintf(stderr,"cannot load '%s'\n", argv[1]);
		return -1;
	}

	for (;;) {
		usb = usb_open(match_omap4_bootloader);
		if (usb)
			return usb_boot(usb, data, sz, data2, sz2);
		if (once) {
			once = 0;
			fprintf(stderr,"waiting for OMAP44xx device...\n");
		}
		usleep(250);
	}
	
	return -1;    
}
