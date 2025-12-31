// SPDX-License-Identifier: GPL-2.0
/*
 * Driver for MediaTek ISP MCLK clock control.
 * Copyright (c) 2025-2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/clk.h>
#include <linux/clk-provider.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pm_domain.h>
#include <linux/pm_runtime.h>
#include <linux/regmap.h>

#define SENINF_TOP_CTRL		0x0000
#define SENINF12_PCLK_CG	GENMASK(11, 10)
#define SENINF12_PCLK_SEL	GENMASK(9, 8)

#define SENINF_TG1_PH_CNT	0x0200
#define SENINF_TG1_PCEN		BIT(31)
#define SENINF_TG1_ADCLK_EN	BIT(29)
#define SENINF_TG1_PADCLK_INV	BIT(6)
#define SENINF_TG1_CLKFL_POL	BIT(2)
#define SENINF_TG1_TGCLK_SEL	GENMASK(1, 0)

#define SENINF_TG1_SEN_CK	0x0204
#define SENINF_TG1_CLKCNT	GENMASK(21, 16)
#define SENINF_TG1_CLKRS	GENMASK(13, 8)
#define SENINF_TG1_CLKFL	GENMASK(5, 0)

#define SENINF1_CTRL		0x0100
#define SENINF1_EN		BIT(0)

#define SENINF1_MUX_CTRL	0x0120
#define SENINF1_MUX_EN		BIT(31)

#define NUM_GENPD_DEVS		2

static const char *mtk_isp_mclk_genpd_names[NUM_GENPD_DEVS] = {
	"mm",
	"isp"
};

struct mtk_isp_mclk {
	struct clk_hw hw;
	struct clk *clk;
	struct device *dev;
	struct device *pd_devs[NUM_GENPD_DEVS];
	struct device_link *pd_links[NUM_GENPD_DEVS];
	struct regmap *regmap;
};

#define to_mtk_isp_mclk(_hw)	container_of(_hw, struct mtk_isp_mclk, hw)

static int mtk_isp_mclk_prepare(struct clk_hw *hw)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);
	int ret;

	/* Prepare clocks */
	ret = clk_prepare(priv->clk);
	if (ret)
		return ret;

	/* Set PCEN */
	ret = regmap_set_bits(priv->regmap, SENINF_TG1_PH_CNT, SENINF_TG1_PCEN);
	if (ret)
		return ret;

	/* Clear top clock gating of SENINF1/2 parallel sensor clock */
	ret = regmap_clear_bits(priv->regmap, SENINF_TOP_CTRL, SENINF12_PCLK_CG);
	if (ret)
		return ret;

	/* Set SENINF1/2 to MCLK */
	ret = regmap_set_bits(priv->regmap, SENINF_TOP_CTRL, SENINF12_PCLK_SEL);
	if (ret)
		return ret;

	/* Clear SENINF_TG1 rising edge */
	ret = regmap_clear_bits(priv->regmap, SENINF_TG1_SEN_CK, SENINF_TG1_CLKRS);
	if (ret)
		return ret;

	/* Set sensor master clock to CAM_PLL */
	ret = regmap_update_bits(priv->regmap, SENINF_TG1_PH_CNT,
				 SENINF_TG1_TGCLK_SEL, 1 /* CAM_PLL */);
	if (ret)
		return ret;

	/* Disable pixel clock inversion in PAD side */
	ret =  regmap_clear_bits(priv->regmap, SENINF_TG1_PH_CNT, SENINF_TG1_PADCLK_INV);
	if (ret)
		return ret;

	/* Set SENINF1_EN/SENINF1_MUX_EN */
	ret = regmap_set_bits(priv->regmap, SENINF1_MUX_CTRL, SENINF1_MUX_EN);
	if (ret)
		return ret;

	return regmap_set_bits(priv->regmap, SENINF1_CTRL, SENINF1_EN);
}

