// SPDX-License-Identifier: GPL-2.0
/*
 * ASoC Machine driver for Amazon Echo devices with TLV320AIC32X4
 * audio codec.
 *
 * Copyright (c) 2025-2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#include <linux/gpio/consumer.h>
#include <linux/module.h>
#include <sound/soc.h>
#include <sound/pcm_params.h>
#include <sound/jack.h>

#include "../common/mtk-afe-platform-driver.h"
#include "mt8163-afe-common.h"

#define TI_CODEC_DAI	"tlv320aic32x4-hifi"

#define MCLKFS_RATIO		512

/*
 * TODO: For biscuit, it's most likely possible to control
 * the MCLK rate dynamically to achieve sample rates other
 * than 48000.
 */
#define BISCUIT_MCLK_RATE	9600000

enum machine_type {
	MACHINE_BISCUIT = 0,
	MACHINE_ROOK,
	MACHINE_NUM,
};

struct mt8163_tlv320aic32x4_data {
	struct gpio_desc *accdet_gpio;
	struct gpio_desc *dacmux_gpio;
	struct gpio_desc *extamp_gpio;
	enum machine_type machine;
};

static const char* mt8163_tlv320aic32x4_card_names[MACHINE_NUM] = {
	[MACHINE_BISCUIT] = "biscuit-aic32x4",
	[MACHINE_ROOK] = "rook-aic32x4",
};

static struct snd_soc_jack mt8163_tlv320aic32x4_jack;

static struct snd_soc_jack_gpio mt8163_tlv320aic32x4_jack_gpios[] = {
	{
		.name = "accdet-gpio",
		.report = SND_JACK_HEADPHONE | SND_JACK_LINEOUT,
		.invert = 0,
		.debounce_time = 200,
	},
};

static struct snd_soc_jack_pin mt8163_tlv320aic32x4_jack_pins[] = {
	{
		.pin	= "Headphone",
		.mask	= SND_JACK_HEADPHONE,
	},
};

static const unsigned int mt8163_tlv320aic32x4_sampling_rates[] = { 48000 };

static const struct snd_pcm_hw_constraint_list mt8163_tlv320aic32x4_constraints_rates = {
	.count = ARRAY_SIZE(mt8163_tlv320aic32x4_sampling_rates),
	.list = mt8163_tlv320aic32x4_sampling_rates,
	.mask = 0,
};

static int mt8163_tlv320aic32x4_fe_startup(struct snd_pcm_substream *substream)
{
	return snd_pcm_hw_constraint_list(substream->runtime, 0,
					  SNDRV_PCM_HW_PARAM_RATE,
					  &mt8163_tlv320aic32x4_constraints_rates);
}

static const struct snd_soc_ops mt8163_tlv320aic32x4_fe_ops = {
	.startup = mt8163_tlv320aic32x4_fe_startup,
};

static int mt8163_tlv320aic32x4_hw_params(struct snd_pcm_substream *substream,
					  struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = snd_soc_substream_to_rtd(substream);
	struct snd_soc_dai *cpu_dai = snd_soc_rtd_to_cpu(rtd, 0);
	struct snd_soc_dai *codec_dai = snd_soc_rtd_to_codec(rtd, 0);
	struct mt8163_tlv320aic32x4_data *data = snd_soc_card_get_drvdata(rtd->card);
	int ret;

	/*
	 * Biscuit uses the MCLK source provided by the ISP for
	 * some strange reason, while Rook is more normal and
	 * instead uses the AFE's MCLK generation instead.
	 *
	 * While we dynamically select the clock rate based on
	 * the sampling rate for Rook, we run the MCLK on biscuit
	 * at a fixed rate and set the rate on the codec dai instead
	 * of the cpu dai since the isp_mclk clock is specified
	 * as the mclk clock to the tlv320aic32x4 driver.
	 */
	if (data->machine == MACHINE_ROOK) {
		ret = snd_soc_dai_set_sysclk(cpu_dai, 0,
					     params_rate(params) * MCLKFS_RATIO,
					     SND_SOC_CLOCK_OUT);
		if (ret)
			return ret;
	} else if (data->machine == MACHINE_BISCUIT) {
		ret = snd_soc_dai_set_sysclk(codec_dai, 0,
					    BISCUIT_MCLK_RATE, SND_SOC_CLOCK_OUT);
		if (ret)
			return ret;
	}

	return 0;
}

static const struct snd_soc_ops mt8163_tlv320aic32x4_ops = {
	.hw_params = mt8163_tlv320aic32x4_hw_params,
};

