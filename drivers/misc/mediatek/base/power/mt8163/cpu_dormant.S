/*
 * Copyright (C) 2015 MediaTek Inc.
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

        .global write_cntpctl
write_cntpctl:
        msr	CNTP_CTL_EL0, x0
        ret

        .global read_cntpctl
read_cntpctl:
        mrs	x0, CNTP_CTL_EL0
        ret

        .global read_cpu_id
read_cpu_id:
        mrs	x0, MPIDR_EL1
        ands	x0, x0, #0xff
        ret

        .global read_cluster_id
read_cluster_id:
        mrs	x0, MPIDR_EL1
        lsr	x0, x0, #0x8
        ands	x0, x0, #0xff
        ret

	.global mt_save_generic_timer
mt_save_generic_timer:
	mrs	x3, CNTKCTL_EL1
	str	x3, [x0, #0]
	mrs	x2, CNTP_CTL_EL0
	mrs	x3, CNTP_TVAL_EL0
	stp	x2, x3, [x0, #8]
	mrs	x2, CNTV_CTL_EL0
	mrs	x3, CNTV_TVAL_EL0
	stp	x2, x3, [x0, #24]!
        ret

	.global mt_restore_generic_timer
mt_restore_generic_timer:
	ldr	x3, [x0, #0]
	msr	CNTKCTL_EL1, x3
	ldp	x2, x3, [x0, #8]
	msr	CNTP_CTL_EL0, x2
	msr	CNTP_TVAL_EL0, x3
	ldp	x2, x3, [x0, #24]
	msr	CNTV_CTL_EL0, x2
	msr	CNTV_TVAL_EL0, x3
        ret
