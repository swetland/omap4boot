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
#include <omap4/rom_usb.h>

#define DEVICE_NULL	0x40
#define DEVICE_UART1	0x41
#define DEVICE_UART2	0x42
#define DEVICE_UART3	0x43
#define DEVICE_UART4	0x44
#define DEVICE_USB	0x45
#define DEVICE_USBEXT	0x46

#define XFER_MODE_CPU 0
#define XFER_MODE_DMA 1

#define STATUS_OKAY		0
#define STATUS_FAILED		1
#define STATUS_TIMEOUT		2
#define STATUS_BAD_PARAM	3
#define STATUS_WAITING		4
#define STATUS_NO_MEMORY	5
#define STATUS_INVALID_PTR	6

struct rom_driver {
	int (*init)(struct rom_handle *rh);
	int (*read)(struct rom_handle *rh);
	int (*write)(struct rom_handle *rh);
	int (*close)(struct rom_handle *rh);
	int (*config)(struct rom_handle *rh, void *x);
};

#define API(n) ( (void*) (*((u32 *) (n))) )

int (*rom_get_driver)(struct rom_driver **io, u32 device_type);
int (*rom_get_device)(struct rom_handle **rh);

int usb_open(struct usb *usb)
{
	struct rom_handle *boot;
	int n;

	memset(usb, 0, sizeof(*usb));

	rom_get_driver = API(0x28408);
	rom_get_device = API(0x28484);

	n = rom_get_device(&boot);
	if (n)
		return n;

	if ((boot->device_type != DEVICE_USB) &&
	    (boot->device_type != DEVICE_USBEXT))
		return -1;

	n = rom_get_driver(&usb->io, boot->device_type);
	if (n)
		return n;

	usb->dread.xfer_mode = boot->xfer_mode;
	usb->dread.options = boot->options;
	usb->dread.device_type = boot->device_type;

	usb->dwrite.xfer_mode = boot->xfer_mode;
	usb->dwrite.options = boot->options;
	usb->dwrite.device_type = boot->device_type;

	return 0;
}

void usb_queue_read(struct usb *usb, void *data, unsigned len)
{
	int n;
	usb->dread.data = data;
	usb->dread.length = len;
	usb->dread.status = -1;
	n = usb->io->read(&usb->dread);
	if (n)
		usb->dread.status = n;
}

int usb_wait_read(struct usb *usb)
{
	for (;;) {
		if (usb->dread.status == -1)
			continue;
		if (usb->dread.status == STATUS_WAITING)
			continue;
		return usb->dread.status;
	}
}

void usb_queue_write(struct usb *usb, void *data, unsigned len)
{
	int n;
	usb->dwrite.data = data;
	usb->dwrite.length = len;
	usb->dwrite.status = -1;
	n = usb->io->write(&usb->dwrite);
	if (n)
		usb->dwrite.status = n;
}

int usb_wait_write(struct usb *usb)
{
	for (;;) {
		if (usb->dwrite.status == -1)
			continue;
		if (usb->dwrite.status == STATUS_WAITING)
			continue;
		return usb->dwrite.status;
	}
}

#define USB_MAX_IO 65536
int usb_read(struct usb *usb, void *data, unsigned len)
{
	unsigned xfer;
	unsigned char *x = data;
	int n;
	while (len > 0) {
		xfer = (len > USB_MAX_IO) ? USB_MAX_IO : len;
		usb_queue_read(usb, x, xfer);
		n = usb_wait_read(usb);
		if (n)
			return n;
		x += xfer;
		len -= xfer;
	}
	return 0;
}

int usb_write(struct usb *usb, void *data, unsigned len)
{
	usb_queue_write(usb, data, len);
	return usb_wait_write(usb);
}

void usb_close(struct usb *usb)
{
	usb->io->close(&usb->dread);
}
