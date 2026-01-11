// SPDX-License-Identifier: GPL-2.0-only
/*
 * ASoC (not really) platform driver for Amazon Dough FPGA found on Echo devices.
 *
 * Copyright (c) 2016 Amazon Lab126
 * Copyright (c) 2026 Ben Grisdale <bengris32@protonmail.ch>
 */

/*
 * NOTE: This is a very stripped down version of the
 * sound/soc/mediatek/mt_soc_audio_8163_amzn/amzn-spi-pcm/amzn-mt-spi-pcm.c
 * driver from downstream.
 * 
 * I spent a bit of time breaking downstream and concluded that this
 * FPGA actually needs to be brought up for both playback and capture
 * to work.
 *
 * So, this driver has just enough code to be able to bring up the firmware
 * on the FPGA and switch it to the correct mode. That is it.
 *
 * In the future, if audio capture is to be implemented, we need to
 * fully implement this driver.
 *
 * Oh and also, there exists different FGPA revisions which are used
 * on some of the newer Echo devices such as cronos, which use a different
 * firmware. The obvious solution would be to just specify the FPGA
 * revision in the device tree, I guess.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/firmware.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/spi/spi.h>

#include "amazon-dough-fpga.h"

struct amazon_dough_fpga_data {
	struct gpio_desc *reset_gpio;
	struct regulator *vccio0;
	struct regulator *vccio2;
	uint8_t revision;
};

static int dough_spi_txrx(struct spi_device *spi, void *txb, void *rxb, int len,
			  bool dma_enable, dma_addr_t paddr)
{
	struct spi_message msg = {};
	struct spi_transfer xfer = {};

	spi_message_init(&msg);

	xfer.tx_buf = txb;
	xfer.rx_buf = rxb;
	xfer.len = len;
	xfer.bits_per_word = 8;

	if (dma_enable)
		xfer.rx_dma = paddr;

	spi_message_add_tail(&xfer, &msg);
	return spi_sync_locked(spi, &msg);
}

static int amazon_dough_fpga_probe(struct spi_device *spi)
{
	int ret;
	char buf[SPI_SETUP_BUF_SIZE] = {0};
	const struct firmware *fw_entry;
	struct amazon_dough_fpga_data *priv;
	struct dough_frame *rx_df;
	size_t bytes;
	void *fw_buf;

	priv = devm_kzalloc(&spi->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return dev_err_probe(&spi->dev, -ENOMEM,
				     "Failed to allocate memory for private data.\n");

	spi_set_drvdata(spi, priv);

	rx_df = devm_kzalloc(&spi->dev, sizeof(struct dough_frame), GFP_KERNEL);
	if (!rx_df)
		return dev_err_probe(&spi->dev, -ENOMEM,
				     "Failed to allocate dough RX buffer.\n");

	fw_buf = devm_kzalloc(&spi->dev, FIRMWARE_MAX_BYTES, GFP_KERNEL);
	if (!fw_buf)
		return dev_err_probe(&spi->dev, -ENOMEM,
				     "Failed to allocate dough firmware buffer.\n");

	/* Get regulators */
	priv->vccio0 = devm_regulator_get(&spi->dev, "vccio0");
	if (IS_ERR(priv->vccio0))
		return dev_err_probe(&spi->dev, PTR_ERR(priv->vccio0),
				     "Failed to get vccio0 regulator.\n");

	priv->vccio2 = devm_regulator_get(&spi->dev, "vccio2");
	if (IS_ERR(priv->vccio2))
		return dev_err_probe(&spi->dev, PTR_ERR(priv->vccio2),
				     "Failed to get vccio2 regulator.\n");

	/* Get FPGA reset pin. */
	priv->reset_gpio = devm_gpiod_get(&spi->dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(priv->reset_gpio))
		return dev_err_probe(&spi->dev, PTR_ERR(priv->reset_gpio),
				     "Failed to get FPGA reset GPIO.\n");

	/* Request the firmware and copy to working buffer */
	ret = request_firmware(&fw_entry, FPGA_FIRMWARE_NAME, &spi->dev);
	if (ret)
		return dev_err_probe(&spi->dev, ret,
				     "Failed to request FPGA firmware.\n");

	bytes = roundup(fw_entry->size, 1024) + 1024;
	if (bytes > FIRMWARE_MAX_BYTES) {
		release_firmware(fw_entry);
		return dev_err_probe(&spi->dev, -EINVAL,
				     "FPGA firmware is too big! (size = %lu, max = %u)",
				     bytes, FIRMWARE_MAX_BYTES);
	}

	memcpy(fw_buf, fw_entry->data, fw_entry->size);
	release_firmware(fw_entry);

	/* Setup SPI configuration */
	spi->mode = SPI_MODE_3;
	spi->bits_per_word = 8;
	spi->rt = true;

	ret = spi_setup(spi);
	if (ret)
		return dev_err_probe(&spi->dev, ret,
				     "SPI setup failed.\n");

	/*
	 * Power up the FPGA. VCCIO0 needs to be switched on
	 * after VCCIO2 after a ~10ms delay.
	 */
	ret = regulator_enable(priv->vccio2);
	if (ret)
		return dev_err_probe(&spi->dev, ret,
				     "Failed to enable vccio2 regulator.\n");

	msleep(FPGA_VCC_DELAY_MS);

	ret = regulator_enable(priv->vccio0);
	if (ret)
		return dev_err_probe(&spi->dev, ret,
				     "Failed to enable vccio0 regulator.\n");

	/*
	 * Load the firmware. Running a small transaction seems to pull chip select
	 * low on the FPGA. This needs to happen while the reset line is toggled in
	 * order to flash a new firmware image.
	 */
	spi_bus_lock(spi->controller);

	/* Put FPGA in default mode */
	buf[0] = dough_fw_off;
	ret = dough_spi_txrx(spi, buf, NULL, SPI_SETUP_BUF_SIZE, 0, 0);
	if (ret < 0) {
		dev_err(&spi->dev, "Failed to put FPGA into default mode.\n");
		goto bus_unlock;
	}

	/* Assert FPGA reset. */
	gpiod_set_value(priv->reset_gpio, 1);
	msleep(FPGA_DELAY_MS);
	gpiod_set_value(priv->reset_gpio, 0);
	msleep(FPGA_DELAY_MS);

	/* Send the firmware to the FPGA and pray. */
	ret = dough_spi_txrx(spi, fw_buf, NULL, bytes, 0, 0);
	if (ret < 0) {
		dev_err(&spi->dev, "Failed to download FGPA firmware.\n");
		goto bus_unlock;
	}

	/* Wait for FPGA firmware to boot. */
	msleep(FPGA_DELAY_MS);

	/* Verify FW is active and reporting correct version */
	ret = dough_spi_txrx(spi, NULL, rx_df,
			     sizeof(struct dough_frame), 0, 0);
	if (ret < 0) {
		dev_err(&spi->dev, "Failed to get FGPA revision.\n");
		goto bus_unlock;
	}

	priv->revision = rx_df->dsf.fpga_rev;
	if (priv->revision < DOUGH_FPGA_REV_MIN ||
	    priv->revision > DOUGH_FPGA_REV_MAX) {
		dev_err(&spi->dev, "FPGA reports bad revision: %d.\n",
			priv->revision);
		ret = -EINVAL;
		goto bus_unlock;
	}

	/* Put FPGA in I2S Audio mode */
	buf[0] = dough_fw_i2s;
	ret = dough_spi_txrx(spi, buf, NULL, SPI_SETUP_BUF_SIZE, 0, 0);
	if (ret < 0) {
		dev_err(&spi->dev, "Failed to set FPGA to I2S audio mode.\n");
		goto bus_unlock;
	}

	/* We are done! */
	dev_info(&spi->dev, "Dough firmware has booted and is configured! (rev = %u)\n",
		 priv->revision);
bus_unlock:
	spi_bus_unlock(spi->controller);
	return ret;
}

static const struct spi_device_id amazon_dough_fpga_spi_ids[] = {
	{ "dough-fpga" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(spi, amazon_dough_fpga_spi_ids);

static const struct of_device_id amazon_dough_fpga_dt_ids[] = {
	{ .compatible = "amazon,dough-fpga" },
	{ /* sentinel */},
};
MODULE_DEVICE_TABLE(of, amazon_dough_fpga_dt_ids);

static struct spi_driver amazon_dough_fpga_driver = {
	.driver = {
		.name  = "amazon-dough-fpga",
		.owner = THIS_MODULE,
		.of_match_table = of_match_ptr(amazon_dough_fpga_dt_ids),
		.bus = &spi_bus_type,
	},
	.id_table = amazon_dough_fpga_spi_ids,
	.probe = amazon_dough_fpga_probe,
};
module_spi_driver(amazon_dough_fpga_driver);

MODULE_DESCRIPTION("ASoC driver for Amazon Dough FPGA");
MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_LICENSE("GPL");
