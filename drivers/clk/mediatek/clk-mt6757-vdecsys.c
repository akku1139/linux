// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6757-vdecsys.h>
#include <dt-bindings/reset/mediatek,mt6757-vdecsys.h>

#define VDEC_CKEN_SET			0x00
#define VDEC_CKEN_CLR			0x04
#define VDEC_LARB_CKEN_SET		0x08
#define VDEC_LARB_CKEN_CLR		0x0c
#define VDEC_RESETB				0x10
#define LARB1_RESETB			0x14

#define RST_NR_PER_BANK			32

static struct mtk_gate_regs vdec_cg_regs = {
	.set_ofs = VDEC_CKEN_SET,
	.clr_ofs = VDEC_CKEN_CLR,
	.sta_ofs = VDEC_CKEN_SET,
};

static struct mtk_gate_regs vdec_larb_cg_regs = {
	.set_ofs = VDEC_LARB_CKEN_SET,
	.clr_ofs = VDEC_LARB_CKEN_CLR,
	.sta_ofs = VDEC_LARB_CKEN_SET,
};

static const struct mtk_gate vdecsys_gates[] = {
	GATE_MTK(CLK_VDEC_VDEC, "vdec", "vdec_sel", &vdec_cg_regs, 0, &mtk_clk_gate_ops_setclr_inv),
	GATE_MTK(CLK_VDEC_SMI_LARB1, "vdec_smi_larb1", "vdec_sel", &vdec_larb_cg_regs, 0, &mtk_clk_gate_ops_setclr_inv),
};

static u16 vdecsys_rst_bank_ofs[] = { VDEC_RESETB, LARB1_RESETB };

static u16 vdecsys_rst_idx_map[] = {
	[MT6757_VDEC_RST0_VDEC]		= 0 * RST_NR_PER_BANK + 0,
	[MT6757_VDEC_RST1_LARB]	= 1 * RST_NR_PER_BANK + 0,
};

static const struct mtk_clk_rst_desc vdecsys_resets = {
	.version = MTK_RST_SIMPLE,
	.rst_bank_ofs = vdecsys_rst_bank_ofs,
	.rst_bank_nr = ARRAY_SIZE(vdecsys_rst_bank_ofs),
	.rst_idx_map = vdecsys_rst_idx_map,
	.rst_idx_map_nr = ARRAY_SIZE(vdecsys_rst_idx_map)
};

static const struct mtk_clk_desc vdecsys_clks = {
	.clks = vdecsys_gates,
	.num_clks = ARRAY_SIZE(vdecsys_gates),
	.rst_desc = &vdecsys_resets
};

static const struct of_device_id of_match_mt6757_vdecsys[] = {
	{ .compatible = "mediatek,mt6757-vdecsys", .data = &vdecsys_clks },
	{ /* sentinel */ }
};

static struct platform_driver clk_mt6757_vdecsys = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-vdecsys",
		.of_match_table = of_match_mt6757_vdecsys,
	},
};
module_platform_driver(clk_mt6757_vdecsys);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 vdecsys clock and reset driver");
MODULE_LICENSE("GPL");
