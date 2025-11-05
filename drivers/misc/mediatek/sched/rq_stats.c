/*
 * Copyright (C) 2017 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/cpu.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/notifier.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/cpufreq.h>
#include <linux/kernel_stat.h>
#include <linux/tick.h>
#include <linux/suspend.h>
#include <linux/version.h>
#include <linux/math64.h>
#include <asm/smp_plat.h>

#include <trace/events/sched.h>

#define DEFAULT_RQ_POLL_JIFFIES 1
#define DEFAULT_DEF_TIMER_JIFFIES 5
#define MCPS_UPDATE_TIME 1000000 /* 1 sec */

struct rq_data {
	unsigned int rq_avg;
	unsigned long rq_poll_jiffies;
	unsigned long def_timer_jiffies;
	unsigned long rq_poll_last_jiffy;
	unsigned long rq_poll_total_jiffies;
	unsigned long def_timer_last_jiffy;
	unsigned int def_interval;
	unsigned int hotplug_disabled;
	int64_t def_start_time;
	struct attribute_group *attr_group;
	struct kobject *kobj;
	struct work_struct def_timer_work;
	int init;
};

struct rq_data rq_info;

/* set to 1 if per cpu load is cpu freq. variant */
static int cpufreq_variant = 1;
#ifdef CONFIG_CPU_FREQ
struct notifier_block freq_transition;
#endif /* CONFIG_CPU_FREQ */
struct notifier_block cpu_hotplug;

struct cpu_load_data {
	cputime64_t prev_cpu_idle;
	cputime64_t prev_cpu_wall;
	cputime64_t prev_cpu_iowait;
	cputime64_t mcps_base_time;
	u64 cpu_cycles;
	u64 inc_time;
	u64 inc_mcps;
	unsigned int last_mcps;
	unsigned int hw_max_freq;
	unsigned int avg_load_maxfreq_rel;
	unsigned int avg_load_maxfreq_abs;
	unsigned int samples;
	unsigned int window_size;
	unsigned int cur_freq;
	unsigned int policy_max;
	ktime_t last_update;
	cpumask_var_t related_cpus;
	spinlock_t cpu_load_lock;
};

enum AVG_LOAD_ID {
	AVG_LOAD_IGNORE,
	AVG_LOAD_UPDATE
};

static DEFINE_PER_CPU(struct cpu_load_data, cpuload);
static DEFINE_PER_CPU(struct cpufreq_policy, cpupolicy);

static inline cputime64_t get_cpu_iowait_time(unsigned int cpu,
							cputime64_t *wall)
{
	u64 iowait_time = get_cpu_iowait_time_us(cpu, wall);

	if (iowait_time == -1ULL)
		return 0;

	return iowait_time;
}

