/*
 * Copyright (C) 2020 MediaTek Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#ifndef __MDP_BASE_H__
#define __MDP_BASE_H__

#define MDP_HW_CHECK

#include "cmdq_device.h"

static u32 mdp_engine_port[ENGBASE_COUNT] = {
	0,	/*ENGBASE_MMSYS_CONFIG*/
	0,	/*ENGBASE_MDP_RDMA0*/
	0,	/*ENGBASE_MDP_RSZ0*/
	0,	/*ENGBASE_MDP_RSZ1*/
	0,	/*ENGBASE_MDP_WDMA*/
	0,	/*ENGBASE_MDP_WROT0*/
	0,	/*ENGBASE_MDP_TDSHP0*/
	0,	/*ENGBASE_MMSYS_MUTEX*/
	0,	/*ENGBASE_MMSYS_CMDQ*/
	0,	/*ENGBASE_CAM_CTRL*/
};

static u32 mdp_base[] = {
	[ENGBASE_MMSYS_CONFIG] = MMSYS_CONFIG_BASE_PA,
	[ENGBASE_MDP_RDMA0] = MDP_RDMA0_BASE_PA,
	[ENGBASE_MDP_RSZ0] = MDP_RSZ0_BASE_PA,
	[ENGBASE_MDP_RSZ1] = MDP_RSZ1_BASE_PA,
	[ENGBASE_MDP_WDMA] = MDP_WDMA_BASE_PA,
	[ENGBASE_MDP_WROT0] = MDP_WROT0_BASE_PA,
	[ENGBASE_MDP_TDSHP0] = MDP_TDSHP0_BASE_PA,
	[ENGBASE_MMSYS_MUTEX] = MM_MUTEX_BASE_PA,
	[ENGBASE_MMSYS_CMDQ] = 0x10212000,
	[ENGBASE_CAM_CTRL] = 0x15004000,
};

#endif
