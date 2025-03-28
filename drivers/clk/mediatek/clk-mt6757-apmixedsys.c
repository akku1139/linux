// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-pll.h"

#include <dt-bindings/clock/mediatek,mt6757-apmixedsys.h>

#define AP_PLL_CON3		0x00c
#define AP_PLL_CON4		0x010

#define APLL1_CON0		0x2a0
#define APLL1_CON1		0x2a4
#define APLL1_CON2		0x2a8
#define APLL1_PWR_CON0		0x2b0
#define APLL2_CON0		0x2b4
#define APLL2_CON1		0x2b8
#define APLL2_CON2		0x2bc
#define APLL2_PWR_CON0		0x2c4

#define ARMPLL_L_CON0		0x210
#define ARMPLL_L_CON1		0x214
#define ARMPLL_L_PWR_CON0	0x21c
#define ARMPLL_LL_CON0		0x200
#define ARMPLL_LL_CON1		0x204
#define ARMPLL_LL_PWR_CON0	0x20c

#define CCIPLL_CON0		0x290
#define CCIPLL_CON1		0x294
#define CCIPLL_PWR_CON0		0x29c

#define MAINPLL_CON0		0x220
#define MAINPLL_CON1		0x224
#define MAINPLL_PWR_CON0	0x22c

#define MMPLL_CON0		0x240
#define MMPLL_CON1		0x244
#define MMPLL_PWR_CON0		0x24c

#define MSDCPLL_CON0		0x250
#define MSDCPLL_CON1		0x254
#define MSDCPLL_PWR_CON0	0x25c

#define TVDPLL_CON0		0x270
#define TVDPLL_CON1		0x274
#define TVDPLL_PWR_CON0		0x27c

#define UNIV2PLL_CON0		0x230
#define UNIV2PLL_CON1		0x234
#define UNIV2PLL_PWR_CON0	0x23c

#define VENCPLL_CON0		0x260
#define VENCPLL_CON1		0x264
#define VENCPLL_PWR_CON0	0x26c

#define MT6757_PLL_FMAX		(3000UL * MHZ)
#define MT6757_PLL_FMIN		(1000UL * MHZ)
#define MT6757_INTEGER_BITS	(8)

#define CON0_MT6757_RST_BAR	BIT(24)

#define PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,	\
			_pd_reg, _pd_shift, _tuner_reg, _pcw_reg,	\
			_pcw_shift, _div_table) {			\
	.id = _id,							\
	.name = _name,							\
	.reg = _reg,							\
	.pwr_reg = _pwr_reg,						\
	.en_mask = _en_mask,						\
	.flags = _flags,						\
	.rst_bar_mask = CON0_MT6757_RST_BAR,				\
	.fmax = MT6757_PLL_FMAX,					\
	.fmin = MT6757_PLL_FMIN,					\
	.pcwbits = _pcwbits,						\
	.pcwibits = MT6757_INTEGER_BITS,				\
	.pd_reg = _pd_reg,						\
	.pd_shift = _pd_shift,						\
	.tuner_reg = _tuner_reg,					\
	.pcw_reg = _pcw_reg,						\
	.pcw_shift = _pcw_shift,					\
	.div_table = _div_table,					\
}

#define PLL(_id, _name, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,	\
			_pd_reg, _pd_shift, _tuner_reg, _pcw_reg,	\
			_pcw_shift)					\
	PLL_B(_id, _name, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,	\
			_pd_reg, _pd_shift, _tuner_reg, _pcw_reg,	\
			_pcw_shift, NULL)				\

static const struct mtk_pll_div_table armpll_div_table[] = {
	{ .div = 0, .freq = MT6757_PLL_FMAX },
	{ .div = 1, .freq = 1500 * MHZ },
	{ .div = 2, .freq = 750 * MHZ },
	{ .div = 3, .freq = 375 * MHZ },
	{ .div = 4, .freq = 187500000 },
	{ /* sentinel */ }
};

