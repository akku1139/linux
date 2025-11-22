// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/backlight.h>
#include <linux/delay.h>
#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/regulator/consumer.h>

#include <drm/drm_mipi_dsi.h>
#include <drm/drm_probe_helper.h>
#include <drm/drm_modes.h>
#include <drm/drm_panel.h>

#define PANEL_INNOLUX 0
#define PANEL_TRULY 1

struct sony_ft8607f_truly {
	struct drm_panel panel;
	struct mipi_dsi_device *dsi;
	struct gpio_desc *reset_gpio;
	struct gpio_desc *id_gpio;
	struct regulator_bulk_data *supplies;
};

static const struct regulator_bulk_data sony_ft8607f_truly_supplies[] = {
	{ .supply = "vddio" },
	{ .supply = "vsp" },
	{ .supply = "vsn" },
};

static inline
struct sony_ft8607f_truly *to_sony_ft8607f_truly(struct drm_panel *panel)
{
	return container_of(panel, struct sony_ft8607f_truly, panel);
}

static void sony_ft8607f_truly_reset(struct sony_ft8607f_truly *ctx)
{
	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);
	gpiod_set_value_cansleep(ctx->reset_gpio, 0);
	usleep_range(21000, 22000);
}

static int sony_ft8607f_truly_on(struct sony_ft8607f_truly *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	mipi_dsi_dcs_exit_sleep_mode_multi(&dsi_ctx);
	mipi_dsi_msleep(&dsi_ctx, 120);
	mipi_dsi_dcs_set_display_on_multi(&dsi_ctx);

	return dsi_ctx.accum_err;
}

static int sony_ft8607f_truly_off(struct sony_ft8607f_truly *ctx)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	struct mipi_dsi_multi_context dsi_ctx = { .dsi = dsi };

	mipi_dsi_dcs_set_display_off_multi(&dsi_ctx);
	mipi_dsi_dcs_enter_sleep_mode_multi(&dsi_ctx);

	return dsi_ctx.accum_err;
}

static int sony_ft8607f_truly_prepare(struct drm_panel *panel)
{
	struct sony_ft8607f_truly *ctx = to_sony_ft8607f_truly(panel);
	int ret;

	ret = regulator_bulk_enable(ARRAY_SIZE(sony_ft8607f_truly_supplies),
								ctx->supplies);
	if (ret < 0) {
		dev_err(panel->dev, "Failed to enable regulators: %d\n", ret);
		return ret;
	}

	sony_ft8607f_truly_reset(ctx);

	ret = sony_ft8607f_truly_on(ctx);
	if (ret < 0) {
		dev_err(panel->dev, "Failed to turn on display: %d\n", ret);
		goto disable_regulators;
	}

	return 0;

disable_regulators:
	regulator_bulk_disable(ARRAY_SIZE(sony_ft8607f_truly_supplies),
						   ctx->supplies);
	return ret;
}

static int sony_ft8607f_truly_unprepare(struct drm_panel *panel)
{
	struct sony_ft8607f_truly *ctx = to_sony_ft8607f_truly(panel);
	int ret;

	ret = sony_ft8607f_truly_off(ctx);
	if (ret < 0) {
		dev_err(panel->dev, "Failed to turn off display: %d\n", ret);
		return ret;
	}

	gpiod_set_value_cansleep(ctx->reset_gpio, 1);
	usleep_range(2000, 3000);

	regulator_bulk_disable(ARRAY_SIZE(sony_ft8607f_truly_supplies),
						   ctx->supplies);

	return 0;
}

static const struct drm_display_mode sony_ft8607f_truly_innolux_mode = {
    .clock = (720 + 50 + 4 + 100) * (1280 + 60 + 4 + 40) * 60 / 1000,
    .hdisplay = 720,
    .hsync_start = 720 + 50,
    .hsync_end = 720 + 50 + 4,
    .htotal = 720 + 50 + 4 + 100,
    .vdisplay = 1280,
    .vsync_start = 1280 + 60,
    .vsync_end = 1280 + 60 + 4,
    .vtotal = 1280 + 60 + 4 + 40,
	.width_mm = 62,
	.height_mm = 110,
    .type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
};