static int mt8163_tlv320aic32x4_init(struct snd_soc_pcm_runtime *runtime)
{
	int ret;
	struct snd_soc_card *card = runtime->card;
	struct mt8163_tlv320aic32x4_data *data = snd_soc_card_get_drvdata(card);

	if (data->accdet_gpio) {
		/* enable jack detection */
		ret = snd_soc_card_jack_new_pins(card, "Headphone", SND_JACK_HEADPHONE,
						 &mt8163_tlv320aic32x4_jack,
						 mt8163_tlv320aic32x4_jack_pins,
						 ARRAY_SIZE(mt8163_tlv320aic32x4_jack_pins));
		if (ret) {
			dev_err(card->dev, "Can't create a new Jack %d\n", ret);
			return ret;
		}
		mt8163_tlv320aic32x4_jack_gpios[0].desc = data->accdet_gpio;
		ret = snd_soc_jack_add_gpios(&mt8163_tlv320aic32x4_jack,
					     ARRAY_SIZE(mt8163_tlv320aic32x4_jack_gpios),
					     mt8163_tlv320aic32x4_jack_gpios);
		return ret;
	}

	return 0;
}

static int mt8163_tlv320aic32x4_dacmux_event(struct snd_soc_dapm_widget *w,
					     struct snd_kcontrol *k, int event)
{
	struct mt8163_tlv320aic32x4_data *data = snd_soc_card_get_drvdata(w->dapm->card);

	switch (event) {
		case SND_SOC_DAPM_POST_PMU:
			return gpiod_set_value(data->dacmux_gpio, 1);
		case SND_SOC_DAPM_POST_PMD:
			return gpiod_set_value(data->dacmux_gpio, 0);
		default:
			return 0;
	}
}

static int mt8163_tlv320aic32x4_extamp_event(struct snd_soc_dapm_widget *w,
					     struct snd_kcontrol *k, int event)
{
	struct mt8163_tlv320aic32x4_data *data = snd_soc_card_get_drvdata(w->dapm->card);

	switch (event) {
		case SND_SOC_DAPM_POST_PMU:
			return gpiod_set_value(data->extamp_gpio, 1);
		case SND_SOC_DAPM_POST_PMD:
			return gpiod_set_value(data->extamp_gpio, 0);
		default:
			return 0;
	}
}

static const struct snd_kcontrol_new mt8163_tlv320aic32x4_controls[] = {
	SOC_DAPM_PIN_SWITCH("Speaker"),
	SOC_DAPM_PIN_SWITCH("Headphone"),
};

