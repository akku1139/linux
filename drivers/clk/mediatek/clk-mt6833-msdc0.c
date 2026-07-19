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

static const struct mtk_gate_regs msdc0_cg_regs = {
	.set_ofs = 0xb4,
	.clr_ofs = 0xb4,
	.sta_ofs = 0xb4,
};

#define GATE_MSDC0(_id, _name, _parent, _shift) {	\
		.id = _id,				\
		.name = _name,				\
		.parent_name = _parent,			\
		.regs = &msdc0_cg_regs,			\
		.shift = _shift,			\
		.ops = &mtk_clk_gate_ops_no_setclr_inv,	\
	}

static const struct mtk_gate msdc0_clks[] = {
	GATE_MSDC0(CLK_MSDC0_AXI_WRAP_CKEN, "msdc0_axi_wrap_cken", "axi_ck", 22),
};

static const struct mtk_clk_desc msdc0_desc = {
	.clks = msdc0_clks,
	.num_clks = ARRAY_SIZE(msdc0_clks),
};

static const struct of_device_id of_match_clk_mt6833_msdc0[] = {
	{ .compatible = "mediatek,mt6833-msdc0sys", .data = &msdc0_desc },
	{}
};
MODULE_DEVICE_TABLE(of, of_match_clk_mt6833_msdc0);

static struct platform_driver clk_mt6833_msdc0_drv = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6833-msdc0",
		.of_match_table = of_match_clk_mt6833_msdc0,
	},
};

module_platform_driver(clk_mt6833_msdc0_drv);
