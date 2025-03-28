// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-cpumux.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mediatek,mt6757-infracfg.h>
#include <dt-bindings/reset/mediatek,mt6757-infracfg.h>

#define INFRA_TOP_CKMUXSEL		0x00

#define INFRA_MODULE_SW_CG_0_SET	0x80
#define INFRA_MODULE_SW_CG_0_CLR	0x84
#define INFRA_MODULE_SW_CG_0_STA	0x90

#define INFRA_MODULE_SW_CG_1_SET	0x88
#define INFRA_MODULE_SW_CG_1_CLR	0x8c
#define INFRA_MODULE_SW_CG_1_STA	0x94

#define INFRA_MODULE_SW_CG_2_SET	0xa4
#define INFRA_MODULE_SW_CG_2_CLR	0xa8
#define INFRA_MODULE_SW_CG_2_STA	0xac

#define INFRA_GLOBALCON_RST_0		0x20

#define RST_NR_PER_BANK			32

#define GATE_IFR0(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &infracfg_module_sw_cg_0_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_IFR1(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &infracfg_module_sw_cg_1_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_IFR2(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &infracfg_module_sw_cg_2_regs, _shift, &mtk_clk_gate_ops_setclr)

static struct mtk_gate_regs infracfg_module_sw_cg_0_regs = {
	.set_ofs = INFRA_MODULE_SW_CG_0_SET,
	.clr_ofs = INFRA_MODULE_SW_CG_0_CLR,
	.sta_ofs = INFRA_MODULE_SW_CG_0_STA,
};

static struct mtk_gate_regs infracfg_module_sw_cg_1_regs = {
	.set_ofs = INFRA_MODULE_SW_CG_1_SET,
	.clr_ofs = INFRA_MODULE_SW_CG_1_CLR,
	.sta_ofs = INFRA_MODULE_SW_CG_1_STA,
};

static struct mtk_gate_regs infracfg_module_sw_cg_2_regs = {
	.set_ofs = INFRA_MODULE_SW_CG_2_SET,
	.clr_ofs = INFRA_MODULE_SW_CG_2_CLR,
	.sta_ofs = INFRA_MODULE_SW_CG_2_STA,
};