static const struct mtk_pll_data apmixedsys_plls[] = {
	PLL_B(CLK_APMIXED_ARMPLL_LL, "armpll_ll", ARMPLL_LL_CON0, ARMPLL_LL_PWR_CON0, 0xF0000101, PLL_AO,
		22, ARMPLL_LL_CON1, 24, 0x0, ARMPLL_LL_CON1, 0, armpll_div_table),
	PLL_B(CLK_APMIXED_ARMPLL_L, "armpll_l", ARMPLL_L_CON0, ARMPLL_L_PWR_CON0, 0x00000101, PLL_AO,
		22, ARMPLL_L_CON1, 24, 0x0, ARMPLL_L_CON1, 0, armpll_div_table),
	PLL(CLK_APMIXED_CCIPLL, "ccipll", CCIPLL_CON0, CCIPLL_PWR_CON0, 0x00000101, PLL_AO,
		22, CCIPLL_CON1, 24, 0x0, CCIPLL_CON1, 0),
	PLL(CLK_APMIXED_MAINPLL, "mainpll", MAINPLL_CON0, MAINPLL_PWR_CON0, 0xF0000101, HAVE_RST_BAR,
		22, MAINPLL_CON0, 4, 0x0, MAINPLL_CON1, 0),
	PLL(CLK_APMIXED_UNIV2PLL, "univ2pll", UNIV2PLL_CON0, UNIV2PLL_PWR_CON0, 0xFE000101, HAVE_RST_BAR,
		8, UNIV2PLL_CON0, 4, 0x0, UNIV2PLL_CON1, 14),
	PLL(CLK_APMIXED_MSDCPLL, "msdcpll", MSDCPLL_CON0, MSDCPLL_PWR_CON0, 0x00000101, 0,
		22, MSDCPLL_CON0, 4, 0x0, MSDCPLL_CON1, 0),
	PLL(CLK_APMIXED_VENCPLL, "vencpll", VENCPLL_CON0, VENCPLL_PWR_CON0, 0x00000101, 0,
		22, VENCPLL_CON0, 4, 0x0, VENCPLL_CON1, 0),
	PLL(CLK_APMIXED_MMPLL, "mmpll", MMPLL_CON0, MMPLL_PWR_CON0, 0x00000101, 0,
		22, MMPLL_CON1, 24, 0x0, MMPLL_CON1, 0),
	PLL(CLK_APMIXED_TVDPLL, "tvdpll", TVDPLL_CON0, TVDPLL_PWR_CON0, 0xC0000101, 0,
		22, TVDPLL_CON0, 4, 0x0, TVDPLL_CON1, 0),
	PLL(CLK_APMIXED_APLL1, "apll1", APLL1_CON0, APLL1_PWR_CON0, 0x00000130, 0,
		32, APLL1_CON0, 4, APLL1_CON2, APLL1_CON1, 0),
	PLL(CLK_APMIXED_APLL2, "apll2", APLL2_CON0, APLL2_PWR_CON0, 0x00000130, 0,
		32, APLL2_CON0, 4, APLL2_CON2, APLL2_CON1, 0),
};

static int clk_mt6757_apmixed_probe(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data;
	struct device *dev = &pdev->dev;
	struct device_node *node = dev->of_node;
	void __iomem *base;
	int ret;

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	clk_data = mtk_devm_alloc_clk_data(dev, ARRAY_SIZE(apmixedsys_plls));
	if (!clk_data)
		return -ENOMEM;
	platform_set_drvdata(pdev, clk_data);

	ret = mtk_clk_register_plls(node, apmixedsys_plls,
				   ARRAY_SIZE(apmixedsys_plls), clk_data);
	if (ret) {
		dev_err(dev, "Failed to register PLLs: %d\n", ret);
		return ret;
	}

	ret = devm_of_clk_add_hw_provider(dev, of_clk_hw_onecell_get,
				   clk_data);
	if (ret) {
		dev_err(dev, "Failed to register clock provider: %d\n", ret);
		goto unregister_plls;
	}

	/* Set L, LL and TDCLK to SW mode, others to HW mode */
	writel(0x7D555C, base + AP_PLL_CON3);
	writel(0x2005, base + AP_PLL_CON4);

	return 0;

unregister_plls:
	mtk_clk_unregister_plls(apmixedsys_plls, ARRAY_SIZE(apmixedsys_plls),
				   clk_data);
	return ret;
}

static void clk_mt6757_apmixed_remove(struct platform_device *pdev)
{
	struct clk_hw_onecell_data *clk_data = platform_get_drvdata(pdev);

	mtk_clk_unregister_plls(apmixedsys_plls, ARRAY_SIZE(apmixedsys_plls), clk_data);
}

static const struct of_device_id of_match_mt6757_apmixedsys[] = {
	{ .compatible = "mediatek,mt6757-apmixedsys" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_mt6757_apmixedsys);

static struct platform_driver clk_mt6757_apmixedsys = {
	.probe = clk_mt6757_apmixed_probe,
	.remove = clk_mt6757_apmixed_remove,
	.driver = {
		.name = "clk-mt6757-apmixedsys",
		.of_match_table = of_match_mt6757_apmixedsys,
	},
};
module_platform_driver(clk_mt6757_apmixedsys);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 apmixedsys clock driver");
MODULE_LICENSE("GPL");
