// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk-provider.h>
#include <linux/platform_device.h>

#include "clk-mtk.h"
#include "clk-gate.h"
#include "clk-mux.h"

#include <dt-bindings/clock/mediatek,mt6757-topckgen.h>

#define CLK_MODE		0x00

#define CLK_CFG_UPDATE		0x04
#define CLK_CFG_UPDATE1		0x08

#define CLK_CFG_0		0x40
#define CLK_CFG_0_SET		0x44
#define CLK_CFG_0_CLR		0x48

#define CLK_CFG_1		0x50
#define CLK_CFG_1_SET		0x54
#define CLK_CFG_1_CLR		0x58

#define CLK_CFG_2		0x60
#define CLK_CFG_2_SET		0x64
#define CLK_CFG_2_CLR		0x68

#define CLK_CFG_3		0x70
#define CLK_CFG_3_SET		0x74
#define CLK_CFG_3_CLR		0x78

#define CLK_CFG_4		0x80
#define CLK_CFG_4_SET		0x84
#define CLK_CFG_4_CLR		0x88

#define CLK_CFG_5		0x90
#define CLK_CFG_5_SET		0x94
#define CLK_CFG_5_CLR		0x98

#define CLK_CFG_6		0xa0
#define CLK_CFG_6_SET		0xa4
#define CLK_CFG_6_CLR		0xa8

#define CLK_CFG_7		0xb0
#define CLK_CFG_7_SET		0xb4
#define CLK_CFG_7_CLR		0xb8

#define CLK_CFG_8		0xc0
#define CLK_CFG_8_SET		0xc4
#define CLK_CFG_8_CLR		0xc8

#define CLK_CFG_9		0xd0
#define CLK_CFG_9_SET		0xd4
#define CLK_CFG_9_CLR		0xd8

#define CLK_MISC_CFG_0		0x104

/*
 * For some clocks, we don't care what their actual rates are. And these
 * clocks may change their rate on different products or different scenarios.
 * So we model these clocks' rate as 0, to denote it's not an actual rate.
 */
#define DUMMY_RATE	0

static DEFINE_SPINLOCK(mt6757_topckgen_lock);

/* Some clocks with unknown details are modeled as fixed clocks */
static const struct mtk_fixed_clk topckgen_fixed_clks[] = {
	FIXED_CLK(CLK_TOP_DMPLL, "dmpll", NULL, 466 * MHZ),
	FIXED_CLK(CLK_TOP_OSCPLL, "oscpll", NULL, 208 * MHZ),
	FIXED_CLK(CLK_TOP_DSI0_DIG, "dsi0_dig", "clk26m", DUMMY_RATE),
	FIXED_CLK(CLK_TOP_DSI1_DIG, "dsi1_dig", "clk26m", DUMMY_RATE),
};