static const struct mtk_gate infracfg_gates[] = {
	/* MODULE_SW_CG_0 */
	GATE_IFR0(CLK_INFRA_PMIC_TMR, "pmic_tmr", "pmicspi_sel", 0),
	GATE_IFR0(CLK_INFRA_PMIC_AP, "pmic_ap", "pmicspi_sel", 1),
	GATE_IFR0(CLK_INFRA_PMIC_MD, "pmic_md", "pmicspi_sel", 2),
	GATE_IFR0(CLK_INFRA_PMIC_CONN, "pmic_conn", "pmicspi_sel", 3),
	GATE_IFR0(CLK_INFRA_SCPSYS, "scpsys", "scp_sel", 4),
	GATE_IFR0(CLK_INFRA_SEJ, "sej", "clk26m", 5),
	GATE_IFR0(CLK_INFRA_APXGPT, "apxgpt", "axi_sel", 6),
	GATE_IFR0(CLK_INFRA_ICUSB, "icusb", "axi_sel", 8),
	GATE_IFR0(CLK_INFRA_GCE, "gce", "axi_sel", 9),
	GATE_IFR0(CLK_INFRA_THERM, "therm", "axi_sel", 10),
	GATE_IFR0(CLK_INFRA_I2C0, "i2c0", "i2c_sel", 11),
	GATE_IFR0(CLK_INFRA_I2C1, "i2c1", "i2c_sel", 12),
	GATE_IFR0(CLK_INFRA_I2C2, "i2c2", "i2c_sel", 13),
	GATE_IFR0(CLK_INFRA_I2C3, "i2c3", "i2c_sel", 14),
	GATE_IFR0(CLK_INFRA_PWM_HCLK, "pwm_hclk", "axi_sel", 15),
	GATE_IFR0(CLK_INFRA_PWM1, "pwm1", "i2c_sel", 16),
	GATE_IFR0(CLK_INFRA_PWM2, "pwm2", "i2c_sel", 17),
	GATE_IFR0(CLK_INFRA_PWM3, "pwm3", "i2c_sel", 18),
	GATE_IFR0(CLK_INFRA_PWM4, "pwm4", "i2c_sel", 19),
	GATE_IFR0(CLK_INFRA_PWM, "pwm", "i2c_sel", 21),
	GATE_IFR0(CLK_INFRA_UART0, "uart0", "uart_sel", 22),
	GATE_IFR0(CLK_INFRA_UART1, "uart1", "uart_sel", 23),
	GATE_IFR0(CLK_INFRA_UART2, "uart2", "uart_sel", 24),
	GATE_IFR0(CLK_INFRA_UART3, "uart3", "uart_sel", 25),
	GATE_IFR0(CLK_INFRA_MD2MD_CCIF_0, "md2md_ccif_0", "axi_sel", 27),
	GATE_IFR0(CLK_INFRA_MD2MD_CCIF_1, "md2md_ccif_1", "axi_sel", 28),
	GATE_IFR0(CLK_INFRA_MD2MD_CCIF_2, "md2md_ccif_2", "axi_sel", 29),
	GATE_IFR0(CLK_INFRA_BTIF, "btif", "axi_sel", 31),

	/* MODULE_SW_CG_1 */
	GATE_IFR1(CLK_INFRA_MD2MD_CCIF_3, "md2md_ccif_3", "axi_sel", 0),
	GATE_IFR1(CLK_INFRA_SPI0, "spi0", "spi_sel", 1),
	GATE_IFR1(CLK_INFRA_MSDC0, "msdc0", "msdc50_0_sel", 2),
	GATE_IFR1(CLK_INFRA_MD2MD_CCIF_4, "md2md_ccif_4", "axi_sel", 3),
	GATE_IFR1(CLK_INFRA_MSDC1, "msdc1", "msdc30_1_sel", 4),
	GATE_IFR1(CLK_INFRA_MSDC2, "msdc2", "msdc30_2_sel", 5),
	GATE_IFR1(CLK_INFRA_MSDC3, "msdc3", "msdc30_3_sel", 6),
	GATE_IFR1(CLK_INFRA_MD2MD_CCIF_5, "md2md_ccif_5", "axi_sel", 7),
	GATE_IFR1(CLK_INFRA_GCPU, "gcpu", "axi_sel", 8),
	GATE_IFR1(CLK_INFRA_TRNG, "trng", "axi_sel", 9),
	GATE_IFR1(CLK_INFRA_AUXADC, "auxadc", "clk26m", 10),
	GATE_IFR1(CLK_INFRA_CPUM, "cpum", "axi_sel", 11),
	GATE_IFR1(CLK_INFRA_CCIF1_AP, "ccif1_ap", "axi_sel", 12),
	GATE_IFR1(CLK_INFRA_CCIF1_MD, "ccif1_md", "axi_sel", 13),
	GATE_IFR1(CLK_INFRA_AP_DMA, "ap_dma", "axi_sel", 18),
	GATE_IFR1(CLK_INFRA_XIU, "xiu", "axi_sel", 19),
	GATE_IFR1(CLK_INFRA_DEVICE_APC, "device_apc", "axi_sel", 20),
	GATE_IFR1(CLK_INFRA_XUI2AHB, "xui2ahb", "axi_sel", 21),
	GATE_IFR1(CLK_INFRA_L2C_SRAM, "l2c_sram", "mm_sel", 22),
	GATE_IFR1(CLK_INFRA_CCIF_AP, "ccif_ap", "axi_sel", 23),
	GATE_IFR1(CLK_INFRA_DEBUGSYS, "debugsys", "axi_sel", 24),
	GATE_IFR1(CLK_INFRA_AUDIO, "audio", "axi_sel", 25),
	GATE_IFR1(CLK_INFRA_CCIF_MD, "ccif_md", "axi_sel", 26),
	GATE_MTK_FLAGS(CLK_INFRA_DRAMC_F26M, "dramc_f26m", "clk26m", &infracfg_module_sw_cg_1_regs, 31, &mtk_clk_gate_ops_setclr,
		CLK_IS_CRITICAL),

	/* MODULE_SW_CG_2 */
	GATE_IFR2(CLK_INFRA_IRTX, "irtx", "clk26m", 0),
	GATE_IFR2(CLK_INFRA_SSUSB_TOP, "ssusb_top", "ssusb_top_sys_sel", 1),
	GATE_IFR2(CLK_INFRA_DISP_PWM, "disp_pwm", "disp_pwm_sel", 2),
	GATE_IFR2(CLK_INFRA_CLDMA_BCLK, "cldma_bclk", "axi_sel", 3),
	GATE_IFR2(CLK_INFRA_AUDIO_26M_BCLK, "audio_26m_bclk", "clk26m", 4),
	GATE_IFR2(CLK_INFRA_MODEM_TEMP_26M_BCLK, "modem_temp_26m_bclk", "clk26m", 5),
	GATE_IFR2(CLK_INFRA_SPI1, "spi1", "spi_sel", 6),
	GATE_IFR2(CLK_INFRA_I2C4, "i2c4", "i2c_sel", 7),
	GATE_IFR2(CLK_INFRA_MODEM_TEMP_SHARE, "modem_temp_share", "axi_sel", 8),
	GATE_IFR2(CLK_INFRA_SPI2, "spi2", "spi_sel", 9),
	GATE_IFR2(CLK_INFRA_SPI3, "spi3", "spi_sel", 10),
	GATE_IFR2(CLK_INFRA_I2C5, "i2c5", "i2c_sel", 18),
	GATE_IFR2(CLK_INFRA_I2C5_ARBITER, "i2c5_arbiter", "i2c_sel", 19),
	GATE_IFR2(CLK_INFRA_I2C5_IMM, "i2c5_imm", "i2c_sel", 20),
	GATE_IFR2(CLK_INFRA_I2C1_ARBITER, "i2c1_arbiter", "i2c_sel", 21),
	GATE_IFR2(CLK_INFRA_I2C1_IMM, "i2c1_imm", "i2c_sel", 22),
	GATE_IFR2(CLK_INFRA_I2C2_ARBITER, "i2c2_arbiter", "i2c_sel", 23),
	GATE_IFR2(CLK_INFRA_I2C2_IMM, "i2c2_imm", "i2c_sel", 24),
	GATE_IFR2(CLK_INFRA_SPI4, "spi4", "spi_sel", 25),
	GATE_IFR2(CLK_INFRA_SPI5, "spi5", "spi_sel", 26),
};

