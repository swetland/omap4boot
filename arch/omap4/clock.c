/*
 * clock.c
 *
 * Copyright(c) 2010 Texas Instruments.   All rights reserved.
 *
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
 * Rajendra Nayak <rnayak@ti.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name Texas Instruments nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <aboot/aboot.h>
#include <aboot/io.h>
#include <omap4/hw.h>
#include <omap4/clocks.h>
#include <omap4/bits.h>

#define CONFIG_OMAP4_SDC 1

/* Tables having M,N,M2 et al values for different sys_clk speeds
 * This table is generated only for OPP100
 * The tables are organized as follows:
 * Rows : 1 - 12M, 2 - 13M, 3 - 16.8M, 4 - 19.2M, 5 - 26M, 6 - 27M, 7 - 38.4M
 */

/* MPU parameters */
struct dpll_param mpu_dpll_param_1008mhz[7] = {
	/* 12M values */
	{0x54, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x3f0, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x3c, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x69, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x1f8, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x70, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
	/* RUN MPU @ 1008 MHz */
	{0x69, 0x3, 0x1, 0x00, 0x00, 0x00, 0x00, 0x00},
};

struct dpll_param mpu_dpll_param_920mhz[7] = {
        /* 12M values */
        {0xe6, 0x2, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 13M values */
        {0x398, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 16.8M values */
        {0xdb, 0x3, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 19.2M values */
        {0x23f, 0xb, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 26M values */
        {0x1cc, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 27M values */
        {0x398, 0x1a, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 38.4M values */
        /* RUN MPU @ 920 MHz */
	{0x23f, 0x17, 0x1, 0x00, 0x00, 0x00, 0x00, 0x00},
};

struct dpll_param mpu_dpll_param_600mhz[7] = {
        /* 12M values */
        {0x32, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 13M values */
        {0x258, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 16.8M values */
        {0xfa, 0x6, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 19.2M values */
        {0x7d, 0x3, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 26M values */
        {0x12c, 0xc, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 27M values */
        {0xc8, 0x8, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 38.4M values */
        /* RUN MPU @ 1.2 GHz */
        {0x7d, 0x07, 0x1, 0x00, 0x00, 0x00, 0x00, 0x00},
};

struct dpll_param mpu_dpll_param_1_5ghz[7] = {
        /* 12M values */
        {0x7d, 0x1, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 13M values */
        {0x2ee, 0xc, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 16.8M values */
        {0x271, 0xd, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 19.2M values */
        {0x271, 0xf, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 26M values */
        {0x177, 0xc, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 27M values */
        {0xfa, 0x8, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
        /* 38.4M values */
        /* RUN MPU @ 1.5 GHz */
        {0x271, 0x1f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00},
};

/* PER parameters */
const struct dpll_param per_dpll_param[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
	{0x14, 0x00, 0x08, 0x06, 0x0c, 0x09, 0x04, 0x05},
};

/* IVA parameters */
struct dpll_param iva_dpll_param[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
#ifdef CONFIG_OMAP4_SDC
	{0x61, 0x03, 0x00, 0x00, 0x04, 0x07, 0x00, 0x00},
#else
	{0x61, 0x03, 0x00, 0x00, 0x04, 0x07, 0x00, 0x00},
#endif
};

/* CORE parameters */
struct dpll_param core_dpll_param[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values - DDR@200MHz*/
	{0x7d, 0x05, 0x02, 0x05, 0x08, 0x04, 0x06, 0x05},
};

/* CORE parameters */
struct dpll_param core_dpll_param_ddr400[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values - DDR@400MHz*/
	{0x7d, 0x05, 0x01, 0x05, 0x08, 0x04, 0x06, 0x05},
};

/* CORE parameters for L3 at 190 MHz - For ES1 only*/
struct dpll_param core_dpll_param_l3_190[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
#ifdef CORE_190MHZ 
	{0x1f0, 0x18, 0x01, 0x05, 0x08, 0x04, 0x06, 0x05},
#else /* Default CORE @166MHz */
	{0x1b0, 0x18, 0x01, 0x05, 0x08, 0x04, 0x06, 0x05},
#endif
};

/*
 * ABE parameters
 *
 * By default DPLL_ABE is driven from 32KHz clock.  To drive it from SYS_CK
 * set CONFIG_OMAP4_ABE_SYSCK
 */
struct dpll_param abe_dpll_param[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
#ifdef CONFIG_OMAP4_ABE_SYSCK
	{0x40, 0x18, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0},
#else
	{0x2ee, 0x0, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0},
#endif
};

/* USB parameters */
struct dpll_param usb_dpll_param[7] = {
	/* 12M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 13M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 16.8M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 19.2M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 26M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 27M values */
	{0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0},
	/* 38.4M values */
#ifdef CONFIG_OMAP4_SDC
	{0x32, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4},
#else
	{0x32, 0x1, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4},
#endif
};
/* PER dpll params defined in board file */
extern const struct dpll_param per_dpll_param[7];

typedef struct dpll_param dpll_param;

static void configure_mpu_dpll(u32 clk_index)
{
	dpll_param *dpll_param_p;
	u32 omap4_rev, emif_div_4 = 0, abe_div_8 = 0, dcc_en = 0;

	omap4_rev = get_omap_rev();

	/* Unlock the MPU dpll */
	sr32(CM_CLKMODE_DPLL_MPU, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_MPU, LDELAY);

	if (omap4_rev >= OMAP4460_ES1_0) {

		switch (omap4_silicon_type()) {
#if 0
			case PROD_ID_1_SILICON_TYPE_STD_PERF:
				/*
				 * Same M, N as for 600 MHz from M2 output will
				 * give 1200 MHz from M3 output
				 */
				dpll_param_p = &mpu_dpll_param_600mhz[clk_index];
				dcc_en = 1;
				emif_div_4 = 1;
				abe_div_8 = 1;
				break;
			case PROD_ID_1_SILICON_TYPE_HIGH_PERF:
				dpll_param_p = &mpu_dpll_param_1_5ghz[clk_index];
				dcc_en = 1;
				emif_div_4 = 1;
				abe_div_8 = 1;
				break;
#endif
			default:
				dpll_param_p = &mpu_dpll_param_920mhz[clk_index];
				emif_div_4 = 0;
				abe_div_8 = 1;
				break;
		}

		sr32(CM_MPU_MPU_CLKCTRL, 24, 1, emif_div_4);
		sr32(CM_MPU_MPU_CLKCTRL, 25, 1, abe_div_8);

		/* Enable / disable DCC on 4460 */
		sr32(CM_CLKSEL_DPLL_MPU, 22, 1, dcc_en);

	} else if (omap4_rev > OMAP4430_ES1_0) {
		dpll_param_p = &mpu_dpll_param_1008mhz[clk_index];
	} else {
		dpll_param_p = &mpu_dpll_param_600mhz[clk_index];
	}

	sr32(CM_AUTOIDLE_DPLL_MPU, 0, 3, 0x0); /* Disable DPLL autoidle */

	/* Set M,N,M2 values */
	sr32(CM_CLKSEL_DPLL_MPU, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_MPU, 0, 6, dpll_param_p->n);
	sr32(CM_DIV_M2_DPLL_MPU, 0, 5, dpll_param_p->m2);
	sr32(CM_DIV_M2_DPLL_MPU, 8, 1, 0x1);

	/* Lock the mpu dpll */
	sr32(CM_CLKMODE_DPLL_MPU, 0, 3, PLL_LOCK | 0x10);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_MPU, LDELAY);

	return;
}

static void configure_iva_dpll(u32 clk_index)
{
	dpll_param *dpll_param_p;

	/* Unlock the IVA dpll */
	sr32(CM_CLKMODE_DPLL_IVA, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_IVA, LDELAY);

	/* CM_BYPCLK_DPLL_IVA = CORE_X2_CLK/2 */
	sr32(CM_BYPCLK_DPLL_IVA, 0, 2, 0x1);

	/* Program IVA DPLL */
	dpll_param_p = &iva_dpll_param[clk_index];

	sr32(CM_AUTOIDLE_DPLL_IVA, 0, 3, 0x0); /* Disable DPLL autoidle */

	/* Set M,N,M4,M5 */
	sr32(CM_CLKSEL_DPLL_IVA, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_IVA, 0, 7, dpll_param_p->n);
	sr32(CM_DIV_M4_DPLL_IVA, 0, 5, dpll_param_p->m4);
	sr32(CM_DIV_M4_DPLL_IVA, 8, 1, 0x1);
	sr32(CM_DIV_M5_DPLL_IVA, 0, 5, dpll_param_p->m5);
	sr32(CM_DIV_M5_DPLL_IVA, 8, 1, 0x1);

	/* Lock the iva dpll */
	sr32(CM_CLKMODE_DPLL_IVA, 0, 3, PLL_LOCK);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_IVA, LDELAY);

	return;
}

static void configure_per_dpll(u32 clk_index)
{
	const dpll_param *dpll_param_p;

	/* Unlock the PER dpll */
	sr32(CM_CLKMODE_DPLL_PER, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_PER, LDELAY);

	/* Program PER DPLL */
	dpll_param_p = &per_dpll_param[clk_index];

	/* Disable autoidle */
	sr32(CM_AUTOIDLE_DPLL_PER, 0, 3, 0x0);

	sr32(CM_CLKSEL_DPLL_PER, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_PER, 0, 6, dpll_param_p->n);
	sr32(CM_DIV_M2_DPLL_PER, 0, 5, dpll_param_p->m2);
	sr32(CM_DIV_M2_DPLL_PER, 8, 1, 0x1);
	sr32(CM_DIV_M3_DPLL_PER, 0, 5, dpll_param_p->m3);
	sr32(CM_DIV_M3_DPLL_PER, 8, 1, 0x1);
	sr32(CM_DIV_M4_DPLL_PER, 0, 5, dpll_param_p->m4);
	sr32(CM_DIV_M4_DPLL_PER, 8, 1, 0x1);
	sr32(CM_DIV_M5_DPLL_PER, 0, 5, dpll_param_p->m5);
	sr32(CM_DIV_M5_DPLL_PER, 8, 1, 0x1);
	sr32(CM_DIV_M6_DPLL_PER, 0, 5, dpll_param_p->m6);
	sr32(CM_DIV_M6_DPLL_PER, 8, 1, 0x1);
	sr32(CM_DIV_M7_DPLL_PER, 0, 5, dpll_param_p->m7);
	sr32(CM_DIV_M7_DPLL_PER, 8, 1, 0x1);

	/* Lock the per dpll */
	sr32(CM_CLKMODE_DPLL_PER, 0, 3, PLL_LOCK);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_PER, LDELAY);

	return;
}

/* DPLL_ABE is driven by 32KHz timer by default */
static void configure_abe_dpll(u32 clk_index)
{
	dpll_param *dpll_param_p;
	u32 clkmode_value;

#ifdef CONFIG_OMAP4_ABE_SYSCK
	sr32(CM_ABE_PLL_REF_CLKSEL, 0, 32, 0x0);
#endif

	/* Unlock the ABE dpll */
	sr32(CM_CLKMODE_DPLL_ABE, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_ABE, LDELAY);

	/* Program ABE DPLL */
	dpll_param_p = &abe_dpll_param[clk_index];

	/* Disable autoidle */
	sr32(CM_AUTOIDLE_DPLL_ABE, 0, 3, 0x0);

	/*
	 * Enable higher frequencies when fed from 32KHz clk.  Sets
	 * DPLL_REGM4XEN, DPLL_LPMODE, DPLL_RELOCK_RAMP_EN, DPLL_RAMP_RATE and
	 * DPLL_DRIFTGUARD_EN in the CM_CLKMODE_DPLL_ABE register.
	 *
	 * Public TRM does not cover all of this: DPLL_RAMP_RATE (bit 5)
	 * selects time spent at each stage of clock ramping process.  We
	 * spend 4 REFCLKs.  DPLL_RELOCK_RAMP_EN (bit 9) enables the clock
	 * ramping feature, using the rate specified in DPLL_RAMP_RATE.
	 *
	 * Also the DPLL_REGM4XEN bit provides a magic 4x multplier to
	 * existing MN dividers.  This is how a DPLL driven from 32KHz clock
	 * can achieve 196.608MHz.
	 */
#ifndef CONFIG_OMAP4_ABE_SYSCK
	clkmode_value = (BIT5 | BIT8 | BIT9 | BIT10 | BIT11);
	sr32(CM_CLKMODE_DPLL_ABE, 0, 12, clkmode_value);
#endif

	sr32(CM_CLKSEL_DPLL_ABE, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_ABE, 0, 7, dpll_param_p->n);

	/* Force DPLL CLKOUTHIF to stay enabled */
	sr32(CM_DIV_M2_DPLL_ABE, 0, 32, 0x500);
	sr32(CM_DIV_M2_DPLL_ABE, 0, 5, dpll_param_p->m2);
	sr32(CM_DIV_M2_DPLL_ABE, 8, 1, 0x1);
	/* Force DPLL CLKOUTHIF to stay enabled */
	sr32(CM_DIV_M3_DPLL_ABE, 0, 32, 0x100);
	sr32(CM_DIV_M3_DPLL_ABE, 0, 5, dpll_param_p->m3);
	sr32(CM_DIV_M3_DPLL_ABE, 8, 1, 0x1);

	/* Lock the abe dpll */
	sr32(CM_CLKMODE_DPLL_ABE, 0, 3, PLL_LOCK);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_ABE, LDELAY);

	return;
}

static void configure_usb_dpll(u32 clk_index)
{
	dpll_param *dpll_param_p;

	/* Select the 60Mhz clock 480/8 = 60*/
	sr32(CM_CLKSEL_USB_60MHz, 0, 32, 0x1);

	/* Unlock the USB dpll */
	sr32(CM_CLKMODE_DPLL_USB, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_USB, LDELAY);

	/* Program USB DPLL */
	dpll_param_p = &usb_dpll_param[clk_index];

	/* Disable autoidle */
	sr32(CM_AUTOIDLE_DPLL_USB, 0, 3, 0x0);

	sr32(CM_CLKSEL_DPLL_USB, 24, 8, dpll_param_p->sd_div);
	sr32(CM_CLKSEL_DPLL_USB, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_USB, 0, 6, dpll_param_p->n);

	/* Force DPLL CLKOUT to stay active */
	sr32(CM_DIV_M2_DPLL_USB, 0, 32, 0x100);
	sr32(CM_DIV_M2_DPLL_USB, 0, 5, dpll_param_p->m2);
	sr32(CM_DIV_M2_DPLL_USB, 8, 1, 0x1);
	sr32(CM_CLKDCOLDO_DPLL_USB, 8, 1, 0x1);

	/* Lock the usb dpll */
	sr32(CM_CLKMODE_DPLL_USB, 0, 3, PLL_LOCK);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_USB, LDELAY);

	/* force enable the CLKDCOLDO clock */
	sr32(CM_CLKDCOLDO_DPLL_USB, 0, 32, 0x100);

	return;
}

void configure_core_dpll_no_lock(void)
{
	dpll_param *dpll_param_p = 0;
	u32 clk_index;

	/* Get the sysclk speed from cm_sys_clksel
	 * Set it to 38.4 MHz, in case ROM code is bypassed
	 */
	writel(0x7,CM_SYS_CLKSEL);
	clk_index = 7;

	clk_index = clk_index - 1;
	/* CORE_CLK=CORE_X2_CLK/2, L3_CLK=CORE_CLK/2, L4_CLK=L3_CLK/2 */
	sr32(CM_CLKSEL_CORE, 0, 32, 0x110);

	/* Unlock the CORE dpll */
	sr32(CM_CLKMODE_DPLL_CORE, 0, 3, PLL_MN_POWER_BYPASS);
	wait_on_value(BIT0, 0, CM_IDLEST_DPLL_CORE, LDELAY);

	/* Program Core DPLL */
	if(get_omap_rev() == OMAP4430_ES1_0)
		dpll_param_p = &core_dpll_param_l3_190[clk_index];
	else if (get_omap_rev() == OMAP4430_ES2_0)
		dpll_param_p = &core_dpll_param[clk_index];
	else if (get_omap_rev() >= OMAP4430_ES2_1)
		dpll_param_p = &core_dpll_param_ddr400[clk_index];

	/* Disable autoidle */
	sr32(CM_AUTOIDLE_DPLL_CORE, 0, 3, 0x0);

	sr32(CM_CLKSEL_DPLL_CORE, 8, 11, dpll_param_p->m);
	sr32(CM_CLKSEL_DPLL_CORE, 0, 6, dpll_param_p->n);
	sr32(CM_DIV_M2_DPLL_CORE, 0, 5, dpll_param_p->m2);
	sr32(CM_DIV_M3_DPLL_CORE, 0, 5, dpll_param_p->m3);
	sr32(CM_DIV_M4_DPLL_CORE, 0, 5, dpll_param_p->m4);
	sr32(CM_DIV_M5_DPLL_CORE, 0, 5, dpll_param_p->m5);
	sr32(CM_DIV_M6_DPLL_CORE, 0, 5, dpll_param_p->m6);
	sr32(CM_DIV_M7_DPLL_CORE, 0, 5, dpll_param_p->m7);

	if(get_omap_rev() == OMAP4430_ES1_0)
	{
		/* Do this only on ES1.0 */
		sr32(CM_DIV_M2_DPLL_CORE, 8, 1, 0x1);
		sr32(CM_DIV_M3_DPLL_CORE, 8, 1, 0x1);
		sr32(CM_DIV_M4_DPLL_CORE, 8, 1, 0x1);
		sr32(CM_DIV_M5_DPLL_CORE, 8, 1, 0x1);
		sr32(CM_DIV_M6_DPLL_CORE, 8, 1, 0x1);
		sr32(CM_DIV_M7_DPLL_CORE, 8, 1, 0x1);
	}

	return;
}

void lock_core_dpll(void)
{
	/* Lock the core dpll */
	sr32(CM_CLKMODE_DPLL_CORE, 0, 3, PLL_LOCK);
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_CORE, LDELAY);

	return;
}

void lock_core_dpll_shadow(void)
{
	dpll_param *dpll_param_p = 0;
	u32 clk_index;
	u32 temp;
	temp = readl(CM_MEMIF_CLKSTCTRL);
	temp &= (~3);
	temp |= 2;
	writel(temp, CM_MEMIF_CLKSTCTRL);

	while(readl(CM_MEMIF_EMIF_1_CLKCTRL) & 0x30000)
		;

	while(readl(CM_MEMIF_EMIF_2_CLKCTRL) & 0x30000)
		;

	/* Lock the core dpll using freq update method */
	/*(CM_CLKMODE_DPLL_CORE) */
	writel(0x0A, 0x4A004120);

	clk_index = 6;

	if(get_omap_rev() == OMAP4430_ES1_0)
		dpll_param_p = &core_dpll_param_l3_190[clk_index];
	else if (get_omap_rev() == OMAP4430_ES2_0)
		dpll_param_p = &core_dpll_param[clk_index];
	else if (get_omap_rev() >= OMAP4430_ES2_1)
		dpll_param_p = &core_dpll_param_ddr400[clk_index];

	/* CM_SHADOW_FREQ_CONFIG1: DLL_OVERRIDE = 1(hack), DLL_RESET = 1,
	 * DPLL_CORE_M2_DIV =1, DPLL_CORE_DPLL_EN = 0x7, FREQ_UPDATE = 1
	 */
	writel(0x70D | (dpll_param_p->m2 << 11), 0x4A004260);

	/* Wait for Freq_Update to get cleared: CM_SHADOW_FREQ_CONFIG1 */
	while((readl(0x4A004260) & 0x1) == 0x1)
		;

	/* Wait for DPLL to Lock : CM_IDLEST_DPLL_CORE */
	wait_on_value(BIT0, 1, CM_IDLEST_DPLL_CORE, LDELAY);
	//lock_core_dpll();

	while(readl(CM_MEMIF_EMIF_1_CLKCTRL) & 0x30000)
		;

	while(readl(CM_MEMIF_EMIF_2_CLKCTRL) & 0x30000)
		;

	writel(temp|3, CM_MEMIF_CLKSTCTRL);
	return;
}

static void enable_all_clocks(void)
{
	if (get_omap_rev() == OMAP4430_ES1_0) {
		/* Enable Ducati clocks */
		sr32(CM_DUCATI_DUCATI_CLKCTRL, 0, 32, 0x1);
		sr32(CM_DUCATI_CLKSTCTRL, 0, 32, 0x2);

		wait_on_value(BIT8, BIT8, CM_DUCATI_CLKSTCTRL, LDELAY);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_DUCATI_DUCATI_CLKCTRL, LDELAY);

		/* Enable ivahd and sl2 clocks */
		sr32(IVAHD_IVAHD_CLKCTRL, 0, 32, 0x1);
		sr32(IVAHD_SL2_CLKCTRL, 0, 32, 0x1);
		sr32(IVAHD_CLKSTCTRL, 0, 32, 0x2);

		wait_on_value(BIT8, BIT8, IVAHD_CLKSTCTRL, LDELAY);

		/* wait for ivahd to become accessible */
		//wait_on_value(BIT18|BIT17|BIT16, 0, IVAHD_IVAHD_CLKCTRL, LDELAY);
		/* wait for sl2 to become accessible */
		//wait_on_value(BIT17|BIT16, 0, IVAHD_SL2_CLKCTRL, LDELAY);

		/* Enable Tesla clocks */
		sr32(DSP_DSP_CLKCTRL, 0, 32, 0x1);
		sr32(DSP_CLKSTCTRL, 0, 32, 0x2);

		wait_on_value(BIT8, BIT8, DSP_CLKSTCTRL, LDELAY);

		/* wait for tesla to become accessible */
		//wait_on_value(BIT18|BIT17|BIT16, 0, DSP_DSP_CLKCTRL, LDELAY);

		/* TODO: Some hack needed by MM: Clean this */

		/* ABE clocks */
		sr32(CM1_ABE_CLKSTCTRL, 0, 32, 0x3);
		sr32(CM1_ABE_AESS_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM1_ABE_AESS_CLKCTRL, LDELAY);
		sr32(CM1_ABE_PDM_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_PDM_CLKCTRL, LDELAY);
		sr32(CM1_ABE_DMIC_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_DMIC_CLKCTRL, LDELAY);
		sr32(CM1_ABE_MCASP_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_MCASP_CLKCTRL, LDELAY);
		sr32(CM1_ABE_MCBSP1_CLKCTRL, 0, 32, 0x08000002);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_MCBSP1_CLKCTRL, LDELAY);
		sr32(CM1_ABE_MCBSP2_CLKCTRL, 0, 32, 0x08000002);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_MCBSP2_CLKCTRL, LDELAY);
		sr32(CM1_ABE_MCBSP3_CLKCTRL, 0, 32, 0x08000002);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_MCBSP3_CLKCTRL, LDELAY);
		sr32(CM1_ABE_SLIMBUS_CLKCTRL, 0, 32, 0xf02);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_SLIMBUS_CLKCTRL, LDELAY);
		sr32(CM1_ABE_TIMER5_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_TIMER5_CLKCTRL, LDELAY);
		sr32(CM1_ABE_TIMER6_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_TIMER6_CLKCTRL, LDELAY);
		sr32(CM1_ABE_TIMER7_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_TIMER7_CLKCTRL, LDELAY);
		sr32(CM1_ABE_TIMER8_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_TIMER8_CLKCTRL, LDELAY);
		sr32(CM1_ABE_WDT3_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT17|BIT16, 0, CM1_ABE_WDT3_CLKCTRL, LDELAY);
		/* Disable sleep transitions */
		sr32(CM1_ABE_CLKSTCTRL, 0, 32, 0x0);
	}

	/* L4PER clocks */
	sr32(CM_L4PER_CLKSTCTRL, 0, 32, 0x2);
	sr32(CM_L4PER_DMTIMER10_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER10_CLKCTRL, LDELAY);
	sr32(CM_L4PER_DMTIMER11_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER11_CLKCTRL, LDELAY);
	sr32(CM_L4PER_DMTIMER2_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_DMTIMER3_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_DMTIMER4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER4_CLKCTRL, LDELAY);
	sr32(CM_L4PER_DMTIMER9_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_DMTIMER9_CLKCTRL, LDELAY);

	/* GPIO clocks */
	sr32(CM_L4PER_GPIO2_CLKCTRL, 0 ,32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_GPIO2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_GPIO3_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_GPIO3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_GPIO4_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_GPIO4_CLKCTRL, LDELAY);
	sr32(CM_L4PER_GPIO4_CLKCTRL, 8, 1, 0x1);
	sr32(CM_L4PER_GPIO5_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_GPIO5_CLKCTRL, LDELAY);
	sr32(CM_L4PER_GPIO6_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_GPIO6_CLKCTRL, LDELAY);

	sr32(CM_L4PER_HDQ1W_CLKCTRL, 0, 32, 0x2);

	/* I2C clocks */
	sr32(CM_L4PER_I2C1_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_I2C1_CLKCTRL, LDELAY);
	sr32(CM_L4PER_I2C2_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_I2C2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_I2C3_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_I2C3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_I2C4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_I2C4_CLKCTRL, LDELAY);

	sr32(CM_L4PER_MCBSP4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MCBSP4_CLKCTRL, LDELAY);

	/* MCSPI clocks */
	sr32(CM_L4PER_MCSPI1_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MCSPI1_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MCSPI2_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MCSPI2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MCSPI3_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MCSPI3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MCSPI4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MCSPI4_CLKCTRL, LDELAY);

	/* MMC clocks */
	sr32(CM_L3INIT_HSMMC1_CLKCTRL, 0, 2, 0x2);
	sr32(CM_L3INIT_HSMMC1_CLKCTRL, 24, 1, 0x1);
	//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_HSMMC1_CLKCTRL, LDELAY);
	sr32(CM_L3INIT_HSMMC2_CLKCTRL, 0, 2, 0x2);
	sr32(CM_L3INIT_HSMMC2_CLKCTRL, 24, 1, 0x1);
	//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_HSMMC2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MMCSD3_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT18|BIT17|BIT16, 0, CM_L4PER_MMCSD3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MMCSD4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT18|BIT17|BIT16, 0, CM_L4PER_MMCSD4_CLKCTRL, LDELAY);
	sr32(CM_L4PER_MMCSD5_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_MMCSD5_CLKCTRL, LDELAY);

	/* UART clocks */
	sr32(CM_L4PER_UART1_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_UART1_CLKCTRL, LDELAY);
	sr32(CM_L4PER_UART2_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_UART2_CLKCTRL, LDELAY);
	sr32(CM_L4PER_UART3_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_UART3_CLKCTRL, LDELAY);
	sr32(CM_L4PER_UART4_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_L4PER_UART4_CLKCTRL, LDELAY);

	/* WKUP clocks */
	sr32(CM_WKUP_GPIO1_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_WKUP_GPIO1_CLKCTRL, LDELAY);
	sr32(CM_WKUP_TIMER1_CLKCTRL, 0, 32, 0x01000002);
	wait_on_value(BIT17|BIT16, 0, CM_WKUP_TIMER1_CLKCTRL, LDELAY);

	sr32(CM_WKUP_KEYBOARD_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_WKUP_KEYBOARD_CLKCTRL, LDELAY);

	sr32(CM_SDMA_CLKSTCTRL, 0, 32, 0x0);
	sr32(CM_MEMIF_CLKSTCTRL, 0, 32, 0x3);
	sr32(CM_MEMIF_EMIF_1_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_MEMIF_EMIF_1_CLKCTRL, LDELAY);
	sr32(CM_MEMIF_EMIF_2_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_MEMIF_EMIF_2_CLKCTRL, LDELAY);
	sr32(CM_D2D_CLKSTCTRL, 0, 32, 0x3);
	sr32(CM_L3_2_GPMC_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L3_2_GPMC_CLKCTRL, LDELAY);
	sr32(CM_L3INSTR_L3_3_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L3INSTR_L3_3_CLKCTRL, LDELAY);
	sr32(CM_L3INSTR_L3_INSTR_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L3INSTR_L3_INSTR_CLKCTRL, LDELAY);
	sr32(CM_L3INSTR_OCP_WP1_CLKCTRL, 0, 32, 0x1);
	wait_on_value(BIT17|BIT16, 0, CM_L3INSTR_OCP_WP1_CLKCTRL, LDELAY);

	/* WDT clocks */
	sr32(CM_WKUP_WDT2_CLKCTRL, 0, 32, 0x2);
	wait_on_value(BIT17|BIT16, 0, CM_WKUP_WDT2_CLKCTRL, LDELAY);


	if (get_omap_rev() == OMAP4430_ES1_0) {
		/* Enable Camera clocks */
		sr32(CM_CAM_CLKSTCTRL, 0, 32, 0x3);
		sr32(CM_CAM_ISS_CLKCTRL, 0, 32, 0x102);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_CAM_ISS_CLKCTRL, LDELAY);
		sr32(CM_CAM_FDIF_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_CAM_FDIF_CLKCTRL, LDELAY);
		sr32(CM_CAM_CLKSTCTRL, 0, 32, 0x0);

		/* Enable DSS clocks */
		/* PM_DSS_PWRSTCTRL ON State and LogicState = 1 (Retention) */
		writel(0x7, 0x4A307100);
		sr32(CM_DSS_CLKSTCTRL, 0, 32, 0x2);
		sr32(CM_DSS_DSS_CLKCTRL, 0, 32, 0xf02);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_DSS_DSS_CLKCTRL, LDELAY);
		sr32(CM_DSS_DEISS_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_DSS_DEISS_CLKCTRL, LDELAY);
		/* Check for DSS Clocks */
		while((readl(0x4A009100) & 0xF00) != 0xE00)
			;
		/* Set HW_AUTO transition mode */
		sr32(CM_DSS_CLKSTCTRL, 0, 32, 0x3);

		/* Enable SGX clocks */
		sr32(CM_SGX_CLKSTCTRL, 0, 32, 0x2);
		sr32(CM_SGX_SGX_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_SGX_SGX_CLKCTRL, LDELAY);
		/* Check for SGX FCLK and ICLK */
		while(readl(0x4A009200) != 0x302)
			;
		//sr32(CM_SGX_CLKSTCTRL, 0, 32, 0x0);
		/* Enable hsi/unipro/usb clocks */
		sr32(CM_L3INIT_HSI_CLKCTRL, 0, 32, 0x1);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_HSI_CLKCTRL, LDELAY);
		sr32(CM_L3INIT_UNIPRO1_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_UNIPRO1_CLKCTRL, LDELAY);
		sr32(CM_L3INIT_HSUSBHOST_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_HSUSBHOST_CLKCTRL, LDELAY);
		sr32(CM_L3INIT_HSUSBOTG_CLKCTRL, 0, 32, 0x1);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_HSUSBOTG_CLKCTRL, LDELAY);
		sr32(CM_L3INIT_HSUSBTLL_CLKCTRL, 0, 32, 0x1);
		//wait_on_value(BIT17|BIT16, 0, CM_L3INIT_HSUSBTLL_CLKCTRL, LDELAY);
		sr32(CM_L3INIT_FSUSB_CLKCTRL, 0, 32, 0x2);
		//wait_on_value(BIT18|BIT17|BIT16, 0, CM_L3INIT_FSUSB_CLKCTRL, LDELAY);
		/* enable the 32K, 48M optional clocks and enable the module */
		sr32(CM_L3INIT_USBPHY_CLKCTRL, 0, 32, 0x301);
		//wait_on_value(BIT17|BIT16, 0, CM_L3INIT_USBPHY_CLKCTRL, LDELAY);
	}

	/* Select DPLL PER CLOCK as source for SGX FCLK */
	sr32(CM_SGX_SGX_CLKCTRL, 24, 1, 0x1);

	/* Enable clocks for USB fast boot to work */
	sr32(CM_L3INIT_USBPHY_CLKCTRL, 0, 32, 0x301);
	sr32(CM_L3INIT_HSUSBOTG_CLKCTRL, 0, 32, 0x1);

	/* Enable DSS clocks:
	 * In future we want to enable DSS in bootloader to show splash screen
	 */
	sr32(PM_DSS_PWRSTCTRL, 0, 2, 0x3);
	sr32(CM_DSS_CLKSTCTRL, 0, 2, 0x2);
	sr32(CM_DSS_DSS_CLKCTRL, 0, 11, 0x702);

	return;
}

/******************************************************************************
 * prcm_init() - inits clocks for PRCM as defined in clocks.h
 *   -- called from SRAM, or Flash (using temp SRAM stack).
 *****************************************************************************/
void prcm_init(void)
{
	u32 clk_index;

	/* Get the sysclk speed from cm_sys_clksel
	 * Set the CM_SYS_CLKSEL in case ROM code has not set
	 */
	writel(0x7,CM_SYS_CLKSEL);
	clk_index = readl(CM_SYS_CLKSEL);
	if (!clk_index)
		return; /* Sys clk uninitialized */

	/* Configure all DPLL's at 100% OPP */
	configure_mpu_dpll(clk_index - 1);
	configure_iva_dpll(clk_index - 1);
	configure_per_dpll(clk_index - 1);
	configure_abe_dpll(clk_index - 1);
	configure_usb_dpll(clk_index - 1);

#ifdef CONFIG_OMAP4_SDC
	/* Enable all clocks */
	enable_all_clocks();
#endif

	return;
}
