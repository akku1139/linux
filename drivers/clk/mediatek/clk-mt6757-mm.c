// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/module.h>
#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"

#include <dt-bindings/clock/mediatek,mt6757-mmsys.h>

#define MMSYS_CG_CON0	0x100
#define MMSYS_CG_SET0	0x104
#define MMSYS_CG_CLR0	0x108

#define MMSYS_CG_CON1	0x110
#define MMSYS_CG_SET1	0x114
#define MMSYS_CG_CLR1	0x118

static const struct mtk_gate_regs mm0_cg_regs = {
	.set_ofs = MMSYS_CG_SET0,
	.clr_ofs = MMSYS_CG_CLR0,
	.sta_ofs = MMSYS_CG_CON0,
};

static const struct mtk_gate_regs mm1_cg_regs = {
	.set_ofs = MMSYS_CG_SET1,
	.clr_ofs = MMSYS_CG_CLR1,
	.sta_ofs = MMSYS_CG_CON1,
};

/*
 * For some clocks, we don't care what their actual rates are. And these
 * clocks may change their rate on different products or different scenarios.
 * So we model these clocks' rate as 0, to denote it's not an actual rate.
 */
#define DUMMY_RATE	0

#define FIXED_CLK_MM(_id, _name)				\
	FIXED_CLK(_id, _name, "mm_sel", DUMMY_RATE)		\

#define GATE_MM0(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &mm0_cg_regs, _shift,	\
		&mtk_clk_gate_ops_setclr)

#define GATE_MM1(_id, _name, _parent, _shift)			\
	GATE_MTK(_id, _name, _parent, &mm1_cg_regs, _shift,	\
		&mtk_clk_gate_ops_setclr)

/*
 * These clocks are defined as fixed because their bits control DCM
 * rather than gating the clock, implying that they are always on.
 */
static const struct mtk_fixed_clk mm_fixed_clks[] = {
	/* MM0 */
	FIXED_CLK_MM(CLK_MM_CAM_MDP, "mm_cam_mdp"),
	FIXED_CLK_MM(CLK_MM_MDP_RDMA0, "mm_mdp_rdma0"),
	FIXED_CLK_MM(CLK_MM_MDP_RDMA1, "mm_mdp_rdma1"),
	FIXED_CLK_MM(CLK_MM_MDP_RSZ0, "mm_mdp_rsz0"),
	FIXED_CLK_MM(CLK_MM_MDP_RSZ1, "mm_mdp_rsz1"),
	FIXED_CLK_MM(CLK_MM_MDP_RSZ2, "mm_mdp_rsz2"),
	FIXED_CLK_MM(CLK_MM_MDP_TDSHP, "mm_mdp_tdshp"),
	FIXED_CLK_MM(CLK_MM_MDP_COLOR, "mm_mdp_color"),
	FIXED_CLK_MM(CLK_MM_MDP_WDMA, "mm_mdp_wdma"),
	FIXED_CLK_MM(CLK_MM_MDP_WROT0, "mm_mdp_wrot0"),
	FIXED_CLK_MM(CLK_MM_MDP_WROT1, "mm_mdp_wrot1"),
	FIXED_CLK_MM(CLK_MM_DISP_OVL0, "mm_disp_ovl0"),
	FIXED_CLK_MM(CLK_MM_DISP_OVL1, "mm_disp_ovl1"),
	FIXED_CLK_MM(CLK_MM_DISP_OVL0_2L, "mm_disp_ovl0_2l"),
	FIXED_CLK_MM(CLK_MM_DISP_OVL1_2L, "mm_disp_ovl1_2l"),
	FIXED_CLK_MM(CLK_MM_DISP_RDMA0, "mm_disp_rdma0"),
	FIXED_CLK_MM(CLK_MM_DISP_RDMA1, "mm_disp_rdma1"),
	FIXED_CLK_MM(CLK_MM_DISP_RDMA2, "mm_disp_rdma2"),
	FIXED_CLK_MM(CLK_MM_DISP_WDMA0, "mm_disp_wdma0"),
	FIXED_CLK_MM(CLK_MM_DISP_WDMA1, "mm_disp_wdma1"),
	FIXED_CLK_MM(CLK_MM_DISP_COLOR0, "mm_disp_color0"),
	FIXED_CLK_MM(CLK_MM_DISP_COLOR1, "mm_disp_color1"),
	FIXED_CLK_MM(CLK_MM_DISP_CCORR0, "mm_disp_ccorr0"),
	FIXED_CLK_MM(CLK_MM_DISP_CCORR1, "mm_disp_ccorr1"),
	FIXED_CLK_MM(CLK_MM_DISP_AAL0, "mm_disp_aal0"),
	FIXED_CLK_MM(CLK_MM_DISP_AAL1, "mm_disp_aal1"),
	FIXED_CLK_MM(CLK_MM_DISP_GAMMA0, "mm_disp_gamma0"),
	FIXED_CLK_MM(CLK_MM_DISP_GAMMA1, "mm_disp_gamma1"),

	/* MM1 */
	FIXED_CLK_MM(CLK_MM_DISP_OD, "mm_disp_od"),
	FIXED_CLK_MM(CLK_MM_DISP_DITHER0, "mm_disp_dither0"),
	FIXED_CLK_MM(CLK_MM_DISP_DITHER1, "mm_disp_dither1"),
	FIXED_CLK_MM(CLK_MM_DISP_UFOE, "mm_disp_ufoe"),
	FIXED_CLK_MM(CLK_MM_DISP_DSC, "mm_disp_dsc"),
	FIXED_CLK_MM(CLK_MM_DISP_SPLIT, "mm_disp_split"),
	FIXED_CLK_MM(CLK_MM_DISP_OVL0_MOUT, "mm_disp_ovl0_mout"),
};

