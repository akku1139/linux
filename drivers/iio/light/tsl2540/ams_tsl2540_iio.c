// SPDX-License-Identifier: GPL-2.0-only
/*
 * IIO compatibility layer for TSL2540 driver.
 * Copyright (c) 2025-2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/i2c.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/string.h>

#include <linux/iio/iio.h>

#include "ams_tsl2540.h"
#include "ams_i2c.h"
#include "ams_tsl2540_als.h"
#include "ams_tsl2540_iio.h"

struct tsl2540_indio_data {
	struct tsl2540_chip *chip;
};

static const struct iio_chan_spec tsl2540_channels[] = {
	{
		/* IR */
		.type = IIO_INTENSITY,
		.modified = 1,
		.channel2 = IIO_MOD_LIGHT_IR,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
	},
	{
		/* Light */
		.type = IIO_LIGHT,
		.indexed = 1,
		.channel = 0,
		.info_mask_separate = BIT(IIO_CHAN_INFO_RAW) |
				      BIT(IIO_CHAN_INFO_PROCESSED),
	}
};

static int tsl2540_update_readings(struct tsl2540_chip *chip)
{
	int ret;

	AMS_MUTEX_LOCK(&chip->lock);

	ret = tsl2540_read_als(chip);
		if (ret)
			goto out;

	ret = tsl2540_get_lux(chip);
out:
	AMS_MUTEX_UNLOCK(&chip->lock);
	return ret;
}

static int tsl2540_read_raw(struct iio_dev *indio_dev,
			    struct iio_chan_spec const *chan, int *val,
			    int *val2, long mask)
{
	struct tsl2540_indio_data *data = iio_priv(indio_dev);
	int ret;

	/* update readings from sensor before reporting */
	ret = tsl2540_update_readings(data->chip);
	if (ret)
		return ret;

	switch (mask) {
	case IIO_CHAN_INFO_RAW:
		switch (chan->type) {
		case IIO_LIGHT:
			*val = data->chip->als_inf.als_ch0;
			return IIO_VAL_INT;
		case IIO_INTENSITY:
			*val = data->chip->als_inf.als_ch1;
			return IIO_VAL_INT;
		default:
			return -EINVAL;
		}
	case IIO_CHAN_INFO_PROCESSED:
		switch (chan->type) {
		case IIO_LIGHT:
#ifdef CONFIG_AMZN_AMS_ALS
			/* get the calibrated lux value */
			*val = als_get_calibrated_lux(data->chip);
#else
			*val = data->chip->als_inf.als_ch0;
#endif
			return IIO_VAL_INT;
		default:
			return -EINVAL;
		}
	default:
		return -EINVAL;
	}
}

static const struct iio_info tsl2540_device_info = {
	.read_raw = &tsl2540_read_raw,
};

int tsl2540_initialize_iio(struct tsl2540_chip *chip, const char *name)
{
	struct tsl2540_indio_data *iio_data;
	struct device *dev = &chip->client->dev;
	int ret;

	/* The indio_dev will hold a pointer to the tsl2540_chip */
	chip->indio_dev = devm_iio_device_alloc(dev, sizeof(*iio_data));
	iio_data = iio_priv(chip->indio_dev);
	iio_data->chip = chip;

	/* setup indio dev */
	chip->indio_dev->name = name;
	chip->indio_dev->modes = INDIO_DIRECT_MODE;
	chip->indio_dev->info = &tsl2540_device_info;
	chip->indio_dev->channels = tsl2540_channels;
	chip->indio_dev->num_channels = ARRAY_SIZE(tsl2540_channels);

	ret = devm_iio_device_register(dev, chip->indio_dev);
	if (ret) {
		dev_err(dev, "%s: failed to register indio device: %d.\n",
			    __func__, ret);
		return ret;
	}

	return 0;
}
