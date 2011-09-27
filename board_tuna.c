
#include <aboot/aboot.h>
#include <aboot/io.h>
#include <omap4/mux.h>
#include <omap4/hw.h>

void board_late_init(void)
{
	cfg_machine_type = 3458;
	cfg_uart_base = OMAP44XX_UART3;

	gpio_write(42, 1);
	gpio_write(48, 1);
	gpio_write(49, 1);
}

void board_mux_init(void)
{

	MV(CP(GPMC_A18) , (M3));  /* gpio_42 */
	MV(CP(GPMC_A24) , (M3));  /* gpio_48 */
	MV(CP(GPMC_A25) , (M3));  /* gpio_49 */

	MV(CP(UART4_RX) , (IEN | M0));  /* uart4_rx */
	MV(CP(UART4_TX) , (M0));  /* uart4_tx */

	MV(CP(GPMC_AD0) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat0 */
	MV(CP(GPMC_AD1) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat1 */
	MV(CP(GPMC_AD2) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat2 */
	MV(CP(GPMC_AD3) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat3 */
	MV(CP(GPMC_AD4) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat4 */
	MV(CP(GPMC_AD5) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat5 */
	MV(CP(GPMC_AD6) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat6 */
	MV(CP(GPMC_AD7) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_dat7 */
	MV(CP(GPMC_NOE) , (PTU | IEN | OFF_EN | OFF_OUT_PTD | M1));  /* sdmmc2_clk */
	MV(CP(GPMC_NWE) , (PTU | IEN | OFF_EN | OFF_PD | OFF_IN | M1));  /* sdmmc2_cmd */
}


const struct ddr_regs ddr_cfg_old = {
	.tim1		= 0x10eb065a,
	.tim2		= 0x20370dd2,
	.tim3		= 0x00b1c33f,
	.phy_ctrl_1	= 0x849FF408,
	.ref_ctrl	= 0x00000618,
	.config_init	= 0x80000eb2,
	.config_final	= 0x80001ab2,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};

const struct ddr_regs ddr_cfg_new = {
	.tim1		= 0x10eb07a2,
	.tim2		= 0x20370dd2,
	.tim3		= 0x00b1a33f,
	.phy_ctrl_1	= 0x849FF418,
	.ref_ctrl	= 0x00000618,
	.config_init	= 0x80000eb2,
	.config_final	= 0x80001ab2,
	.zq_config	= 0x500b3215,
	.mr1		= 0x83,
	.mr2		= 0x4
};


void board_ddr_init(void)
{
#if 0
	writel(0x80640300, DMM_BASE + DMM_LISA_MAP_0);
	writel(0x00000000, DMM_BASE + DMM_LISA_MAP_2);
	writel(0xFF020100, DMM_BASE + DMM_LISA_MAP_3);

	omap4_ddr_init(&ddr_cfg_old, &ddr_cfg_old);
#else
	writel(0x80640300, DMM_BASE + DMM_LISA_MAP_0);
	writel(0x80640300, MA_BASE + DMM_LISA_MAP_0);

	writel(0x00000000, DMM_BASE + DMM_LISA_MAP_2);
	writel(0x80640300, DMM_BASE + DMM_LISA_MAP_3);
	writel(0x00000000, MA_BASE + DMM_LISA_MAP_2);
	writel(0xFF020100, MA_BASE + DMM_LISA_MAP_3);

	writel(0x7c7c7c7c, 0x4A100638);
	writel(0x7c7c7c7c, 0x4A10063c);
	writel(0x7c7c7c00, 0x4A100640);
	writel(0x7c7c7c7c, 0x4A100648);
	writel(0x7c7c7c7c, 0x4A10064c);
	writel(0x7c7c7c00, 0x4A100650);
	writel(0xa388bc03, 0x4A100644);
	writel(0xa388bc03, 0x4A100654);

	omap4_ddr_init(&ddr_cfg_new, &ddr_cfg_new);
#endif
	writel(0x0, 0x80000000);
	writel(0x0, 0x80000080);
}