static const struct mtk_gate mm_gates[] = {
	/* MM0 */
	GATE_MM0(CLK_MM_SMI_COMMON, "mm_smi_common", "mm_sel", 0),
	GATE_MM0(CLK_MM_SMI_LARB0, "mm_smi_larb0", "mm_sel", 1),
	GATE_MM0(CLK_MM_SMI_LARB4, "mm_smi_larb4", "mm_sel", 2),
	GATE_MM0(CLK_MM_FAKE_ENG, "mm_fake_eng", "mm_sel", 14),

	/* MM1 */
	GATE_MM1(CLK_MM_DSI0_ENGINE, "mm_dsi0_engine", "mm_sel", 6),
	GATE_MM1(CLK_MM_DSI0_DIGITAL, "mm_dsi0_digital", "dsi0_dig", 7),
	GATE_MM1(CLK_MM_DSI1_ENGINE, "mm_dsi1_engine", "mm_sel", 8),
	GATE_MM1(CLK_MM_DSI1_DIGITAL, "mm_dsi1_digital", "dsi1_dig", 9),
	GATE_MM1(CLK_MM_DPI_ENGINE, "mm_dpi_engine", "mm_sel", 10),
	GATE_MM1(CLK_MM_DPI_PIXEL, "mm_dpi_pixel", "dpi0_sel", 11),
	GATE_MM1(CLK_MM_LARB4_AXI_ASIF_MM, "mm_larb4_axi_asif_mm", "mm_sel", 12),
	GATE_MM1(CLK_MM_LARB4_AXI_ASIF_MJC, "mm_larb4_axi_asif_mjc", "mm_sel", 13),
	GATE_MM1(CLK_MM_FAKE_ENG2, "mm_fake_eng2", "mm_sel", 15),
	GATE_MM1(CLK_MM_DFP, "mm_dfp", "mm_sel", 16),
	GATE_MM1(CLK_MM_DFP_52M, "mm_dfp_52m", "mm_sel", 17),
};

static const struct mtk_clk_desc mm_desc = {
	.fixed_clks = mm_fixed_clks,
	.num_fixed_clks = ARRAY_SIZE(mm_fixed_clks),
	.clks = mm_gates,
	.num_clks = ARRAY_SIZE(mm_gates),
};

static const struct platform_device_id clk_mt6757_mm_id_table[] = {
	{ .name = "clk-mt6757-mm", .driver_data = (kernel_ulong_t)&mm_desc },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(platform, clk_mt6757_mm_id_table);

static struct platform_driver clk_mt6757_mm_drv = {
	.probe = mtk_clk_pdev_probe,
	.remove = mtk_clk_pdev_remove,
	.driver = {
		.name = "clk-mt6757-mm",
	},
	.id_table = clk_mt6757_mm_id_table,
};

module_platform_driver(clk_mt6757_mm_drv);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 mmsys clock driver");
MODULE_LICENSE("GPL");
