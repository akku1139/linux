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

#include <linux/mutex.h>
#include <linux/regmap.h>

#include "pwrap_compat.h"

static DEFINE_MUTEX(pmic_access_mutex);
static DEFINE_MUTEX(pmic_lock_mutex);

static struct regmap *pwrap_regmap = NULL;

void pmic_lock(void)
{
	mutex_lock(&pmic_lock_mutex);
}

void pmic_unlock(void)
{
	mutex_unlock(&pmic_lock_mutex);
}

u32 pmic_read_interface(u32 RegNum, u32 *val, u32 MASK, u32 SHIFT)
{
	u32 return_value = 0;
	u32 pmic6323_reg = 0;
	u32 rdata;

	mutex_lock(&pmic_access_mutex);

	if (pwrap_regmap == NULL) {
		WARN(1, "pwrap_regmap == NULL");
		return -1;
	}

	return_value = regmap_read(pwrap_regmap, RegNum, &rdata);
	pmic6323_reg = rdata;
	if (return_value != 0) {
		pr_notice("Reg[%x]= pmic_wrap read data fail\n", RegNum);
		mutex_unlock(&pmic_access_mutex);
		return return_value;
	}

	pmic6323_reg &= (MASK << SHIFT);
	*val = (pmic6323_reg >> SHIFT);

	mutex_unlock(&pmic_access_mutex);

	return return_value;
}

u32 pmic_config_interface(u32 RegNum, u32 val, u32 MASK, u32 SHIFT)
{
	u32 return_value = 0;
	u32 pmic6323_reg = 0;
	u32 rdata;

	mutex_lock(&pmic_access_mutex);

	if (pwrap_regmap == NULL) {
		WARN(1, "pwrap_regmap == NULL");
		return -1;
	}

	return_value = regmap_read(pwrap_regmap, RegNum, &rdata);
	pmic6323_reg = rdata;
	if (return_value != 0) {
		pr_notice("Reg[%x]= pmic_wrap read data fail\n", RegNum);
		mutex_unlock(&pmic_access_mutex);
		return return_value;
	}

	pmic6323_reg &= ~(MASK << SHIFT);
	pmic6323_reg |= (val << SHIFT);

	return_value = regmap_write(pwrap_regmap, RegNum, pmic6323_reg);
	if (return_value != 0) {
		pr_notice("Reg[%x]= pmic_wrap read data fail\n", RegNum);
		mutex_unlock(&pmic_access_mutex);
		return return_value;
	}

	mutex_unlock(&pmic_access_mutex);

	return return_value;
}

void set_pwrap_regmap(struct regmap *r)
{
	mutex_lock(&pmic_access_mutex);
	pwrap_regmap = r;
	mutex_unlock(&pmic_access_mutex);
}
