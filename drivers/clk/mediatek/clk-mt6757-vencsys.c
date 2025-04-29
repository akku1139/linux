// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6757-vencsys.h>

#define VENCSYS_CG_CON			0x00
#define VENCSYS_CG_SET			0x04
#define VENCSYS_CG_CLR			0x08

static struct mtk_gate_regs venc_cg_regs = {
	.set_ofs = VENCSYS_CG_SET,
	.clr_ofs = VENCSYS_CG_CLR,
	.sta_ofs = VENCSYS_CG_CON,
};

#define GATE_VENC(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &venc_cg_regs, _shift, &mtk_clk_gate_ops_setclr_inv)

static const struct mtk_gate vencsys_gates[] = {
	GATE_VENC(CLK_VENC_SMI_LARB3, "venc_smi_larb3", "mm_sel", 0),
	GATE_VENC(CLK_VENC_VENC, "venc", "mm_sel", 4),
	GATE_VENC(CLK_VENC_JPGENC, "venc_jpgenc", "mm_sel", 8),
	GATE_VENC(CLK_VENC_JPGDEC, "venc_jpgdec", "mm_sel", 12),
};

static const struct mtk_clk_desc vencsys_clks = {
	.clks = vencsys_gates,
	.num_clks = ARRAY_SIZE(vencsys_gates),
};

static const struct of_device_id of_match_mt6757_vencsys[] = {
	{ .compatible = "mediatek,mt6757-vencsys", .data = &vencsys_clks },
	{ /* sentinel */ }
};

static struct platform_driver clk_mt6757_vencsys = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-vencsys",
		.of_match_table = of_match_mt6757_vencsys,
	},
};
module_platform_driver(clk_mt6757_vencsys);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 vencsys clock driver");
MODULE_LICENSE("GPL");
