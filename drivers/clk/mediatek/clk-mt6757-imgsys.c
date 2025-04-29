// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6757-imgsys.h>

#define IMGSYS_CG_CON			0x00
#define IMGSYS_CG_SET			0x04
#define IMGSYS_CG_CLR			0x08

static struct mtk_gate_regs imgsys_cg_regs = {
	.set_ofs = IMGSYS_CG_SET,
	.clr_ofs = IMGSYS_CG_CLR,
	.sta_ofs = IMGSYS_CG_CON,
};

#define GATE_IMG(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &imgsys_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate imgsys_gates[] = {
	GATE_IMG(CLK_IMG_SMI_LARB5, "img_smi_larb5", "img_sel", 0),
	GATE_IMG(CLK_IMG_DFP_VAD, "img_dfp_vad", "img_sel", 1),
	GATE_IMG(CLK_IMG_DIP, "img_dip", "img_sel", 6),
	GATE_IMG(CLK_IMG_DPE, "img_dpe", "img_sel", 10),
	GATE_IMG(CLK_IMG_FDVT, "img_fdvt", "img_sel", 11),
	GATE_IMG(CLK_IMG_GEPF, "img_gepf", "img_sel", 12),
	GATE_IMG(CLK_IMG_RSC, "img_rsc", "img_sel", 13),
};

static const struct mtk_clk_desc imgsys_clks = {
	.clks = imgsys_gates,
	.num_clks = ARRAY_SIZE(imgsys_gates),
};

static const struct of_device_id of_match_mt6757_imgsys[] = {
	{ .compatible = "mediatek,mt6757-imgsys", .data = &imgsys_clks },
	{ /* sentinel */ }
};

static struct platform_driver clk_mt6757_imgsys = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-imgsys",
		.of_match_table = of_match_mt6757_imgsys,
	},
};
module_platform_driver(clk_mt6757_imgsys);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("Mediatek MT6757 imgsys clock driver");
MODULE_LICENSE("GPL");
