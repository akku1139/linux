// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-gate.h"
#include "clk-mtk.h"

#include <dt-bindings/clock/mediatek,mt6757-camsys.h>

#define CAMSYS_CG_CON			0x00
#define CAMSYS_CG_SET			0x04
#define CAMSYS_CG_CLR			0x08

static struct mtk_gate_regs camsys_cg_regs = {
	.set_ofs = CAMSYS_CG_SET,
	.clr_ofs = CAMSYS_CG_CLR,
	.sta_ofs = CAMSYS_CG_CON,
};

#define GATE_CAM(_id, _name, _parent, _shift)		\
	GATE_MTK(_id, _name, _parent, &camsys_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

static const struct mtk_gate camsys_gates[] = {
	GATE_CAM(CLK_CAM_SMI_LARB2, "cam_smi_larb2", "img_sel", 0),
	GATE_CAM(CLK_CAM_CAM, "cam_cam", "img_sel", 6),
	GATE_CAM(CLK_CAM_CAMTG, "cam_camtg", "camtg_sel", 7),
	GATE_CAM(CLK_CAM_SENINF, "cam_seninf", "img_sel", 8),
	GATE_CAM(CLK_CAM_CAMSV0, "cam_camsv0", "img_sel", 9),
	GATE_CAM(CLK_CAM_CAMSV1, "cam_camsv1", "img_sel", 10),
	GATE_CAM(CLK_CAM_CAMSV2, "cam_camsv2", "img_sel", 11),
	GATE_CAM(CLK_CAM_TSF, "cam_tsf", "img_sel", 12),
};

static const struct mtk_clk_desc camsys_clks = {
	.clks = camsys_gates,
	.num_clks = ARRAY_SIZE(camsys_gates),
};

static const struct of_device_id of_match_mt6757_camsys[] = {
	{ .compatible = "mediatek,mt6757-camsys", .data = &camsys_clks },
	{ /* sentinel */ }
};

static struct platform_driver clk_mt6757_camsys = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-camsys",
		.of_match_table = of_match_mt6757_camsys,
	},
};
module_platform_driver(clk_mt6757_camsys);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("Mediatek MT6757 camsys clock driver");
MODULE_LICENSE("GPL");
