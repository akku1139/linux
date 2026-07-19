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
static struct pwr_status pwr_stat = GATE_PWR_STAT(0x16C,
		0x170, INV_OFS, BIT(14), BIT(14));

static const struct mtk_gate_regs ipe_cg_regs = {
	.set_ofs = 0x4,
	.clr_ofs = 0x8,
	.sta_ofs = 0x0,
};

#define GATE_IPE(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &ipe_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_setclr,	\
		.pwr_stat = &pwr_stat,			\
	}

static const struct mtk_gate ipe_clks[] = {
	GATE_IPE(CLK_IPE_LARB19, "ipe_larb19", "ipe_ck", 0),
	GATE_IPE(CLK_IPE_LARB20, "ipe_larb20", "ipe_ck", 1),
	GATE_IPE(CLK_IPE_SMI_SUBCOM, "ipe_smi_subcom", "ipe_ck", 2),
	GATE_IPE(CLK_IPE_FD, "ipe_fd", "ipe_ck", 3),
	GATE_IPE(CLK_IPE_FE, "ipe_fe", "ipe_ck", 4),
	GATE_IPE(CLK_IPE_RSC, "ipe_rsc", "ipe_ck", 5),
	GATE_IPE(CLK_IPE_DPE, "ipe_dpe", "dpe_ck", 6),
	GATE_IPE(CLK_IPE_GALS, "ipe_gals", "img2_ck", 8),
};

static int clk_mt6833_ipe_probe(struct platform_device *pdev)
{
	struct clk_onecell_data *clk_data;
	int r;
	struct device_node *node = pdev->dev.of_node;

	clk_data = mtk_alloc_clk_data(CLK_IPE_NR_CLK);

	mtk_clk_register_gates(node, ipe_clks, ARRAY_SIZE(ipe_clks),
			clk_data);

	r = of_clk_add_provider(node, of_clk_src_onecell_get, clk_data);

	if (r)
		pr_notice("%s(): could not register clock provider: %d\n",
			__func__, r);

	return r;
}

static const struct of_device_id of_match_clk_mt6833_ipe[] = {
	{ .compatible = "mediatek,mt6833-ipesys", },
	{}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6833_ipe);

static struct platform_driver clk_mt6833_ipe_drv = {
	.probe = clk_mt6833_ipe_probe,
	.driver = {
		.name = "clk-mt6833-ipe",
		.of_match_table = of_match_clk_mt6833_ipe,
	},
};

builtin_platform_driver(clk_mt6833_ipe_drv);