static const char * const armpll_bus_sel_parents[] = {
	"clk26m",
	"ccipll",
	"armpll_div_pll1",
	"armpll_div_pll2",
};

static const char * const armpll_l_sel_parents[] = {
	"clk26m",
	"armpll_l",
	"armpll_div_pll1",
	"armpll_div_pll2",
};

static const char * const armpll_ll_sel_parents[] = {
	"clk26m",
	"armpll_ll",
	"armpll_div_pll1",
	"armpll_div_pll2",
};

static const struct mtk_composite infracfg_cpu_muxes[] = {
	MUX(CLK_INFRA_ARMPLL_L_SEL, "infra_armpll_l_sel", armpll_l_sel_parents, INFRA_TOP_CKMUXSEL, 4, 2),
	MUX(CLK_INFRA_ARMPLL_LL_SEL, "infra_armpll_ll_sel", armpll_ll_sel_parents, INFRA_TOP_CKMUXSEL, 8, 2),
	MUX(CLK_INFRA_ARMPLL_BUS_SEL, "infra_armpll_bus_sel", armpll_bus_sel_parents, INFRA_TOP_CKMUXSEL, 12, 2),
};

static u16 infracfg_rst_bank_ofs[] = { INFRA_GLOBALCON_RST_0 };

static u16 infracfg_rst_idx_map[] = {
	/* GLOBALCON_RST_0 */
	[MT6757_INFRA_RST0_THERM_CTRL] = 0 * RST_NR_PER_BANK + 0,
	[MT6757_INFRA_RST0_USB_TOP] = 0 * RST_NR_PER_BANK + 1,
	[MT6757_INFRA_RST0_PERI_IOMMU] = 0 * RST_NR_PER_BANK + 2,
	[MT6757_INFRA_RST0_MM_IOMMU] = 0 * RST_NR_PER_BANK + 3,
	[MT6757_INFRA_RST0_MSDC3] = 0 * RST_NR_PER_BANK + 4,
	[MT6757_INFRA_RST0_MSDC2] = 0 * RST_NR_PER_BANK + 5,
	[MT6757_INFRA_RST0_MSDC1] = 0 * RST_NR_PER_BANK + 6,
	[MT6757_INFRA_RST0_MSDC0] = 0 * RST_NR_PER_BANK + 7,
	[MT6757_INFRA_RST0_DRAMC] = 0 * RST_NR_PER_BANK + 8,
	[MT6757_INFRA_RST0_AP_DMA] = 0 * RST_NR_PER_BANK + 9,
	[MT6757_INFRA_RST0_MIPI_D] = 0 * RST_NR_PER_BANK + 10,
	[MT6757_INFRA_RST0_MIPI_C] = 0 * RST_NR_PER_BANK + 11,
	[MT6757_INFRA_RST0_BTIF] = 0 * RST_NR_PER_BANK + 12,
	[MT6757_INFRA_RST0_SSUSB_TOP] = 0 * RST_NR_PER_BANK + 13,
	[MT6757_INFRA_RST0_DISP_PWM] = 0 * RST_NR_PER_BANK + 14,
	[MT6757_INFRA_RST0_AUXADC] = 0 * RST_NR_PER_BANK + 15,

	/* GLOBALCON_RST_1 */
	[MT6757_INFRA_RST1_IRTX] = 1 * RST_NR_PER_BANK + 0,
	[MT6757_INFRA_RST1_SPI0] = 1 * RST_NR_PER_BANK + 1,
	[MT6757_INFRA_RST1_I2C0] = 1 * RST_NR_PER_BANK + 2,
	[MT6757_INFRA_RST1_I2C1] = 1 * RST_NR_PER_BANK + 3,
	[MT6757_INFRA_RST1_I2C2] = 1 * RST_NR_PER_BANK + 4,
	[MT6757_INFRA_RST1_I2C3] = 1 * RST_NR_PER_BANK + 5,
	[MT6757_INFRA_RST1_UART0] = 1 * RST_NR_PER_BANK + 6,
	[MT6757_INFRA_RST1_UART1] = 1 * RST_NR_PER_BANK + 7,
	[MT6757_INFRA_RST1_UART2] = 1 * RST_NR_PER_BANK + 8,
	[MT6757_INFRA_RST1_PWM] = 1 * RST_NR_PER_BANK + 9,
	[MT6757_INFRA_RST1_SPI1] = 1 * RST_NR_PER_BANK + 10,
	[MT6757_INFRA_RST1_I2C4] = 1 * RST_NR_PER_BANK + 11,
	[MT6757_INFRA_RST1_DVFS_PROC] = 1 * RST_NR_PER_BANK + 12,
	[MT6757_INFRA_RST1_SPI2] = 1 * RST_NR_PER_BANK + 13,
	[MT6757_INFRA_RST1_SPI3] = 1 * RST_NR_PER_BANK + 14,

	/* GLOBALCON_RST_2 */
	[MT6757_INFRA_RST2_PMIC_WRAP] = 2 * RST_NR_PER_BANK + 0,
	[MT6757_INFRA_RST2_SPM] = 2 * RST_NR_PER_BANK + 1,
	[MT6757_INFRA_RST2_USBIF] = 2 * RST_NR_PER_BANK + 2,
	[MT6757_INFRA_RST2_SCP] = 2 * RST_NR_PER_BANK + 3,
	[MT6757_INFRA_RST2_KP] = 2 * RST_NR_PER_BANK + 4,
	[MT6757_INFRA_RST2_APXGPT] = 2 * RST_NR_PER_BANK + 5,
	[MT6757_INFRA_RST2_CLDMA_TOP] = 2 * RST_NR_PER_BANK + 6,
};

