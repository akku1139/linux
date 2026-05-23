/*
 * Copyright (C) 2018 MediaTek Inc.
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

#ifndef __H_DDP_MET__
#define __H_DDP_MET__

#if 0
void ddp_init_met_tag(int state, int rdma0_mode, int rdma1_mode);
#else
static inline void ddp_init_met_tag(int state, int rdma0_mode,
                    int rdma1_mode)
{
}
#endif
#endif
