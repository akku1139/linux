// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6757-pericfg.h>

#define PERI_GLOBALCON_DCMCTL	0x050

static struct mtk_gate_regs peri_cg_regs = {
	.sta_ofs = PERI_GLOBALCON_DCMCTL,
};

static const struct mtk_gate pericfg_gates[] = {
	GATE_MTK(CLK_PERI_AXI, "peri_axi", "axi_sel", &peri_cg_regs, 1, &mtk_clk_gate_ops_no_setclr),
};

static const struct mtk_clk_desc pericfg_clks = {
	.clks = pericfg_gates,
	.num_clks = ARRAY_SIZE(pericfg_gates),
};

static const struct of_device_id of_match_mt6757_pericfg[] = {
	{ .compatible = "mediatek,mt6757-pericfg", .data = &pericfg_clks },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_mt6757_pericfg);

static struct platform_driver clk_mt6757_pericfg = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-pericfg",
		.of_match_table = of_match_mt6757_pericfg,
	},
};
module_platform_driver(clk_mt6757_pericfg);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 pericfg clock driver");
MODULE_LICENSE("GPL");
