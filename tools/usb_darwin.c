/*
 * Copyright (C) 2008 The Android Open Source Project
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

/*
 * This file is based on modified work from AOSP.
 *
 * Author: Christopher Friedt <chrisfriedt@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOCFPlugIn.h>
#include <IOKit/usb/IOUSBLib.h>
#include <IOKit/IOMessage.h>
#include <mach/mach_port.h>

#include "usb.h"

#if 0
#define D(fmt,args...) printf("W/" fmt, ##args)
#define W(fmt,args...) printf("W/" fmt, ##args)
#define E(fmt,args...) printf("E/" fmt, ##args)
#else
#define D(fmt,args...)
#define W(fmt,args...)
#define E(fmt,args...)
#endif

/** An open usb device */
struct usb_handle
{
    int success;
    ifc_match_func callback;
    usb_ifc_info info;

    UInt8 bulkIn;
    UInt8 bulkOut;
    IOUSBInterfaceInterface182 **interface;

    UInt16 maxPacketSize[2];

    unsigned int zero_mask;
};

usb_handle *usb_open(ifc_match_func callback)
{
	usb_handle *hp = NULL;
	usb_handle h;

	CFMutableDictionaryRef matchingDict;
	kern_return_t kr;
	io_iterator_t i1, i2;
	io_service_t sysdev;
	IOCFPlugInInterface **plugdev = NULL;
	IOUSBDeviceInterface **usbdev = NULL;
	SInt32 score;
	IOUSBFindInterfaceRequest request;
	io_service_t sysifc;
	IOCFPlugInInterface **plugifc = NULL;
	IOUSBInterfaceInterface182 **usbifc = NULL;
	UInt8 interfaceNumEndpoints;
	UInt8 endpoint;
	UInt8 configuration;
	UInt8   transferType;
	UInt16  maxPacketSize;
	UInt8   interval;
	UInt8   number;
	UInt8   direction;

	h.success = 0;
	h.callback = callback;

	matchingDict = IOServiceMatching(kIOUSBDeviceClassName);
	if (matchingDict == NULL) {
		E("Couldn't create USB matching dictionary.\n");
		return NULL;
	}
	kr = IOServiceGetMatchingServices(
			kIOMasterPortDefault, matchingDict, &i1);
	if (kr) {
		E("Could not create iterator.");
		return NULL;
	}

	for (;;) {

		if (! IOIteratorIsValid(i1)) {
			IOIteratorReset(i1);
			continue;
		}

		sysdev = IOIteratorNext(i1);
		if (!sysdev)
			break;

		kr = IOCreatePlugInInterfaceForService(sysdev, kIOUSBDeviceUserClientTypeID, kIOCFPlugInInterfaceID, &plugdev, &score);
		IOObjectRelease(sysdev); sysdev = 0;
		if ( kr || !plugdev ) {
			E("Couldn't create plug-in device (%08x)\n", kr);
			continue;
		}

		kr = (*plugdev)->QueryInterface(plugdev,CFUUIDGetUUIDBytes(kIOUSBDeviceInterfaceID), (LPVOID) &usbdev);
		IODestroyPlugInInterface(plugdev);
		if ( kr || !usbdev ) {
			E("Couldn't create a device interface (%08x)\n", kr);
			continue;
		}

		if (
			(kr = (*usbdev)->GetDeviceVendor(usbdev, &h.info.dev_vendor))     ||
			(kr = (*usbdev)->GetDeviceProduct(usbdev, &h.info.dev_product))   ||
			(kr = (*usbdev)->GetDeviceClass(usbdev, &h.info.dev_class))       ||
			(kr = (*usbdev)->GetDeviceSubClass(usbdev, &h.info.dev_subclass)) ||
			(kr = (*usbdev)->GetDeviceProtocol(usbdev, &h.info.dev_protocol))
		) {
			E("failed to get usb device info (%08x)\n", kr);
			(*usbdev)->Release(usbdev); usbdev = 0;
			continue;
		}
		h.info.writable = 1;

		D("%04x:%04x\n", h.info.dev_vendor, h.info.dev_product);

		kr = (*usbdev)->USBDeviceOpen(usbdev);
	    if ( kr ) {
	    	(*usbdev)->Release(usbdev);
	    	D("Failed to open usb device (%08x)\n", kr );
	    	continue;
	    }

	    configuration = 0;
	    kr = (*usbdev)->GetConfiguration(usbdev, &configuration);
	    if ( kr ) {
	    	(*usbdev)->Release(usbdev);
	    	D("Failed to get configuration (%08x)\n", kr );
	    	continue;
	    }
	    if (configuration != 1) {
	        kr = (*usbdev)->SetConfiguration(usbdev, 1);
	        if ( kr ) {
		    	(*usbdev)->Release(usbdev);
		    	D("Failed to set configuration (%08x)\n", kr );
		    	continue;
	        }
	    }

	    request.bInterfaceClass = kIOUSBFindInterfaceDontCare;
	    request.bInterfaceSubClass = kIOUSBFindInterfaceDontCare;
	    request.bInterfaceProtocol = kIOUSBFindInterfaceDontCare;
	    request.bAlternateSetting = kIOUSBFindInterfaceDontCare;

	    kr = (*usbdev)->CreateInterfaceIterator(usbdev, &request, &i2);
	    if ( kr ) {
	    	E("failed to create interface iterator (%08x)\n", kr);
	    	continue;
	    }

		D("%04x:%04x Searching for USB interfaces\n", h.info.dev_vendor, h.info.dev_product);

		if (! IOIteratorIsValid(i2)) {
			D("resetting iterator\n");
			IOIteratorReset(i2);
			continue;
		}

		int i;
		for( i=1, sysifc = IOIteratorNext(i2); ; i++, sysifc = IOIteratorNext(i2) ) {

			if ( !sysifc )
				break;

			kr = IOCreatePlugInInterfaceForService(sysifc, kIOUSBInterfaceUserClientTypeID, kIOCFPlugInInterfaceID, &plugifc, &score);
			IOObjectRelease(sysifc); sysifc = 0;
			if ( kr ) {
				W("Couldn't create plugin interface (%08x)\n"kr);
				continue;
			}

			kr = (*plugifc)->QueryInterface(plugifc,CFUUIDGetUUIDBytes(kIOUSBInterfaceInterfaceID),(LPVOID) &usbifc);
			(*plugifc)->Release(plugifc); plugifc = 0;
			if ( kr ) {
				E("Couldn't create usb interface: (%08x)\n", kr);
				continue;
			}

			if (
				(kr = (*usbifc)->USBInterfaceOpen(usbifc)) ||
				(kr = (*usbifc)->GetNumEndpoints(usbifc,&interfaceNumEndpoints)) ||
				(kr = (*usbifc)->GetInterfaceClass(usbifc,&h.info.ifc_class)) ||
				(kr = (*usbifc)->GetInterfaceSubClass(usbifc,&h.info.ifc_subclass)) ||
				(kr = (*usbifc)->GetInterfaceProtocol(usbifc,&h.info.ifc_protocol))
			) {
				(*usbifc)->Release(usbifc); usbifc = 0;
				D("Couldn't open usb interface: (%08x)\n", kr);
				continue;
			}

			D("%04x:%04x-%u Searching for USB Bulk endpoints\n", h.info.dev_vendor, h.info.dev_product, i);

			h.info.has_bulk_in = 0;
			h.info.has_bulk_out = 0;

			for (endpoint = 0; endpoint <= interfaceNumEndpoints; endpoint++) {
				kr = (*usbifc)->GetPipeProperties(usbifc, endpoint, &direction, &number, &transferType, &maxPacketSize, &interval);
				if (kr) {
					E("Failed to get pipe properties (%08x)\n", kr);
					continue;
				}

				if (transferType != kUSBBulk)
					continue;

				if (direction == kUSBIn) {
					D("%04x:%04x-%u-%u is a bulk in endpoint\n", h.info.dev_vendor, h.info.dev_product, i, endpoint);
					h.info.has_bulk_in = 1;
					h.bulkIn = endpoint;
					h.maxPacketSize[0] = maxPacketSize;
				} else if (direction == kUSBOut) {
					D("%04x:%04x-%u-%u is a bulk out endpoint\n", h.info.dev_vendor, h.info.dev_product, i, endpoint);
					h.info.has_bulk_out = 1;
					h.bulkOut = endpoint;
					h.maxPacketSize[1] = maxPacketSize;
				}

				if (
					h.info.has_bulk_in &&
					h.info.has_bulk_out &&
					!callback(&h.info)
				) {
					h.interface = usbifc;
					h.success = 1;
					break;
				}
			}

			if ( !h.success ) {
				(*usbifc)->Release(usbifc); usbifc = 0;
			} else {
				break;
			}
		}
		IOObjectRelease(i2); i2 = 0;
		IOObjectRelease(sysdev); sysdev = 0;

		if (h.success) {
			hp = malloc(sizeof(usb_handle));
			memcpy(hp, &h, sizeof(usb_handle));
			break;
		}
	}
	IOObjectRelease(i1); i1 = 0;

	return hp;
}

int usb_read(usb_handle *h, void *data, int len) {
    IOReturn result;
    UInt32 numBytes = len;

    if ( len <= 0 )
        return 0;

    if ( ! (h && h->interface && h->bulkIn) ) {
    	E("usb handle, interface, or bulkIn is invalid\n");
        return -1;
    }

    result = (*h->interface)->ReadPipe(
            h->interface, h->bulkIn, data, &numBytes);

    if ( result ) {
    	E("ReadPipe failed (%08x)\n", result);
    	return -1;
    }

    return len;
}

int usb_write(usb_handle *h, const void *data, int len) {
    IOReturn result;
    int numBytes, _len;

    if ( len <= 0 )
        return 0;

    if (! ( h && h->interface && h->bulkOut) ) {
    	E("usb handle, interface, or bulkOut is invalid\n");
        return -1;
    }

    for( _len = len; _len; _len -= numBytes, data += numBytes ) {

		numBytes = _len <= h->maxPacketSize[1] ? _len : h->maxPacketSize[1];

		result = (*h->interface)->WritePipe(
				h->interface, h->bulkOut, (void *)data, numBytes);

		if ( result ) {
			E("WritePipe failed (%08x)\n", result);
			return -1;
		}
    }

    return len;
}