static const struct mtk_fixed_factor topckgen_factors[] = {
	FACTOR(CLK_TOP_SYSPLL_D2, "syspll_d2", "mainpll", 1, 2),
	FACTOR(CLK_TOP_SYSPLL1_D2, "syspll1_d2", "syspll_d2", 1, 2),
	FACTOR(CLK_TOP_SYSPLL1_D4, "syspll1_d4", "syspll_d2", 1, 4),
	FACTOR(CLK_TOP_SYSPLL1_D8, "syspll1_d8", "syspll_d2", 1, 8),
	FACTOR(CLK_TOP_SYSPLL1_D16, "syspll1_d16", "syspll_d2", 1, 16),
	FACTOR(CLK_TOP_SYSPLL_D3, "syspll_d3", "mainpll", 1, 3),
	FACTOR(CLK_TOP_SYSPLL2_D2, "syspll2_d2", "syspll_d3", 1, 2),
	FACTOR(CLK_TOP_SYSPLL2_D4, "syspll2_d4", "syspll_d3", 1, 4),
	FACTOR(CLK_TOP_SYSPLL2_D8, "syspll2_d8", "syspll_d3", 1, 8),
	FACTOR(CLK_TOP_SYSPLL_D3_D3, "syspll_d3_d3", "syspll_d3", 1, 3),
	FACTOR(CLK_TOP_SYSPLL_D5, "syspll_d5", "mainpll", 1, 5),
	FACTOR(CLK_TOP_SYSPLL3_D2, "syspll3_d2", "syspll_d5", 1, 2),
	FACTOR(CLK_TOP_SYSPLL3_D4, "syspll3_d4", "syspll_d5", 1, 4),
	FACTOR(CLK_TOP_SYSPLL_D7, "syspll_d7", "mainpll", 1, 7),
	FACTOR(CLK_TOP_SYSPLL4_D2, "syspll4_d2", "syspll_d7", 1, 2),
	FACTOR(CLK_TOP_SYSPLL4_D4, "syspll4_d4", "syspll_d7", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL, "univpll", "univ2pll", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL_D2, "univpll_d2", "univpll", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL1_D2, "univpll1_d2", "univpll_d2", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL1_D4, "univpll1_d4", "univpll_d2", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL1_D8, "univpll1_d8", "univpll_d2", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D3, "univpll_d3", "univpll", 1, 3),
	FACTOR(CLK_TOP_UNIVPLL2_D2, "univpll2_d2", "univpll_d3", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL2_D4, "univpll2_d4", "univpll_d3", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL2_D8, "univpll2_d8", "univpll_d3", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D5, "univpll_d5", "univpll", 1, 5),
	FACTOR(CLK_TOP_UNIVPLL3_D2, "univpll3_d2", "univpll_d5", 1, 2),
	FACTOR(CLK_TOP_UNIVPLL3_D4, "univpll3_d4", "univpll_d5", 1, 4),
	FACTOR(CLK_TOP_UNIVPLL3_D8, "univpll3_d8", "univpll_d5", 1, 8),
	FACTOR(CLK_TOP_UNIVPLL_D7, "univpll_d7", "univpll", 1, 7),
	FACTOR(CLK_TOP_UNIVPLL_192M, "univpll_192m", "univ2pll", 1, 13),
	FACTOR(CLK_TOP_UNIVPLL_192M_D4, "univpll_192m_d4", "univpll_192m", 1, 4),
	FACTOR(CLK_TOP_TVDPLL_D2, "tvdpll_d2", "tvdpll", 1, 2),
	FACTOR(CLK_TOP_TVDPLL_D4, "tvdpll_d4", "tvdpll", 1, 4),
	FACTOR(CLK_TOP_TVDPLL_D8, "tvdpll_d8", "tvdpll", 1, 8),
	FACTOR(CLK_TOP_TVDPLL_D16, "tvdpll_d16", "tvdpll", 1, 16),
	FACTOR(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", "msdcpll", 1, 2),
	FACTOR(CLK_TOP_MSDCPLL_D4, "msdcpll_d4", "msdcpll", 1, 4),
	FACTOR(CLK_TOP_MSDCPLL_D8, "msdcpll_d8", "msdcpll", 1, 8),
	FACTOR(CLK_TOP_MSDCPLL_D16, "msdcpll_d16", "msdcpll", 1, 16),
	FACTOR(CLK_TOP_OSCPLL_D2, "oscpll_d2", "oscpll", 1, 2),
	FACTOR(CLK_TOP_OSCPLL_D4, "oscpll_d4", "oscpll", 1, 4),
	FACTOR(CLK_TOP_OSCPLL_D8, "oscpll_d8", "oscpll", 1, 8),
};

static const char * const axi_sel_parents[] = {
	"clk26m",
	"syspll1_d4",
	"syspll2_d2",
	"oscpll_d8"
};

static const char * const mem_sel_parents[] = {
	"clk26m",
	"dmpll_ck",
};

static const char * const ddrphycfg_sel_parents[] = {
	"clk26m",
	"syspll1_d8",
};

static const char * const mm_sel_parents[] = {
	"clk26m",
	"syspll2_d2",
	"vencpll",
	"syspll1_d2",
	"univpll1_d2",
	"syspll_d3",
	"syspll_d2",
};

static const char * const vdec_sel_parents[] = {
	"clk26m",
	"vencpll",
	"syspll2_d2",
	"syspll1_d2",
	"syspll1_d4",
	"univpll_d3",
	"syspll_d5",
};

static const char * const mfg_sel_parents[] = {
	"clk26m",
	"mmpll",
	"univpll_d3",
	"syspll_d3",
};

static const char * const f52m_mfg_sel_parents[] = {
	"clk26m",
	"univpll2_d2",
	"univpll2_d4",
	"univpll2_d8",
};

static const char * const camtg_sel_parents[] = {
	"clk26m",
	"univpll_192m_d4",
	"univpll2_d2",
};

static const char * const uart_sel_parents[] = {
	"clk26m",
	"univpll2_d8",
};

static const char * const spi_sel_parents[] = {
	"clk26m",
	"syspll3_d2",
	"syspll2_d4",
	"msdcpll_d4",
};

static const char * const msdc50_0_hclk_sel_parents[] = {
	"clk26m",
	"syspll1_d2",
	"syspll2_d2",
	"syspll4_d2",
};

static const char * const msdc50_0_sel_parents[] = {
	"clk26m",
	"msdcpll",
	"msdcpll_d2",
	"univpll1_d4",
	"syspll2_d2",
	"syspll_d7",
	"msdcpll_d4",
	"univpll_d2",
	"univpll1_d2",
};

static const char * const msdc30_1_sel_parents[] = {
	"clk26m",
	"univpll2_d2",
	"msdcpll_d4",
	"univpll1_d4",
	"syspll2_d2",
	"syspll_d7",
	"univpll_d7",
	"msdcpll_d2",
};

static const char * const msdc30_2_sel_parents[] = {
	"clk26m",
	"univpll2_d2",
	"msdcpll_d4",
	"univpll1_d4",
	"syspll2_d2",
	"syspll_d7",
	"univpll_d7",
	"msdcpll_d2",
};

static const char * const msdc30_3_sel_parents[] = {
	"clk26m",
	"msdcpll_d8",
	"msdcpll_d4",
	"univpll1_d4",
	"univpll_192m_d4",
	"syspll_d7",
	"univpll_d7",
	"syspll3_d4",
	"msdcpll_d16",
};

static const char * const audio_sel_parents[] = {
	"clk26m",
	"syspll3_d4",
	"syspll4_d4",
	"syspll1_d16",
};

static const char * const aud_intbus_sel_parents[] = {
	"clk26m",
	"syspll1_d4",
	"syspll4_d2",
};

static const char * const pmicspi_sel_parents[] = {
	"clk26m",
	"syspll1_d8",
	"oscpll_d8",
};

static const char * const atb_sel_parents[] = {
	"clk26m",
	"syspll1_d2",
	"syspll_d5",
};

static const char * const dpi0_sel_parents[] = {
	"clk26m",
	"tvdpll_d2",
	"tvdpll_d4",
	"tvdpll_d8",
	"tvdpll_d16",
};

static const char * const scam_sel_parents[] = {
	"clk26m",
	"syspll3_d2",
};

static const char * const aud_1_sel_parents[] = {
	"clk26m",
	"apll1",
};

static const char * const aud_2_sel_parents[] = {
	"clk26m",
	"apll2",
};

static const char * const disp_pwm_sel_parents[] = {
	"clk26m",
	"univpll2_d4",
	"oscpll_d4",
	"oscpll_d8",
};

static const char * const pwm_sel_parents[] = {
	"clk26m",
	"univpll2_d2",
	"univpll2_d4",
};

static const char * const ssusb_top_sys_sel_parents[] = {
	"clk26m",
	"univpll3_d2",
};

static const char * const ssusb_top_xhci_sel_parents[] = {
	"clk26m",
	"univpll3_d2",
};

static const char * const usb_top_sel_parents[] = {
	"univpll3_d4",
	"clk26m",
};

static const char * const spm_sel_parents[] = {
	"clk26m",
	"syspll1_d8",
};

static const char * const bsi_spi_sel_parents[] = {
	"clk26m",
	"syspll_d3_d3",
	"syspll1_d4",
	"syspll_d7",
};

static const char * const i2c_sel_parents[] = {
	"clk26m",
	"syspll1_d8",
	"univpll3_d4",
};

static const char * const dvfsp_sel_parents[] = {
	"clk26m",
	"syspll1_d8",
};

static const char * const scp_sel_parents[] = {
	"clk26m",
	"syspll1_d2",
	"syspll1_d4",
};

static const char * const mjc_sel_parents[] = {
	"clk26m",
	"syspll_d5",
	"univpll_d5",
};

static const char * const venc_sel_parents[] = {
	"mm_sel",
};

static const char * const univ_48m_sel_parents[] = {
	"univpll_192m",
	"univpll_192m_d4",
};

static const struct mtk_mux topckgen_muxes[] = {
	/* CLK_CFG_0 */
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_AXI_SEL, "axi_sel",
		axi_sel_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR, 0, 2, 7, CLK_CFG_UPDATE, 0,
		CLK_IS_CRITICAL),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MEM_SEL, "mem_sel",
		mem_sel_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR, 8, 2, 15, CLK_CFG_UPDATE, 1,
		CLK_IS_CRITICAL),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_DDRPHYCFG_SEL, "ddrphycfg_sel",
		ddrphycfg_sel_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR, 16, 1, 23, CLK_CFG_UPDATE, 2,
		CLK_IS_CRITICAL),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MM_SEL, "mm_sel",
		mm_sel_parents, CLK_CFG_0, CLK_CFG_0_SET, CLK_CFG_0_CLR, 24, 3, 31, CLK_CFG_UPDATE, 3),

	/* CLK_CFG_1 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_PWM_SEL, "pwm_sel",
		pwm_sel_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR, 0, 2, 7, 0, 0),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_VDEC_SEL, "vdec_sel",
		vdec_sel_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR, 8, 3, 15, CLK_CFG_UPDATE, 5),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_VENC_SEL, "venc_sel",
		venc_sel_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR, 16, 4, 23, CLK_CFG_UPDATE, 6),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MFG_SEL, "mfg_sel",
		mfg_sel_parents, CLK_CFG_1, CLK_CFG_1_SET, CLK_CFG_1_CLR, 24, 2, 31, CLK_CFG_UPDATE, 7),

	/* CLK_CFG_2 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_CAMTG_SEL, "camtg_sel",
		camtg_sel_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR, 0, 2, 7, CLK_CFG_UPDATE, 8),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_UART_SEL, "uart_sel",
		uart_sel_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR, 8, 1, 15, CLK_CFG_UPDATE, 9),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SPI_SEL, "spi_sel",
		spi_sel_parents, CLK_CFG_2, CLK_CFG_2_SET, CLK_CFG_2_CLR, 16, 2, 23, CLK_CFG_UPDATE, 10),

	/* CLK_CFG_3 */
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MSDC50_0_HCLK_SEL, "msdc50_0_hclk_sel",
		msdc50_0_hclk_sel_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR, 8, 2, 15, CLK_CFG_UPDATE, 12,
		0),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MSDC50_0_SEL, "msdc50_0_sel",
		msdc50_0_sel_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR, 16, 4, 23, CLK_CFG_UPDATE, 13,
		0),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MSDC30_1_SEL, "msdc30_1_sel",
		msdc30_1_sel_parents, CLK_CFG_3, CLK_CFG_3_SET, CLK_CFG_3_CLR, 24, 3, 31, CLK_CFG_UPDATE, 14,
		0),

	/* CLK_CFG_4 */
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MSDC30_2_SEL, "msdc30_2_sel",
		msdc30_2_sel_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR, 0, 3, 7, CLK_CFG_UPDATE, 15,
		0),
	MUX_GATE_CLR_SET_UPD_FLAGS(CLK_TOP_MSDC30_3_SEL, "msdc30_3_sel",
		msdc30_3_sel_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR, 8, 4, 15, CLK_CFG_UPDATE, 16,
		0),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUDIO_SEL, "audio_sel",
		audio_sel_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR, 16, 2, 23, CLK_CFG_UPDATE, 17),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_INTBUS_SEL, "aud_intbus_sel",
		aud_intbus_sel_parents, CLK_CFG_4, CLK_CFG_4_SET, CLK_CFG_4_CLR, 24, 2, 31, CLK_CFG_UPDATE, 18),

	/* CLK_CFG_5 */
	MUX_CLR_SET_UPD(CLK_TOP_PMICSPI_SEL, "pmicspi_sel",
		pmicspi_sel_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR, 0, 2, CLK_CFG_UPDATE, 19),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SCP_SEL, "scp_sel",
		scp_sel_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR, 8, 2, 15, CLK_CFG_UPDATE, 20),
	MUX_CLR_SET_UPD(CLK_TOP_ATB_SEL, "atb_sel",
		atb_sel_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR, 16, 2, CLK_CFG_UPDATE, 21),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_MJC_SEL, "mjc_sel",
		mjc_sel_parents, CLK_CFG_5, CLK_CFG_5_SET, CLK_CFG_5_CLR, 24, 2, 31, CLK_CFG_UPDATE, 22),

	/* CLK_CFG_6 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DPI0_SEL, "dpi0_sel",
		dpi0_sel_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR, 0, 3, 7, CLK_CFG_UPDATE, 23),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SCAM_SEL, "scam_sel",
		scam_sel_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR, 8, 1, 15, CLK_CFG_UPDATE, 24),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_1_SEL, "aud_1_sel",
		aud_1_sel_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR, 16, 1, 23, CLK_CFG_UPDATE, 25),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_AUD_2_SEL, "aud_2_sel",
		aud_2_sel_parents, CLK_CFG_6, CLK_CFG_6_SET, CLK_CFG_6_CLR, 24, 1, 31, CLK_CFG_UPDATE, 26),

	/* CLK_CFG_7 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DISP_PWM_SEL, "disp_pwm_sel",
		disp_pwm_sel_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR, 0, 2, 7, CLK_CFG_UPDATE, 27),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SSUSB_TOP_SYS_SEL, "ssusb_top_sys_sel",
		ssusb_top_sys_sel_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR, 8, 1, 15, CLK_CFG_UPDATE, 28),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_SSUSB_TOP_XHCI_SEL, "ssusb_top_xhci_sel",
		ssusb_top_xhci_sel_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR, 16, 1, 23, CLK_CFG_UPDATE, 29),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_USB_TOP_SEL, "usb_top_sel",
		usb_top_sel_parents, CLK_CFG_7, CLK_CFG_7_SET, CLK_CFG_7_CLR, 24, 1, 31, CLK_CFG_UPDATE, 30),

	/* CLK_CFG_8 */
	MUX_CLR_SET_UPD(CLK_TOP_SPM_SEL, "spm_sel",
		spm_sel_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR, 0, 1, CLK_CFG_UPDATE, 31),
	MUX_CLR_SET_UPD(CLK_TOP_BSI_SPI_SEL, "bsi_spi_sel",
		bsi_spi_sel_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR, 8, 2, CLK_CFG_UPDATE1, 0),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_I2C_SEL, "i2c_sel",
		i2c_sel_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR, 16, 2, 23, CLK_CFG_UPDATE1, 1),
	MUX_GATE_CLR_SET_UPD(CLK_TOP_DVFSP_SEL, "dvfsp_sel",
		dvfsp_sel_parents, CLK_CFG_8, CLK_CFG_8_SET, CLK_CFG_8_CLR, 24, 1, 31, CLK_CFG_UPDATE1, 2),

	/* CLK_CFG_9 */
	MUX_GATE_CLR_SET_UPD(CLK_TOP_F52M_MFG_SEL, "f52m_mfg_sel",
		f52m_mfg_sel_parents, CLK_CFG_9, CLK_CFG_9_SET, CLK_CFG_9_CLR, 0, 2, 7, CLK_CFG_UPDATE, 11),
};

