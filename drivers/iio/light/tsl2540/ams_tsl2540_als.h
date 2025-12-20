/*
 * Device driver for monitoring ambient light intensity (lux)
 * for device tsl2540.
 *
 * Copyright (c) 2017, ams AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */

#ifndef __AMS_TSL2540_ALS_H
#define __AMS_TSL2540_ALS_H

extern const struct attribute_group tsl2540_als_attr_group;

extern int tsl2540_configure_als_mode(struct tsl2540_chip *chip, u8 state);
extern int tsl2540_get_lux(struct tsl2540_chip *chip);
extern int tsl2540_read_als(struct tsl2540_chip *chip);
extern void tsl2540_report_als(struct tsl2540_chip *chip);

#ifdef CONFIG_AMZN_AMS_ALS
#define ALS_MAX_LUX 400
#define ALS_MIN_LUX 0

static inline int als_get_calibrated_lux(struct tsl2540_chip *chip)
{
	struct tsl2540_i2c_platform_data *pdata = chip->pdata;
	int lux = chip->als_inf.lux;
	int coeff = pdata->lux400_lux;
	int calibrated_lux;

	calibrated_lux = ALS_MAX_LUX * lux / coeff;

	if (calibrated_lux > ALS_MAX_LUX)
		calibrated_lux = ALS_MAX_LUX;

	if (calibrated_lux < ALS_MIN_LUX)
		calibrated_lux = ALS_MIN_LUX;

	return lux;
}
#endif

#endif /* __AMS_TSL2540_ALS_H */
