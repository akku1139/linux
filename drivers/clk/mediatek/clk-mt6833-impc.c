// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020 MediaTek Inc.
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>
#include <linux/module.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mediatek,mt6833-clk.h>

#define INV_OFS			-1

/* get spm power status struct to register inside clk_data */
static struct pwr_status pwr_stat = GATE_PWR_STAT(INV_OFS, INV_OFS,
		0x00b0, BIT(15), 0);

static const struct mtk_gate_regs impc_cg_regs = {
	.set_ofs = 0xe08,
	.clr_ofs = 0xe04,
	.sta_ofs = 0xe00,
};

#define GATE_IMPC(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &impc_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
		.pwr_stat = &pwr_stat,			\
	}

static const struct mtk_gate impc_clks[] = {
	GATE_IMPC(CLK_IMPC_AP_CLOCK_RO_I2C10, "impc_ap_i2c10", "i2c_pseudo", 0),
	GATE_IMPC(CLK_IMPC_AP_CLOCK_RO_I2C11, "impc_ap_i2c11", "i2c_pseudo", 1),
};

static int clk_mt6833_impc_probe(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	int r;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_IMPC_NR_CLK);

	mtk_clk_register_gates(node, impc_clks, ARRAY_SIZE(impc_clks),
			clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r)
		pr_notice("%s(): could not register clock provider: %d\n",
			__func__, r);

	return r;
}

static const struct of_device_id of_match_clk_mt6833_impc[] = {
	{ .compatible = "mediatek,mt6833-imp_iic_wrap_c", },
	{}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6833_impc);

static struct platform_driver clk_mt6833_impc_drv = {
	.probe = clk_mt6833_impc_probe,
	.driver = {
		.name = "clk-mt6833-impc",
		.of_match_table = of_match_clk_mt6833_impc,
	},
};

builtin_platform_driver(clk_mt6833_impc_drv);
