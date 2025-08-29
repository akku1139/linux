// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2015 MediaTek Inc.
 */

#include <linux/init.h>
#include <asm/cpu_ops.h>

#include "mtcmos.h"

extern const struct cpu_operations cpu_psci_ops;

static int __init mt_cpu_psci_cpu_init(unsigned int cpu)
{
	return cpu_psci_ops.cpu_init(cpu);
}

static int __init mt_cpu_psci_cpu_prepare(unsigned int cpu)
{
	if (cpu == 1) mt_spm_mtcmos_init();
	return cpu_psci_ops.cpu_prepare(cpu);
}

static int mt_cpu_psci_cpu_boot(unsigned int cpu)
{
	int ret;
	
	ret = cpu_psci_ops.cpu_boot(cpu);
	if (ret < 0)
		return ret;

	return spm_mtcmos_ctrl_cpu(cpu, STA_POWER_ON, 1);
}

#ifdef CONFIG_HOTPLUG_CPU
static bool mt_cpu_psci_cpu_can_disable(unsigned int cpu)
{
	return cpu_psci_ops.cpu_can_disable(cpu);
}

static int mt_cpu_psci_cpu_disable(unsigned int cpu)
{
	return cpu_psci_ops.cpu_disable(cpu);
}

static void mt_cpu_psci_cpu_die(unsigned int cpu)
{
	return cpu_psci_ops.cpu_die(cpu);
}

static int mt_cpu_psci_cpu_kill(unsigned int cpu)
{
	cpu_psci_ops.cpu_kill(cpu);
	return spm_mtcmos_ctrl_cpu(cpu, STA_POWER_DOWN, 1);
}
#endif

const struct cpu_operations mt_cpu_psci_ops = {
	.name		= "mt-psci",
	.cpu_init	= mt_cpu_psci_cpu_init,
	.cpu_prepare	= mt_cpu_psci_cpu_prepare,
	.cpu_boot	= mt_cpu_psci_cpu_boot,
#ifdef CONFIG_HOTPLUG_CPU
	.cpu_can_disable = mt_cpu_psci_cpu_can_disable,
	.cpu_disable	= mt_cpu_psci_cpu_disable,
	.cpu_die	= mt_cpu_psci_cpu_die,
	.cpu_kill	= mt_cpu_psci_cpu_kill,
#endif
};

