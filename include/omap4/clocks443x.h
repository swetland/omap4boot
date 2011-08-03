/*
 * clocks443x.h
 *
 * Copyright(c) 2010 Texas Instruments.   All rights reserved.
 * Texas Instruments, <www.ti.com>
 * Richard Woodruff <r-woodruff2@ti.com>
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
#ifndef _OMAP443X_CLOCKS_H_
#define _OMAP443X_CLOCKS_H_

#define PLL_STOP        1         /* PER & IVA */
#define PLL_MN_POWER_BYPASS 	4
#define PLL_LOW_POWER_BYPASS   5  /* MPU, IVA & CORE */
#define PLL_FAST_RELOCK_BYPASS 6  /* CORE */
#define PLL_LOCK        7         /* MPU, IVA, CORE & PER */

/* CM_IDLEST_DPLL fields */
#define ST_DPLL_CLK_MASK                1

/* The following configurations are OPP and SysClk value independant
 * and hence are defined here. All the other DPLL related values are
 * tabulated in lowlevel_init.S.
 */

/* CORE DPLL */
#  define CORE_M3X2      2        /* 332MHz : CM_CLKSEL1_EMU */
#  define CORE_SSI_DIV   3        /* 221MHz : CM_CLKSEL_CORE */
#  define CORE_FUSB_DIV  2        /* 41.5MHz: */
#  define CORE_L4_DIV    2        /*  83MHz : L4 */
#  define CORE_L3_DIV    2        /* 166MHz : L3 {DDR} */
#  define GFX_DIV        2        /*  83MHz : CM_CLKSEL_GFX */
#  define WKUP_RSM       2        /* 41.5MHz: CM_CLKSEL_WKUP */

/* PER DPLL */
# define PER_M6X2       3         /* 288MHz: CM_CLKSEL1_EMU */
# define PER_M5X2       4         /* 216MHz: CM_CLKSEL_CAM */
# define PER_M4X2       9         /* 96MHz : CM_CLKSEL_DSS-dss1 */
# define PER_M3X2       16        /* 54MHz : CM_CLKSEL_DSS-tv */

#  define CLSEL1_EMU_VAL ((CORE_M3X2 << 16) | (PER_M6X2 << 24) | (0x0a50))

#ifdef PRCM_CLK_CFG2_332MHZ
# define M_12		0xA6
# define N_12		0x05
# define FSEL_12	0x07
# define M2_12		0x01 /* M3 of 2 */

# define M_12_ES1	0x0E
# define FSL_12_ES1 0x03
# define M2_12_ES1  0x1 /* M3 of 2 */

# define M_13       0x14C
# define N_13       0x0C
# define FSEL_13	0x03
# define M2_13		0x01 /* M3 of 2 */

# define M_13_ES1	0x1B2
# define N_13_ES1	0x10
# define FSL_13_ES1 0x03
# define M2_13_ES1	0x01 /* M3 of 2 */

# define M_19p2     0x19F
# define N_19p2     0x17
# define FSEL_19p2  0x03
# define M2_19p2    0x01 /* M3 of 2 */

# define M_19p2_ES1	0x19F
# define N_19p2_ES1	0x17
# define FSL_19p2_ES1 0x03
# define M2_19p2_ES1 0x01 /* M3 of 2 */

# define M_26       0xA6
# define N_26       0x0C
# define FSEL_26    0x07
# define M2_26      0x01 /* M3 of 2 */

# define M_26_ES1	0x1B2
# define N_26_ES1	0x21
# define FSL_26_ES1	0x03
# define M2_26_ES1	0x01 /* M3 of 2 */

# define M_38p4     0x19F
# define N_38p4     0x2F
# define FSEL_38p4  0x03
# define M2_38p4    0x01 /* M3 of 2 */

# define M_38p4_ES1	0x19F
# define N_38p4_ES1	0x2F
# define FSL_38p4_ES1 0x03
# define M2_38p4_ES1 0x01 /* M3 of 2 */

#elif defined(PRCM_CLK_CFG2_266MHZ)
# define M_12		0x85
# define N_12       0x05
# define FSEL_12    0x07
# define M2_12      0x02 /* M3 of 2 */

# define M_12_ES1	0x85  /* 0x10A */
# define N_12_ES1	0x05  /* 0x05 */
# define FSL_12_ES1 0x07  /* 0x7 */
# define M2_12_ES1  0x2   /* 0x2 with an M3 of 4*/

# define M_13       0x10A
# define N_13       0x0C
# define FSEL_13    0x3
# define M2_13      0x1 /* M3 of 2 */

# define M_13_ES1	0x10A /* 0x214 */
# define N_13_ES1	0x0C  /* 0xC */
# define FSL_13_ES1 0x3   /* 0x3 */
# define M2_13_ES1	0x1   /* 0x2 with an M3 of 4*/

# define M_19p2     0x115
# define N_19p2     0x13
# define FSEL_19p2  0x03
# define M2_19p2    0x01 /* M3 of 2 */

# define M_19p2_ES1	0x115  /* 0x299 */
# define N_19p2_ES1	0x13   /* 0x17 */
# define FSL_19p2_ES1 0x03 /* 0x03 */
# define M2_19p2_ES1 0x01  /* 0x2 with M3 of 4 */

# define M_26		0x85
# define N_26		0x0C
# define FSEL_26	0x07
# define M2_26		0x01 /* M3 of 2 */

# define M_26_ES1	0x85  /* 0x10A */
# define N_26_ES1	0x0C  /* 0xC */
# define FSL_26_ES1 0x07  /* 0x7 */
# define M2_26_ES1	0x01  /* 0x2 with an M3 of 4 */

# define M_38p4     0x11C
# define N_38p4     0x28
# define FSEL_38p4  0x03
# define M2_38p4    0x01 /* M3 of 2 */

# define M_38p4_ES1	0x11C  /* 0x299 */
# define N_38p4_ES1	0x28   /* 0x2f */
# define FSL_38p4_ES1 0x03 /* 0x3 */
# define M2_38p4_ES1 0x01  /* 0x2 with an M3 of 4*/

#endif

#endif  /* endif _OMAP443X_CLOCKS_H_ */