static const struct mtk_clk_rst_desc infracfg_resets = {
	.version = MTK_RST_SIMPLE,
	.rst_bank_ofs = infracfg_rst_bank_ofs,
	.rst_bank_nr = ARRAY_SIZE(infracfg_rst_bank_ofs),
	.rst_idx_map = infracfg_rst_idx_map,
	.rst_idx_map_nr = ARRAY_SIZE(infracfg_rst_idx_map)
};

static int clk_mt6757_infracfg_probe(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data;
	struct device_node *node = pdev->dev.of_node;
	void __iomem *base;
	int ret;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	clk_data = mtk_alloc_clk_data(CLK_INFRA_NR_CLK);
	if (!clk_data)
		return -ENOMEM;

	ret = mtk_register_reset_controller_with_dev(&pdev->dev, &infracfg_resets);
	if (ret)
		goto free_clk_data;

	ret = mtk_clk_register_gates(&pdev->dev, node, infracfg_gates,
					  ARRAY_SIZE(infracfg_gates), clk_data);
	if (ret)
		goto free_clk_data;

	ret = mtk_clk_register_cpumuxes(&pdev->dev, node, infracfg_cpu_muxes,
					  ARRAY_SIZE(infracfg_cpu_muxes), clk_data);
	if (ret)
		goto unregister_gates;

	ret = of_clk_add_hw_provider(node, of_clk_hw_onecell_get, clk_data);
	if (ret)
		goto unregister_cpumuxes;

	return 0;

unregister_cpumuxes:
	mtk_clk_unregister_cpumuxes(infracfg_cpu_muxes, ARRAY_SIZE(infracfg_cpu_muxes), clk_data);
unregister_gates:
	mtk_clk_unregister_gates(infracfg_gates, ARRAY_SIZE(infracfg_gates), clk_data);
free_clk_data:
	mtk_free_clk_data(clk_data);
	return ret;
}

static void clk_mt6757_infracfg_remove(struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct clk_hw_onecell_data *clk_data = platform_get_drvdata(pdev);

	of_clk_del_provider(node);
	mtk_clk_unregister_cpumuxes(infracfg_cpu_muxes, ARRAY_SIZE(infracfg_cpu_muxes), clk_data);
	mtk_clk_unregister_gates(infracfg_gates, ARRAY_SIZE(infracfg_gates), clk_data);
	mtk_free_clk_data(clk_data);
}

static const struct of_device_id of_match_mt6757_infracfg[] = {
	{ .compatible = "mediatek,mt6757-infracfg" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_mt6757_infracfg);

static struct platform_driver clk_mt6757_infracfg = {
	.probe = clk_mt6757_infracfg_probe,
	.remove = clk_mt6757_infracfg_remove,
	.driver = {
		.name = "clk-mt6757-infracfg",
		.of_match_table = of_match_mt6757_infracfg,
	},
};
module_platform_driver(clk_mt6757_infracfg);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 infracfg clock and reset driver");
MODULE_LICENSE("GPL");