static int
update_average_load(enum AVG_LOAD_ID id, unsigned int freq, unsigned int cpu)
{

	struct cpu_load_data *pcpu = &per_cpu(cpuload, cpu);
	cputime64_t cur_wall_time, cur_idle_time, cur_iowait_time;
	unsigned int idle_time, wall_time, iowait_time;
	unsigned int cur_load, prev_avg_load_rel, prev_avg_load_abs;
	unsigned int load_at_max_freq_rel, load_at_max_freq_abs;
	cputime64_t prev_wall_time, prev_cpu_idle, prev_cpu_iowait;
	cputime64_t delta_time;
	u64 cpu_cycles;

#ifdef CONFIG_CPU_FREQ
	cur_idle_time = get_cpu_idle_time(cpu, &cur_wall_time, 0);
#else  /* !CONFIG_CPU_FREQ  */
	cur_idle_time = get_cpu_idle_time_internal(cpu, &cur_wall_time);
#endif /* CONFIG_CPU_FREQ */
	cur_iowait_time = get_cpu_iowait_time(cpu, &cur_wall_time);

	prev_wall_time = pcpu->prev_cpu_wall;
	prev_cpu_idle = pcpu->prev_cpu_idle;
	prev_cpu_iowait = pcpu->prev_cpu_iowait;

	wall_time = (unsigned int) (cur_wall_time - pcpu->prev_cpu_wall);
	pcpu->prev_cpu_wall = cur_wall_time;

	idle_time = (unsigned int) (cur_idle_time - pcpu->prev_cpu_idle);
	pcpu->prev_cpu_idle = cur_idle_time;

	iowait_time = (unsigned int) (cur_iowait_time - pcpu->prev_cpu_iowait);
	pcpu->prev_cpu_iowait = cur_iowait_time;

	if (idle_time >= iowait_time)
		idle_time -= iowait_time;

	delta_time = cur_wall_time - pcpu->mcps_base_time;
	if (unlikely(!wall_time || wall_time < idle_time)) {
		/* Correct MCPS Computing */
		if (delta_time >= MCPS_UPDATE_TIME) {
			pcpu->last_mcps = (unsigned int)
				div64_u64(pcpu->cpu_cycles, delta_time);
			pcpu->inc_time = cur_wall_time; /*us*/
			pcpu->inc_mcps += pcpu->last_mcps;
			pcpu->mcps_base_time = cur_wall_time;
			pcpu->cpu_cycles = 0;
		}
		return 0;
	}

	/* Compute MCPS */
	if (pcpu->mcps_base_time) {
		cpu_cycles = ((wall_time - idle_time) * (freq/1000));
		pcpu->cpu_cycles += cpu_cycles;
	}
	if (delta_time >= MCPS_UPDATE_TIME) {
		pcpu->last_mcps = (unsigned int)
			div64_u64(pcpu->cpu_cycles, delta_time);
		pcpu->inc_time = cur_wall_time; /*us*/
		pcpu->inc_mcps += pcpu->last_mcps;
		pcpu->mcps_base_time = 0;
	}
	if (!pcpu->mcps_base_time) {
		pcpu->mcps_base_time = cur_wall_time;
		pcpu->cpu_cycles = 0;
	}

	if (id == AVG_LOAD_IGNORE)
		cur_load = 0;
	else
		cur_load = 100 * (wall_time - idle_time) / wall_time;

	/* Calculate the scaled load across CPU */
	if (cpu_online(cpu)) {
		load_at_max_freq_abs = (cur_load * freq) / pcpu->policy_max;
		load_at_max_freq_rel = cur_load;
	} else {
		load_at_max_freq_abs = 0;
		load_at_max_freq_rel = 0;
	}

	prev_avg_load_rel = pcpu->avg_load_maxfreq_rel;
	prev_avg_load_abs = pcpu->avg_load_maxfreq_abs;

	if (!pcpu->avg_load_maxfreq_rel || !pcpu->avg_load_maxfreq_abs) {
		/* This is the first sample in this window*/
		pcpu->avg_load_maxfreq_rel = load_at_max_freq_rel;
		pcpu->avg_load_maxfreq_abs = load_at_max_freq_abs;
		pcpu->window_size = wall_time;
	} else {
		/*
		 * The is already a sample available in this window.
		 * Compute weighted average with prev entry, so that we get
		 * the precise weighted load.
		 */
		pcpu->avg_load_maxfreq_rel =
		    ((pcpu->avg_load_maxfreq_rel * pcpu->window_size) +
			(load_at_max_freq_rel * wall_time)) /
			(wall_time + pcpu->window_size);
		pcpu->avg_load_maxfreq_abs =
		    ((pcpu->avg_load_maxfreq_abs * pcpu->window_size) +
			(load_at_max_freq_abs * wall_time)) /
			(wall_time + pcpu->window_size);

		pcpu->window_size += wall_time;
	}

	return 0;
}

void sched_get_percpu_load2(int cpu, bool reset, unsigned int *rel_load,
				unsigned int *abs_load)
{
	struct cpu_load_data *pcpu;
	unsigned long flags;

	if (!rel_load || !abs_load)
		return;

	*rel_load = 0;
	*abs_load = 0;
	if (rq_info.init != 1) {
		*rel_load = 90;
		*abs_load = 90;
		return;
	}

	pcpu = &per_cpu(cpuload, cpu);
	spin_lock_irqsave(&pcpu->cpu_load_lock, flags);
	update_average_load(AVG_LOAD_UPDATE, pcpu->cur_freq, cpu);
	*rel_load = pcpu->avg_load_maxfreq_rel;
	*abs_load = pcpu->avg_load_maxfreq_abs;
	if (reset) {
		pcpu->avg_load_maxfreq_rel = 0;
		pcpu->avg_load_maxfreq_abs = 0;
	}
	spin_unlock_irqrestore(&pcpu->cpu_load_lock, flags);
}
EXPORT_SYMBOL(sched_get_percpu_load2);