static const struct mtk_composite topckgen_composites[] = {
	/* CLK_MISC_CFG_0 */
	MUX(CLK_TOP_USB20_48M_SEL, "usb20_48m_sel",
		univ_48m_sel_parents, CLK_MISC_CFG_0, 8, 1),
	MUX(CLK_TOP_UNIV_48M_SEL, "univ_48m_sel",
		univ_48m_sel_parents, CLK_MISC_CFG_0, 9, 1),
	MUX(CLK_TOP_SSUSB_48M_SEL, "ssusb_48m_sel",
		univ_48m_sel_parents, CLK_MISC_CFG_0, 10, 1),
};

static const struct mtk_gate_regs cfg_3_cg_regs = {
	.set_ofs = CLK_CFG_3_SET,
	.clr_ofs = CLK_CFG_3_CLR,
	.sta_ofs = CLK_CFG_3,
};

static const struct mtk_gate_regs misc_cfg_cg_regs = {
	.sta_ofs = CLK_MISC_CFG_0,
};

static const struct mtk_gate_regs mode_cg_regs = {
	.sta_ofs = CLK_MODE,
};

#define GATE_TOP_CFG_3(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &cfg_3_cg_regs, _shift, &mtk_clk_gate_ops_setclr)

#define GATE_TOP_MISC(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &misc_cfg_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr_inv)