static const struct snd_soc_dapm_widget mt8163_tlv320aic32x4_widgets[] = {
	SND_SOC_DAPM_SPK("Speaker", NULL),
	SND_SOC_DAPM_HP("Headphone", NULL),
	SND_SOC_DAPM_PGA_E("DAC MUX", SND_SOC_NOPM, 0, 0, NULL, 0,
			   mt8163_tlv320aic32x4_dacmux_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
	SND_SOC_DAPM_PGA_E("EXT AMP", SND_SOC_NOPM, 0, 0, NULL, 0,
			   mt8163_tlv320aic32x4_extamp_event,
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_POST_PMD),
};

static const struct snd_soc_dapm_route mt8163_tlv320aic32x4_routes[] = {
	{ "Headphone", NULL, "DAC MUX" },
	{ "DAC MUX", NULL, "HPL" },
	{ "DAC MUX", NULL, "HPR" },

	{ "Speaker", NULL, "EXT AMP" },
	{ "EXT AMP", NULL, "HPR" },
};

SND_SOC_DAILINK_DEFS(playback,
	DAILINK_COMP_ARRAY(COMP_CPU("DL1")),
	DAILINK_COMP_ARRAY(COMP_DUMMY()),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

SND_SOC_DAILINK_DEFS(codec,
	DAILINK_COMP_ARRAY(COMP_CPU("I2S1")),
	DAILINK_COMP_ARRAY(COMP_CODEC(NULL, TI_CODEC_DAI)),
	DAILINK_COMP_ARRAY(COMP_EMPTY()));

/* Digital audio interface glue - connects codec <---> CPU */
static struct snd_soc_dai_link mt8163_tlv320aic32x4_dais[] = {
	/* Front End DAI links */
	{
		.name = "TLV320AIC32X4 Playback",
		.stream_name = "TLV320AIC32X4 Playback",
		.trigger = {SND_SOC_DPCM_TRIGGER_PRE,
			    SND_SOC_DPCM_TRIGGER_PRE},
		.dynamic = 1,
		.playback_only = 1,
		.ops = &mt8163_tlv320aic32x4_fe_ops,
		SND_SOC_DAILINK_REG(playback),
	},
	/* Back End DAI links */
	{
		.name = "Codec",
		.no_pcm = 1,
		.playback_only = 1,
		.ignore_suspend = 1,
		.init = mt8163_tlv320aic32x4_init,
		.ops = &mt8163_tlv320aic32x4_ops,
		.dai_fmt = SND_SOC_DAIFMT_I2S | SND_SOC_DAIFMT_NB_NF |
			   SND_SOC_DAIFMT_CBC_CFC,
		SND_SOC_DAILINK_REG(codec),
	},
};

static struct snd_soc_card mt8163_tlv320aic32x4_card = {
	.name = "mt8163-tlv320aic32x4",
	.owner = THIS_MODULE,
	.dai_link = mt8163_tlv320aic32x4_dais,
	.num_links = ARRAY_SIZE(mt8163_tlv320aic32x4_dais),
	.controls = mt8163_tlv320aic32x4_controls,
	.num_controls = ARRAY_SIZE(mt8163_tlv320aic32x4_controls),
	.dapm_widgets = mt8163_tlv320aic32x4_widgets,
	.num_dapm_widgets = ARRAY_SIZE(mt8163_tlv320aic32x4_widgets),
	.dapm_routes = mt8163_tlv320aic32x4_routes,
	.num_dapm_routes = ARRAY_SIZE(mt8163_tlv320aic32x4_routes),
};

static int mt8163_tlv320aic32x4_dev_probe(struct platform_device *pdev)
{
	struct device_node *codec_node, *platform_node;
	struct mt8163_tlv320aic32x4_data *priv;
	struct snd_soc_card *card = &mt8163_tlv320aic32x4_card;
	struct snd_soc_dai_link *dai_link;
	int ret, i;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	platform_node = of_parse_phandle(pdev->dev.of_node,
					 "mediatek,platform", 0);
	if (!platform_node) {
		dev_err(&pdev->dev, "Property 'platform' missing or invalid\n");
		return -EINVAL;
	}
	for_each_card_prelinks(card, i, dai_link) {
		if (!dai_link->platforms->name)
			dai_link->platforms->of_node = platform_node;
	}

	codec_node = of_parse_phandle(pdev->dev.of_node,
				      "mediatek,audio-codec", 0);
	if (!codec_node) {
		dev_err(&pdev->dev,
			"Property 'audio-codec' missing or invalid\n");
		ret = -EINVAL;
		goto put_platform_node;
	}
	for_each_card_prelinks(card, i, dai_link) {
		if (dai_link->codecs->name)
			continue;
		dai_link->codecs->of_node = codec_node;
	}

	card->dev = &pdev->dev;

	snd_soc_card_set_drvdata(card, priv);

	priv->machine = (enum machine_type)of_device_get_match_data(&pdev->dev);
	card->name = mt8163_tlv320aic32x4_card_names[priv->machine];

	priv->dacmux_gpio = devm_gpiod_get(&pdev->dev, "dacmux",
					   GPIOD_OUT_LOW);
	if (IS_ERR(priv->dacmux_gpio)) {
		ret = PTR_ERR(priv->dacmux_gpio);
		dev_err(&pdev->dev, "Failed to get DAC MUX GPIO: %d.\n",
			ret);
		goto put_codec_node;
	}

	priv->extamp_gpio = devm_gpiod_get(&pdev->dev, "extamp",
					   GPIOD_OUT_LOW);
	if (IS_ERR(priv->extamp_gpio)) {
		ret = PTR_ERR(priv->extamp_gpio);
		dev_err(&pdev->dev, "Failed to get EXT AMP GPIO: %d.\n",
			ret);
		goto put_codec_node;
	}

	/* Accessory detection GPIO is optional */
	priv->accdet_gpio = devm_gpiod_get_optional(&pdev->dev, "accdet",
						    GPIOD_IN);
	if (IS_ERR(priv->accdet_gpio)) {
		ret = PTR_ERR(priv->accdet_gpio);
		dev_err(&pdev->dev, "Failed to get ACCDET GPIO: %d.\n",
			ret);
		goto put_codec_node;
	}

	ret = devm_snd_soc_register_card(&pdev->dev, card);
	if (ret) {
		dev_err(&pdev->dev, "Register card failed with :%d.\n", ret);
		goto put_codec_node;
	}

put_codec_node:
	of_node_put(codec_node);
put_platform_node:
	of_node_put(platform_node);
	return ret;
}

/* Machines supported by this driver. */
static const struct of_device_id mt8163_tlv320aic32x4_dt_match[] = {
	{ .compatible = "amazon,mt8163-biscuit-tlv320aic32x4",
	  .data = (void *)MACHINE_BISCUIT },
	{ .compatible = "amazon,mt8163-rook-tlv320aic32x4",
	  .data = (void *)MACHINE_ROOK },
	{ /* sentintel */ }
};
MODULE_DEVICE_TABLE(of, mt8163_tlv320aic32x4_dt_match);

static struct platform_driver mt8163_tlv320aic32x4_driver = {
	.driver = {
		   .name = "mt8163-tlv320aic32x4",
		   .of_match_table = mt8163_tlv320aic32x4_dt_match,
		   .pm = &snd_soc_pm_ops,
	},
	.probe = mt8163_tlv320aic32x4_dev_probe,
};

module_platform_driver(mt8163_tlv320aic32x4_driver);

/* Module information */
MODULE_DESCRIPTION("MT8163 TLV320AIC32X4 ALSA SoC machine driver");
MODULE_AUTHOR("Ben Grisdale <bengris32@protonmail.ch>");
MODULE_LICENSE("GPL v2");