unsigned int sched_get_percpu_load(int cpu, bool reset, bool use_maxfreq)
{
	int rel_load, abs_load;

	sched_get_percpu_load2(cpu, reset, &rel_load, &abs_load);
	return use_maxfreq ? abs_load : rel_load;
}
EXPORT_SYMBOL(sched_get_percpu_load);


#ifdef CONFIG_CPU_FREQ
static int cpufreq_transition_handler(struct notifier_block *nb,
			unsigned long val, void *data)
{
	struct cpufreq_freqs *freqs = data;
	struct cpu_load_data *this_cpu = &per_cpu(cpuload, freqs->cpu);
	int j;
	unsigned long flags;

	if (rq_info.init != 1)
		return 0;

	switch (val) {
	case CPUFREQ_POSTCHANGE:
		for_each_cpu(j, this_cpu->related_cpus) {
			struct cpu_load_data *pcpu = &per_cpu(cpuload, j);
			/* flush previous laod */
			if (pcpu->policy_max == 1) {
				struct cpufreq_policy cpu_policy;

				if (!cpufreq_get_policy(&cpu_policy, j)) {
					spin_lock_irqsave(&pcpu->cpu_load_lock,
					flags);
					pcpu->policy_max =
					cpu_policy.cpuinfo.max_freq;
					spin_unlock_irqrestore
					(&pcpu->cpu_load_lock, flags);
				}
			}

			spin_lock_irqsave(&pcpu->cpu_load_lock, flags);
				if (cpu_online(j))
					update_average_load(AVG_LOAD_UPDATE,
						freqs->old,
						freqs->cpu);
			pcpu->cur_freq = cpufreq_variant ? freqs->new :
				pcpu->policy_max;
			spin_unlock_irqrestore(&pcpu->cpu_load_lock, flags);
		}
		break;
	}
	return 0;
}
#endif /* CONFIG_CPU_FREQ */

static int cpu_hotplug_handler(struct notifier_block *nb,
			unsigned long val, void *data)
{
	unsigned int cpu = (unsigned long)data;
	struct cpu_load_data *this_cpu = &per_cpu(cpuload, cpu);
	unsigned long flags;
	unsigned int i;

	if (rq_info.init != 1)
		return NOTIFY_OK;

	switch (val) {
	case CPU_ONLINE:
		if (!this_cpu->cur_freq)
			this_cpu->cur_freq = cpufreq_quick_get(cpu);
		for_each_cpu(i, cpu_online_mask) {
			struct cpu_load_data *cld = &per_cpu(cpuload, i);
			struct cpufreq_policy *cpu_policy  =
				&per_cpu(cpupolicy, i);

			cpufreq_get_policy(cpu_policy, i);
			cpumask_copy(cld->related_cpus, cpu_policy->cpus);
		}
		/* cpu_online()=0 here, count cpu offline period as idle */
		spin_lock_irqsave(&this_cpu->cpu_load_lock, flags);
		update_average_load(AVG_LOAD_IGNORE, 0, cpu);
		spin_unlock_irqrestore(&this_cpu->cpu_load_lock, flags);
		break;
	case CPU_ONLINE_FROZEN:
	case CPU_UP_PREPARE:
		/* cpu_online()=0 here, count cpu offline period as idle */
		spin_lock_irqsave(&this_cpu->cpu_load_lock, flags);
		update_average_load(AVG_LOAD_IGNORE, 0, cpu);
		spin_unlock_irqrestore(&this_cpu->cpu_load_lock, flags);
//#ifdef CONFIG_MTK_SCHED_RQAVG_KS
//		/* clear per_cpu variables for heavy task if needed */
//		if (val == CPU_UP_PREPARE)
//			WARN_ON(reset_heavy_task_stats(cpu));
//#endif
		break;
	case CPU_DOWN_PREPARE:
		/* cpu_online()=1 here, flush previous load */
		spin_lock_irqsave(&this_cpu->cpu_load_lock, flags);
		update_average_load(AVG_LOAD_UPDATE, this_cpu->cur_freq, cpu);
		spin_unlock_irqrestore(&this_cpu->cpu_load_lock, flags);
		break;
	}
	return NOTIFY_OK;
}