static void mtk_isp_mclk_unprepare(struct clk_hw *hw)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);

	/* Clear SENINF1_EN/SENINF1_MUX_EN */
	regmap_clear_bits(priv->regmap, SENINF1_CTRL, SENINF1_EN);
	regmap_clear_bits(priv->regmap, SENINF1_MUX_CTRL, SENINF1_MUX_EN);

	regmap_clear_bits(priv->regmap, SENINF_TG1_PH_CNT,
			  SENINF_TG1_TGCLK_SEL);
	regmap_clear_bits(priv->regmap, SENINF_TOP_CTRL,
			  SENINF12_PCLK_SEL);

	/* Set top clock gating of SENINF1/2 parallel sensor clock */
	regmap_set_bits(priv->regmap, SENINF_TOP_CTRL, SENINF12_PCLK_CG);

	/* Clear PCEN */
	regmap_clear_bits(priv->regmap, SENINF_TG1_PH_CNT, SENINF_TG1_PCEN);

	clk_unprepare(priv->clk);
}

static int mtk_isp_mclk_enable(struct clk_hw *hw)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);
	int ret;

	/* Enable clocks */
	ret = clk_enable(priv->clk);
	if (ret)
		return ret;

	/* Enable MCLK output to sensor */
	return regmap_set_bits(priv->regmap, SENINF_TG1_PH_CNT, SENINF_TG1_ADCLK_EN);
}

static void mtk_isp_mclk_disable(struct clk_hw *hw)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);

	/* Disable MCLK output to sensor */
	regmap_clear_bits(priv->regmap, SENINF_TG1_PH_CNT, SENINF_TG1_ADCLK_EN);

	clk_disable(priv->clk);
}

static int mtk_isp_mclk_set_rate(struct clk_hw *hw,
				 unsigned long rate,
				 unsigned long parent_rate)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);
	u32 clkcnt, clkf_edge;

	clkcnt = (parent_rate / rate) - 1;
	clkf_edge = clkcnt > 1 ? ((clkcnt + 1) >> 1) : 1;

	regmap_update_bits(priv->regmap, SENINF_TG1_SEN_CK,
			   SENINF_TG1_CLKCNT | SENINF_TG1_CLKFL,
			   FIELD_PREP(SENINF_TG1_CLKCNT, clkcnt) |
			   FIELD_PREP(SENINF_TG1_CLKFL, clkf_edge));

	regmap_update_bits(priv->regmap, SENINF_TG1_PH_CNT,
			   SENINF_TG1_CLKFL_POL,
			   !(clkcnt & 1) ? SENINF_TG1_CLKFL_POL : 0);

	return 0;
}

static unsigned long mtk_isp_mclk_recalc_rate(struct clk_hw *hw,
					      unsigned long parent_rate)
{
	struct mtk_isp_mclk *priv = to_mtk_isp_mclk(hw);
	u32 reg, clkcnt;

	regmap_read(priv->regmap, SENINF_TG1_SEN_CK, &reg);

	clkcnt = FIELD_GET(SENINF_TG1_CLKCNT, reg);

	return parent_rate / (clkcnt + 1);
}

static long mtk_isp_mclk_round_rate(struct clk_hw *hw,
				    unsigned long rate,
				    unsigned long *parent_rate)
{
	unsigned long div;

	div = *parent_rate / rate;
	if (div < 2)
		div = 2;

	return *parent_rate / div;
}

static const struct clk_ops mtk_isp_mclk_ops = {
	.prepare = mtk_isp_mclk_prepare,
	.unprepare = mtk_isp_mclk_unprepare,
	.enable = mtk_isp_mclk_enable,
	.disable = mtk_isp_mclk_disable,
	.set_rate = mtk_isp_mclk_set_rate,
	.recalc_rate = mtk_isp_mclk_recalc_rate,
	.round_rate = mtk_isp_mclk_round_rate,
};

static const struct regmap_config mtk_isp_mclk_regmap_conf = {
	.reg_bits = 32,
	.val_bits = 32,
	.reg_stride = 4,
	.name = "mtk-isp-mclk",
};

