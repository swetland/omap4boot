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

#ifndef _ROM_USB_H_
#define _ROM_USB_H_

struct rom_handle {
	void *set_to_null;
	void (*callback)(struct rom_handle *rh);
	void *data;
	u32 length;
	u16 *options;
	u32 xfer_mode;
	u32 device_type;
	volatile u32 status;
	u16 hs_toc_mask;
	u16 gp_toc_mask;
};

struct usb {
	struct rom_handle dread;
	struct rom_handle dwrite;
	struct rom_driver *io;
};

int usb_open(struct usb *usb);
void usb_close(struct usb *usb);

void usb_queue_read(struct usb *usb, void *data, unsigned len);
int usb_wait_read(struct usb *usb);

void usb_queue_write(struct usb *usb, void *data, unsigned len);
int usb_wait_write(struct usb *usb);

int usb_read(struct usb *usb, void *data, unsigned len);
int usb_write(struct usb *usb, void *data, unsigned len);

#endif
