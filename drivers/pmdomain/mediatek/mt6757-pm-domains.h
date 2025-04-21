/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#ifndef __SOC_MEDIATEK_MT6757_PM_DOMAINS_H
#define __SOC_MEDIATEK_MT6757_PM_DOMAINS_H

#include "mtk-pm-domains.h"
#include <dt-bindings/power/mediatek,mt6757-power-controller.h>

/*
 * MT6757 power domain support
 */

#define MT6757_BUS_PROT_INFRA_UPDATE_TOPAXI(_mask)		\
	BUS_PROT_UPDATE(INFRA, _mask,				\
			MT6797_INFRA_TOPAXI_PROTECTEN,		\
			MT6797_INFRA_TOPAXI_PROTECTEN,		\
			MT6797_INFRA_TOPAXI_PROTECTSTA1)

#define MT6757_BUS_PROT_INFRA_UPDATE_TOPAXI_1(_mask)		\
	BUS_PROT_UPDATE(INFRA, _mask,				\
			MT6797_INFRA_TOPAXI_PROTECTEN_1,	\
			MT6797_INFRA_TOPAXI_PROTECTEN_1,	\
			MT6797_INFRA_TOPAXI_PROTECTSTA1_1)

static const struct scpsys_domain_data scpsys_domain_data_mt6757[] = {
	[MT6757_POWER_DOMAIN_AUDIO] = {
		.name = "audio",
		.sta_mask = BIT(24),
		.ctl_offs = 0x0314,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(15, 12),
	},
	[MT6757_POWER_DOMAIN_CAM] = {
		.name = "cam",
		.sta_mask = BIT(27),
		.ctl_offs = 0x0344,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(9, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
	},
	[MT6757_POWER_DOMAIN_CONN] = {
		.name = "conn",
		.sta_mask = BIT(1),
		.ctl_offs = 0x032c,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = 0,
		.sram_pdn_ack_bits = 0,
		.bp_cfg = {
			MT6757_BUS_PROT_INFRA_UPDATE_TOPAXI(MT6757_TOP_AXI_PROT_EN_CONN),
		},
	},
	[MT6757_POWER_DOMAIN_DISP] = {
		.name = "disp",
		.sta_mask = BIT(3),
		.ctl_offs = 0x030c,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
		.bp_cfg = {
			MT6757_BUS_PROT_INFRA_UPDATE_TOPAXI(MT6757_TOP_AXI_PROT_EN_DISP),
		},
	},
	[MT6757_POWER_DOMAIN_ISP] = {
		.name = "isp",
		.sta_mask = BIT(5),
		.ctl_offs = 0x0308,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(9, 8),
		.sram_pdn_ack_bits = GENMASK(13, 12),
	},
	[MT6757_POWER_DOMAIN_MFG_ASYNC] = {
		.name = "mfg_async",
		.sta_mask = BIT(23),
		.ctl_offs = 0x0334,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = 0,
		.sram_pdn_ack_bits = 0,
	},
	[MT6757_POWER_DOMAIN_MFG_CORE0] = {
		.name = "mfg_core0",
		.sta_mask = BIT(31),
		.ctl_offs = 0x033c,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(5, 5),
		.sram_pdn_ack_bits = GENMASK(6, 6),
	},
	[MT6757_POWER_DOMAIN_MFG_CORE1] = {
		.name = "mfg_core1",
		.sta_mask = BIT(30),
		.ctl_offs = 0x0340,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(5, 5),
		.sram_pdn_ack_bits = GENMASK(6, 6),
	},
	[MT6757_POWER_DOMAIN_MFG] = {
		.name = "mfg",
		.sta_mask = BIT(4),
		.ctl_offs = 0x0338,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(16, 16),
		.bp_cfg = {
			MT6757_BUS_PROT_INFRA_UPDATE_TOPAXI(MT6757_TOP_AXI_PROT_EN_MFG),
		},
	},
	[MT6757_POWER_DOMAIN_VDE] = {
		.name = "vde",
		.sta_mask = BIT(7),
		.ctl_offs = 0x0300,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(8, 8),
		.sram_pdn_ack_bits = GENMASK(12, 12),
	},
	[MT6757_POWER_DOMAIN_VEN] = {
		.name = "ven",
		.sta_mask = BIT(21),
		.ctl_offs = 0x0304,
		.pwr_sta_offs = SPM_PWR_STATUS_MT6797,
		.pwr_sta2nd_offs = SPM_PWR_STATUS_2ND_MT6797,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(15, 12),
	},
};

static const struct scpsys_soc_data mt6757_scpsys_data = {
	.domains_data = scpsys_domain_data_mt6757,
	.num_domains = ARRAY_SIZE(scpsys_domain_data_mt6757),
};

#endif /* __SOC_MEDIATEK_MT6757_PM_DOMAINS_H */