#define GATE_TOP_MODE(_id, _name, _parent, _shift)				\
	GATE_MTK(_id, _name, _parent, &mode_cg_regs, _shift, &mtk_clk_gate_ops_no_setclr)

static const struct mtk_gate topckgen_gates[] = {
	GATE_TOP_CFG_3(CLK_TOP_IMG, "img", "mm_sel", 7),
	GATE_TOP_MISC(CLK_TOP_ARMPLL_DIV_PLL1, "armpll_div_pll1", "mainpll", 4),
	GATE_TOP_MISC(CLK_TOP_ARMPLL_DIV_PLL2, "armpll_div_pll2", "univpll_d2", 5),
	GATE_TOP_MODE(CLK_TOP_MD_32K, "md_32k", "clk32k", 8),
	GATE_TOP_MODE(CLK_TOP_MD_26M, "md_26m", "clk26m", 9),
	GATE_TOP_MODE(CLK_TOP_CONN_32K, "conn_32k", "clk32k", 10),
	GATE_TOP_MODE(CLK_TOP_CONN_26M, "conn_26m", "clk26m", 11),
};

static const struct mtk_clk_desc topckgen_desc = {
	.composite_clks = topckgen_composites,
	.num_composite_clks = ARRAY_SIZE(topckgen_composites),
	.fixed_clks = topckgen_fixed_clks,
	.num_fixed_clks = ARRAY_SIZE(topckgen_fixed_clks),
	.factor_clks = topckgen_factors,
	.num_factor_clks = ARRAY_SIZE(topckgen_factors),
	.clks = topckgen_gates,
	.num_clks = ARRAY_SIZE(topckgen_gates),
	.mux_clks = topckgen_muxes,
	.num_mux_clks = ARRAY_SIZE(topckgen_muxes),
	.clk_lock = &mt6757_topckgen_lock,
};

static const struct of_device_id of_match_mt6757_topckgen[] = {
	{ .compatible = "mediatek,mt6757-topckgen", .data = &topckgen_desc},
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, of_match_mt6757_topckgen);

static struct platform_driver clk_mt6757_topckgen = {
	.probe = mtk_clk_simple_probe,
	.remove = mtk_clk_simple_remove,
	.driver = {
		.name = "clk-mt6757-topckgen",
		.of_match_table = of_match_mt6757_topckgen,
	},
};
module_platform_driver(clk_mt6757_topckgen);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("MediaTek MT6757 topckgen clock driver");
MODULE_LICENSE("GPL");
