/*
 * Copyright (C) 2011 The Android Open Source Project
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

/* omap44xx ID detection and reporting */

#include <aboot/aboot.h>
#include <aboot/io.h>

#define CONTROL_ID_CODE 0x4A002204

unsigned int cortex_a9_rev(void)
{
	unsigned int i;
	/* turn off I/D-cache */
	asm ("mrc p15, 0, %0, c0, c0, 0" : "=r" (i));
	return i;
}

int get_omap_rev(void)
{
	/*
	 * For some of the ES2/ES1 boards ID_CODE is not reliable:
	 * Also, ES1 and ES2 have different ARM revisions
	 * So use ARM revision for identification
	 */
	unsigned int rev = cortex_a9_rev();

	switch (rev) {
		case MIDR_CORTEX_A9_R0P1:
			return OMAP4430_ES1_0;
		case MIDR_CORTEX_A9_R1P2:
			rev = readl(CONTROL_ID_CODE);
			switch (rev) {
				case OMAP4_CONTROL_ID_CODE_ES2_2:
					return OMAP4430_ES2_2;
				case OMAP4_CONTROL_ID_CODE_ES2_1:
					return OMAP4430_ES2_1;
				case OMAP4_CONTROL_ID_CODE_ES2_0:
					return OMAP4430_ES2_0;
				default:
					return OMAP4430_ES2_0;
			}
		case MIDR_CORTEX_A9_R1P3:
			return OMAP4430_ES2_3;
		case MIDR_CORTEX_A9_R2P10:
			return OMAP4460_ES1_0;
		default:
			return OMAP4430_SILICON_ID_INVALID;
	}

}