static int mtk_isp_mclk_probe(struct platform_device *pdev)
{
	struct mtk_isp_mclk *priv;
	struct clk_init_data init = { };
	struct clk *clk;
	const char *parent_name;
	void __iomem *base;
	int i, ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	priv->dev = &pdev->dev;
	platform_set_drvdata(pdev, priv);

	base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(base))
		return PTR_ERR(base);

	/*
	 * Setup regmap. The SENTG registers are clocked with the
	 * sen_cam clock so we need to attach it to the regmap.
	 */
	priv->regmap = devm_regmap_init_mmio_clk(&pdev->dev, "sen_cam",
						 base, &mtk_isp_mclk_regmap_conf);
	if (IS_ERR(priv->regmap)) {
		dev_err(&pdev->dev, "Failed to obtain regmap.\n");
		return PTR_ERR(priv->regmap);
	}

	/* Obtain sen_tg clock, providing the source for the divider. */
	priv->clk = devm_clk_get(&pdev->dev, "sen_tg");
	if (IS_ERR(priv->clk))
		return dev_err_probe(&pdev->dev, PTR_ERR(priv->clk),
				     "Failed to get parent clock.\n");
	parent_name = __clk_get_name(priv->clk);

	/*
	 * The ISP block needs both its own power domain and the Multimedia
	 * power domain to be powered in order for register writes to work.
	 * We do our own setup to attach two power domains to our device.
	 */
	for (i = 0; i < NUM_GENPD_DEVS; i++) {
		priv->pd_devs[i] = dev_pm_domain_attach_by_name(&pdev->dev,
								mtk_isp_mclk_genpd_names[i]);
		if (IS_ERR(priv->pd_devs[i]))
			return dev_err_probe(&pdev->dev, PTR_ERR(priv->pd_devs[i]),
					     "Failed to get genpd.\n");

		priv->pd_links[i] = device_link_add(&pdev->dev, priv->pd_devs[i],
						    DL_FLAG_STATELESS |
						    DL_FLAG_PM_RUNTIME |
						    DL_FLAG_RPM_ACTIVE);
		if (IS_ERR(priv->pd_links[i]))
			return dev_err_probe(&pdev->dev, PTR_ERR(priv->pd_links[i]),
					     "Failed to add device link.\n");
	}

	/*
	 * pm_runtime_enable needs to be called before clk register.
	 * That is to make core->rpm_enabled to be true for clock
	 * usage.
	 */
	ret = devm_pm_runtime_enable(&pdev->dev);
	if (ret)
		return dev_err_probe(&pdev->dev, ret,
				     "Failed to enable runtime PM.\n");

	init.name = "isp_mclk";
	init.ops = &mtk_isp_mclk_ops;
	init.flags = 0;
	init.parent_names = &parent_name;
	init.num_parents = 1;

	priv->hw.init = &init;

	pm_runtime_get_sync(&pdev->dev);

	clk = devm_clk_register(&pdev->dev, &priv->hw);
	if (IS_ERR(clk)) {
		dev_err(&pdev->dev, "Failed to register clock.\n");
		return PTR_ERR(clk);
	}

	ret = of_clk_add_provider(pdev->dev.of_node, of_clk_src_simple_get, clk);
	if (ret) {
		dev_err(&pdev->dev, "Failed to register DT clock provider.\n");
		return ret;
	}

	pm_runtime_put_sync(&pdev->dev);
	return 0;
}

static void mtk_isp_mclk_remove(struct platform_device *pdev)
{
	struct mtk_isp_mclk *priv = dev_get_drvdata(&pdev->dev);
	int i;

	of_clk_del_provider(pdev->dev.of_node);

	for (i = 0; i < NUM_GENPD_DEVS; i++) {
		device_link_del(priv->pd_links[i]);
		dev_pm_domain_detach(priv->pd_devs[i], false);
	}
}

static const struct of_device_id mtk_isp_mclk_of_match[] = {
	{ .compatible = "mediatek,isp-mclk" },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, mtk_isp_mclk_of_match);

static struct platform_driver mtk_isp_mclk_driver = {
	.probe = mtk_isp_mclk_probe,
	.remove = mtk_isp_mclk_remove,
	.driver = {
		.name = "mtk-isp-mclk",
		.of_match_table = mtk_isp_mclk_of_match,
	},
};
module_platform_driver(mtk_isp_mclk_driver);

MODULE_DESCRIPTION("MediaTek ISP MCLK provider driver");
MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_LICENSE("GPL");