static const struct drm_display_mode sony_ft8607f_truly_truly_mode = {
    .clock = (720 + 50 + 8 + 92) * (1280 + 20 + 4 + 36) * 60 / 1000,
    .hdisplay = 720,
    .hsync_start = 720 + 50,
    .hsync_end = 720 + 50 + 8,
    .htotal = 720 + 50 + 8 + 92,
    .vdisplay = 1280,
    .vsync_start = 1280 + 20,
    .vsync_end = 1280 + 20 + 4,
    .vtotal = 1280 + 20 + 4 + 36,
	.width_mm = 62,
	.height_mm = 110,
    .type = DRM_MODE_TYPE_DRIVER | DRM_MODE_TYPE_PREFERRED,
};

static int sony_ft8607f_truly_get_modes(struct drm_panel *panel,
				struct drm_connector *connector)
{
	const struct drm_display_mode *mode;
	struct sony_ft8607f_truly *ctx = to_sony_ft8607f_truly(panel);
	int id;

	id = gpiod_get_value_cansleep(ctx->id_gpio);
	if (id < 0) {
		dev_err(panel->dev, "Failed to read panel ID: %d\n", id);
		return id;
	}

	dev_info(panel->dev, "Detected panel ID: %d\n", id);

	if (id == PANEL_INNOLUX) {
		mode = &sony_ft8607f_truly_innolux_mode;
	} else if (id == PANEL_TRULY) {
		mode = &sony_ft8607f_truly_truly_mode;
	}

	return drm_connector_helper_get_modes_fixed(connector, mode);
}

static const struct drm_panel_funcs sony_ft8607f_truly_panel_funcs = {
	.prepare = sony_ft8607f_truly_prepare,
	.unprepare = sony_ft8607f_truly_unprepare,
	.get_modes = sony_ft8607f_truly_get_modes,
};

static int sony_ft8607f_truly_probe(struct mipi_dsi_device *dsi)
{
	struct device *dev = &dsi->dev;
	struct sony_ft8607f_truly *ctx;
	int ret;

	ctx = devm_kzalloc(dev, sizeof(*ctx), GFP_KERNEL);
	if (!ctx)
		return -ENOMEM;

	ctx->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_HIGH);
	if (IS_ERR(ctx->reset_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->reset_gpio),
					 "Failed to get reset GPIO\n");

	ctx->id_gpio = devm_gpiod_get(dev, "id", GPIOD_IN);
	if (IS_ERR(ctx->id_gpio))
		return dev_err_probe(dev, PTR_ERR(ctx->id_gpio),
					 "Failed to get ID GPIO\n");

	ret = devm_regulator_bulk_get_const(dev,
					 ARRAY_SIZE(sony_ft8607f_truly_supplies),
					 sony_ft8607f_truly_supplies, &ctx->supplies);
	if (ret < 0)
		return dev_err_probe(dev, ret,
					 "Failed to get regulators\n");

	ctx->dsi = dsi;
	mipi_dsi_set_drvdata(dsi, ctx);

	dsi->lanes = 4;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_CLOCK_NON_CONTINUOUS;

	drm_panel_init(&ctx->panel, dev, &sony_ft8607f_truly_panel_funcs,
			   DRM_MODE_CONNECTOR_DSI);
	ctx->panel.prepare_prev_first = true;

	ret = drm_panel_of_backlight(&ctx->panel);
	if (ret)
		return dev_err_probe(dev, ret, "Failed to get backlight\n");

	drm_panel_add(&ctx->panel);

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		drm_panel_remove(&ctx->panel);
		return dev_err_probe(dev, ret, "Failed to attach to DSI host\n");
	}

	return 0;
}

static void sony_ft8607f_truly_remove(struct mipi_dsi_device *dsi)
{
	struct sony_ft8607f_truly *ctx = mipi_dsi_get_drvdata(dsi);
	int ret;

	ret = mipi_dsi_detach(dsi);
	if (ret < 0)
		dev_err(&dsi->dev, "Failed to detach from DSI host: %d\n", ret);

	drm_panel_remove(&ctx->panel);
}

static const struct of_device_id sony_ft8607f_truly_of_match[] = {
	{ .compatible = "sony,ft8607f-dsi-vdo-truly" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, sony_ft8607f_truly_of_match);

static struct mipi_dsi_driver sony_ft8607f_truly_driver = {
	.probe = sony_ft8607f_truly_probe,
	.remove = sony_ft8607f_truly_remove,
	.driver = {
		.name = "panel-sony-ft8607f-dsi-vdo-truly",
		.of_match_table = sony_ft8607f_truly_of_match,
	},
};
module_mipi_dsi_driver(sony_ft8607f_truly_driver);

MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_DESCRIPTION("DRM Driver for sony-ft8607f-dsi-vdo-truly");
MODULE_LICENSE("GPL");