static int system_suspend_handler(struct notifier_block *nb,
				unsigned long val, void *data)
{
	switch (val) {
	case PM_POST_HIBERNATION:
	case PM_POST_SUSPEND:
	case PM_POST_RESTORE:
		rq_info.hotplug_disabled = 0;
		break;
	case PM_HIBERNATION_PREPARE:
	case PM_SUSPEND_PREPARE:
		rq_info.hotplug_disabled = 1;
		break;
	default:
		return NOTIFY_DONE;
	}
	return NOTIFY_OK;
}


static int __init rq_stats_init(void)
{
	int i;
#ifdef CONFIG_CPU_FREQ
	struct cpufreq_policy *cpu_policy;

	cpu_policy = kmalloc(sizeof(struct cpufreq_policy), GFP_KERNEL);
	if (!cpu_policy) {
		rq_info.init = 0;
		return -ENODATA;
	}
#endif
	/* Bail out if this is not an SMP Target */
#ifndef CONFIG_SMP
#ifdef CONFIG_CPU_FREQ
	kfree(cpu_policy);
#endif
	rq_info.init = 0;
	return -ENODATA;
#endif

#ifdef CONFIG_MTK_SCHED_RQAVG_US_ENABLE_WQ
	rq_wq = create_singlethread_workqueue("rq_stats");

	if (!rq_wq)
		return -ENODATA;

	INIT_WORK(&rq_info.def_timer_work, def_work_fn);
#endif

	rq_info.rq_poll_jiffies = DEFAULT_RQ_POLL_JIFFIES;
	rq_info.def_timer_jiffies = DEFAULT_DEF_TIMER_JIFFIES;
	rq_info.rq_poll_last_jiffy = 0;
	rq_info.def_timer_last_jiffy = 0;
	rq_info.hotplug_disabled = 0;

	for_each_possible_cpu(i) {
		struct cpu_load_data *pcpu = &per_cpu(cpuload, i);

		spin_lock_init(&pcpu->cpu_load_lock);
		pcpu->cur_freq = pcpu->policy_max = pcpu->hw_max_freq = 1;
#ifdef CONFIG_CPU_FREQ
		if (!cpufreq_get_policy(cpu_policy, i)) {
			pcpu->policy_max = cpu_policy->cpuinfo.max_freq;
			pcpu->hw_max_freq = cpu_policy->cpuinfo.max_freq / 1000;
			if (cpu_online(i))
				pcpu->cur_freq = cpufreq_quick_get(i);
			cpumask_copy(pcpu->related_cpus, cpu_policy->cpus);
		}
#endif /* CONFIG_CPU_FREQ */
		if (!cpufreq_variant)
			pcpu->cur_freq = pcpu->policy_max;
	}
#ifdef CONFIG_CPU_FREQ
	freq_transition.notifier_call = cpufreq_transition_handler;
	cpufreq_register_notifier(&freq_transition,
		CPUFREQ_TRANSITION_NOTIFIER);
#endif /* CONFIG_CPU_FREQ */
	cpu_hotplug.notifier_call = cpu_hotplug_handler;
	register_hotcpu_notifier(&cpu_hotplug);

	rq_info.init = 1;
#ifdef CONFIG_CPU_FREQ
	kfree(cpu_policy);
#endif

	return 0;
}
late_initcall(rq_stats_init);

static int __init rq_stats_early_init(void)
{

	/* Bail out if this is not an SMP Target */
#ifndef CONFIG_SMP
		rq_info.init = 0;
		return -ENODATA;
#endif

	pm_notifier(system_suspend_handler, 0);
	return 0;
}
core_initcall(rq_stats_early_init);
