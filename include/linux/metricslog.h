/*
 * Copyright 2011-2017 Amazon Technologies, Inc. All Rights Reserved.
 * Portions Copyright (C) 2007-2008 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _LINUX_METRICSLOG_H
#define _LINUX_METRICSLOG_H

#define AUDIO_MINERVA_METRIC_HEADER "0ookao6i:38tk/2/02330400:100"
#define AUDIO_MINERVA_METRIC_PREDEFINED_FIELDS \
	"_buildType=;SY,_countryOfResidence=;SY,_customerId=;SY," \
	"_deviceId=;SY,_deviceLanguage=;SY,_deviceType=;SY,_hardware=;SY," \
	"_marketPlaceId=;SY,_otaGroupName=;SY,_platform=;SY," \
	"_softwareVersion=;SY,_timeZone=;SY"
#define MINERVA_METRIC_TAIL "us-east-1"
#define METRICS_DATA_LEN 512

typedef enum {
	VITALS_NORMAL = 0,
	VITALS_FGTRACKING,
	VITALS_TIME_BUCKET,
} vitals_type;

/*
 * Android log priority values, in ascending priority order.
 */
typedef enum android_log_priority {
	ANDROID_LOG_UNKNOWN = 0,
	ANDROID_LOG_DEFAULT,	/* only for SetMinPriority() */
	ANDROID_LOG_VERBOSE,
	ANDROID_LOG_DEBUG,
	ANDROID_LOG_INFO,
	ANDROID_LOG_WARN,
	ANDROID_LOG_ERROR,
	ANDROID_LOG_FATAL,
	ANDROID_LOG_SILENT,	/* only for SetMinPriority(); must be last */
} android_LogPriority;

#ifdef CONFIG_AMAZON_METRICS_LOG
int log_to_metrics(enum android_log_priority priority,
	const char *domain, char *logmsg);

int log_counter_to_vitals(enum android_log_priority priority,
	const char *domain, const char *program,
	const char *source, const char *key,
	long counter_value, const char *unit,
	const char *metadata, vitals_type type);
int log_timer_to_vitals(enum android_log_priority priority,
	const char *domain, const char *program,
	const char *source, const char *key,
	long timer_value, const char *unit, vitals_type type);

#else
int log_to_metrics(enum android_log_priority priority,
        const char *domain, char *logmsg) { return -1; };

int log_counter_to_vitals(enum android_log_priority priority,
        const char *domain, const char *program,
        const char *source, const char *key,
        long counter_value, const char *unit,
        const char *metadata, vitals_type type) { return -1; };

int log_timer_to_vitals(enum android_log_priority priority,
        const char *domain, const char *program,
        const char *source, const char *key,
        long timer_value, const char *unit, vitals_type type) { return -1; };
#endif /* CONFIG_AMAZON_METRICS_LOG */

#endif /* _LINUX_METRICSLOG_H */
