/*
 * rt5616e.c  --  RT5616e ALSA SoC audio codec driver
 *
 * Copyright 2020 Realtek Semiconductor Corp.
 * Author: Jack Yu <jack.yu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "rl6231.h"
#include "rt5616e.h"

static const struct reg_default rt5616e_reg[] = {
	{ 0x0000, 0x0000 },
	{ 0x0002, 0xc8c8 },
	{ 0x0003, 0xc8c8 },
	{ 0x0005, 0x0000 },
	{ 0x000d, 0x0000 },
	{ 0x000f, 0x0808 },
	{ 0x0019, 0xffff },
	{ 0x0027, 0x6060 },
	{ 0x0029, 0x8080 },
	{ 0x002a, 0x4242 },
	{ 0x0031, 0x001b },
	{ 0x0032, 0x000b },
	{ 0x003b, 0x0000 },
	{ 0x003c, 0x006f },
	{ 0x0045, 0xa000 },
	{ 0x0053, 0xf000 },
	{ 0x0061, 0x0000 },
	{ 0x0063, 0x00c6 },
	{ 0x0064, 0x0000 },
	{ 0x0065, 0x0000 },
	{ 0x0066, 0x0000 },
	{ 0x0067, 0x0200 },
	{ 0x0081, 0x2f02 },
	{ 0x0082, 0x0800 },
	{ 0x0084, 0xe400 },
	{ 0x008b, 0x0600 },
	{ 0x008e, 0x0004 },
	{ 0x008f, 0x1100 },
	{ 0x0090, 0x0646 },
	{ 0x0091, 0x0c00 },
	{ 0x0093, 0x2000 },
	{ 0x00bb, 0x0000 },
	{ 0x00bc, 0x0000 },
	{ 0x00bd, 0x0000 },
	{ 0x00be, 0x0000 },
	{ 0x00bf, 0x0000 },
	{ 0x00d6, 0x0400 },
	{ 0x00d9, 0x0809 },
	{ 0x00da, 0x0000 },
	{ 0x00fa, 0x0090 },
	{ 0x00fb, 0xffff },
	{ 0x00fc, 0x0000 },
	{ 0x00fd, 0x0000 },
	{ 0x00fe, 0x10ec },
	{ 0x00ff, 0x6680 },
	{ 0x0112, 0x0aaa },
	{ 0x0113, 0x0a88 },
	{ 0x0114, 0xa80a },
	{ 0x0115, 0xaa80 },
	{ 0x0116, 0x5000 },
	{ 0x0117, 0x0000 },
	{ 0x0124, 0x0420 },
	{ 0x0125, 0x7530 },
	{ 0x0131, 0x7774 },
	{ 0x0137, 0x7c00 },
	{ 0x0138, 0x39a1 },
	{ 0x013a, 0x0000 },
	{ 0x013d, 0xa800 },
	{ 0x013e, 0x1f00 },
	{ 0x0141, 0x0300 },
	{ 0x0142, 0x0000 },
	{ 0x0143, 0x0000 },
	{ 0x0144, 0x0000 },
	{ 0x0145, 0x0000 },
	{ 0x01fb, 0xffff },
	{ 0x01fc, 0x0000 },
	{ 0x0200, 0x0000 },
	{ 0x0201, 0x0000 },
	{ 0x0202, 0x0000 },
	{ 0x0203, 0x0000 },
	{ 0x0204, 0x0000 },
	{ 0x0205, 0x0000 },
	{ 0x0206, 0x0000 },
	{ 0x0207, 0x0000 },
	{ 0x0208, 0x0600 },
	{ 0x0209, 0x0000 },
	{ 0x020a, 0x0000 },
	{ 0x0300, 0x0000 },
	{ 0x0301, 0x0000 },
	{ 0x0302, 0x0000 },
	{ 0x0303, 0x5540 },
	{ 0x0304, 0x0055 },
	{ 0x0305, 0x0000 },
	{ 0x0306, 0x0000 },
	{ 0x0307, 0x0000 },
	{ 0x0308, 0x0000 },
	{ 0x0800, 0x0200 },
	{ 0x0801, 0x0000 },
	{ 0x0802, 0x0000 },
	{ 0x0e00, 0x0001 },
	{ 0x0e01, 0x000a },
	{ 0x0e02, 0x0000 },
	{ 0x0e03, 0x002f },
	{ 0x0e04, 0x002f },
	{ 0x0e05, 0x00f3 },
	{ 0x0e06, 0x0000 },
	{ 0x0e07, 0x0023 },
	{ 0x0e08, 0x0000 },
	{ 0x1800, 0x0005 },
	{ 0x1801, 0x2000 },
	{ 0x1802, 0x00ff },
	{ 0x2200, 0x0038 },
	{ 0x3000, 0x1f00 },
	{ 0x3001, 0x4040 },
	{ 0x3002, 0x0000 },
	{ 0x3003, 0x0000 },
	{ 0x3004, 0x0000 },
	{ 0x3005, 0x0000 },
	{ 0x3006, 0x0000 },
	{ 0x3007, 0x0000 },
	{ 0x3008, 0x0000 },
	{ 0x3100, 0x0000 },
	{ 0x3101, 0x3300 },
	{ 0x3102, 0x3300 },
};

struct rt5616e_priv {
	struct snd_soc_codec *codec;
	struct delayed_work patch_work;
	struct regmap *regmap;
	struct clk *mclk;

	int sysclk;
	int sysclk_src;
	int lrck[RT5616e_AIFS];
	int bclk[RT5616e_AIFS];
	int master[RT5616e_AIFS];

	int pll_src;
	int pll_in;
	int pll_out;
	int hw_ver;
};

static bool rt5616e_volatile_register(struct device *dev, unsigned int reg)
{

	switch (reg) {
	case RT5616e_RESET:
	case RT5616e_INT_IRQ_ST:
	case RT5616e_VERSION_ID:
	case RT5616e_VENDOR_ID:
	case RT5616e_DEVICE_ID:
	case RT5616e_HP_OFFSET_CALI1:
	case RT5616e_HP_OFFSET_CALI2:
	case RT5616e_HP_OFFSET_CALI4:
		return true;
	default:
		return false;
	}
}

static bool rt5616e_readable_register(struct device *dev, unsigned int reg)
{

	switch (reg) {
	case RT5616e_RESET:
	case RT5616e_VERSION_ID:
	case RT5616e_VENDOR_ID:
	case RT5616e_DEVICE_ID:
	case RT5616e_HP_VOL:
	case RT5616e_LOUT_CTRL1:
	case RT5616e_LOUT_CTRL2:
	case RT5616e_IN1_IN2:
	case RT5616e_DAC1_DIG_VOL:
	case RT5616e_STO1_ADC_MIXER:
	case RT5616e_AD_DA_MIXER:
	case RT5616e_STO_DAC_MIXER:
	case RT5616e_PDM_I2C_CTL1:
	case RT5616e_PDM_I2C_CTL2:
	case RT5616e_REC_L1_MIXER:
	case RT5616e_REC_L2_MIXER:
	case RT5616e_HPO_MIXER:
	case RT5616e_LOUT_MIXER:
	case RT5616e_PWR_DIG1:
	case RT5616e_PWR_ANLG1:
	case RT5616e_PWR_ANLG2:
	case RT5616e_PWR_MIXER:
	case RT5616e_PWR_VOL:
	case RT5616e_PWR_LDO_DACREF:
	case RT5616e_PLL_CTRL1:
	case RT5616e_PLL_CTRL2:
	case RT5616e_ADC_LPF_CTRL:
	case RT5616e_HP_OVCD:
	case RT5616e_DEPOP_M1:
	case RT5616e_DEPOP_M2:
	case RT5616e_DEPOP_M3:
	case RT5616e_CHARGE_PUMP:
	case RT5616e_MICBIAS:
	case RT5616e_JD_CTRL1:
	case RT5616e_JD_CTRL2:
	case RT5616e_IRQ_CTRL1:
	case RT5616e_IRQ_CTRL2:
	case RT5616e_INT_IRQ_ST:
	case RT5616e_HP_CALIB_AMP_DET:
	case RT5616e_SV_ZCD1:
	case RT5616e_SV_ZCD2:
	case RT5616e_D_MISC:
	case RT5616e_DUMMY1:
	case RT5616e_DUMMY2:
	case RT5616e_BIAS_CUR1:
	case RT5616e_BIAS_CUR2:
	case RT5616e_BIAS_CUR3:
	case RT5616e_BIAS_CUR4:
	case RT5616e_BIAS_CUR5:
	case RT5616e_VERF_ADJ:
	case RT5616e_CP_INT1:
	case RT5616e_CP_INT2:
	case RT5616e_JDH1:
	case RT5616e_SOFT_DEPOP:
	case RT5616e_PLL_INT:
	case RT5616e_TEST_CTL1:
	case RT5616e_CHOP_DAC:
	case RT5616e_SOFT_DCC:
	case RT5616e_HP_DC_OST1:
	case RT5616e_HP_DC_OST2:
	case RT5616e_HP_DC_OST3:
	case RT5616e_HP_DC_OST4:
	case RT5616e_HP_DC_OST5:
	case RT5616e_DUMMY3:
	case RT5616e_DUMMY4:
	case RT5616e_SYS_CLK:
	case RT5616e_PLL_CTRL:
	case RT5616e_RC_CTRL:
	case RT5616e_OSR_CTRL:
	case RT5616e_FTR_CLK_CTRL:
	case RT5616e_TRACK_CTRL:
	case RT5616e_FIFO_CTRL:
	case RT5616e_I2S_CLK_CTRL:
	case RT5616e_IP_CTRL:
	case RT5616e_GFM_CTRL1:
	case RT5616e_GFM_CTRL2:
	case RT5616e_GPIO_HL_CTRL1:
	case RT5616e_GPIO_HL_CTRL2:
	case RT5616e_GPIO_CTRL1:
	case RT5616e_GPIO_DRV_CTRL1:
	case RT5616e_GPIO_DRV_CTRL2:
	case RT5616e_GPIO_TEST1:
	case RT5616e_GPIO_TEST2:
	case RT5616e_GPIO_TEST3:
	case RT5616e_GPIO_TEST4:
	case RT5616e_ASRC_CTRL1:
	case RT5616e_ASRC_CTRL2:
	case RT5616e_ASRC_CTRL3:
	case RT5616e_ADC_ADJ_CTRL1:
	case RT5616e_ADC_ADJ_CTRL2:
	case RT5616e_ADC_ADJ_CTRL3:
	case RT5616e_ADC_ADJ_CTRL4:
	case RT5616e_ADC_ADJ_CTRL5:
	case RT5616e_ADC_ADJ_CTRL6:
	case RT5616e_ADC_ADJ_CTRL7:
	case RT5616e_ADC_ADJ_CTRL8:
	case RT5616e_ADC_ADJ_CTRL9:
	case RT5616e_DA_FTR_CTRL1:
	case RT5616e_DA_FTR_CTRL2:
	case RT5616e_DA_FTR_CTRL3:
	case RT5616e_GPIO_CTRL2:
	case RT5616e_HP_OFFSET_CALI1:
	case RT5616e_HP_OFFSET_CALI2:
	case RT5616e_HP_OFFSET_CALI3:
	case RT5616e_HP_OFFSET_CALI4:
	case RT5616e_HP_OFFSET_CALI5:
	case RT5616e_HP_OFFSET_CALI6:
	case RT5616e_HP_OFFSET_CALI7:
	case RT5616e_HP_OFFSET_CALI8:
	case RT5616e_HP_OFFSET_CALI9:
	case RT5616e_HP_AMP_DET1:
	case RT5616e_HP_AMP_DET2:
	case RT5616e_HP_AMP_DET3:
		return true;
	default:
		return false;
	}
}

static const DECLARE_TLV_DB_SCALE(out_vol_tlv, -4650, 150, 0);
static const DECLARE_TLV_DB_SCALE(dac_vol_tlv, -95625, 375, 0);
static const DECLARE_TLV_DB_SCALE(in_vol_tlv, -3450, 150, 0);
static const DECLARE_TLV_DB_SCALE(adc_vol_tlv, -17625, 375, 0);
static const DECLARE_TLV_DB_SCALE(adc_bst_tlv, 0, 1200, 0);

/* {0, +20, +24, +30, +35, +40, +44, +50, +52} dB */
static unsigned int bst_tlv[] = {
	TLV_DB_RANGE_HEAD(7),
	0, 0, TLV_DB_SCALE_ITEM(0, 0, 0),
	1, 1, TLV_DB_SCALE_ITEM(2000, 0, 0),
	2, 2, TLV_DB_SCALE_ITEM(2400, 0, 0),
	3, 5, TLV_DB_SCALE_ITEM(3000, 500, 0),
	6, 6, TLV_DB_SCALE_ITEM(4400, 0, 0),
	7, 7, TLV_DB_SCALE_ITEM(5000, 0, 0),
	8, 8, TLV_DB_SCALE_ITEM(5200, 0, 0),
};

static const struct snd_kcontrol_new rt5616e_snd_controls[] = {
	SOC_DOUBLE_TLV("HP Playback Volume", RT5616e_HP_VOL,
		       RT5616e_L_VOL_SFT, RT5616e_R_VOL_SFT, 39, 1, out_vol_tlv),
	/* OUTPUT Control */
	SOC_DOUBLE_TLV("OUT Playback Volume", RT5616e_LOUT_CTRL1,
		       RT5616e_L_VOL_SFT, RT5616e_R_VOL_SFT, 39, 1, out_vol_tlv),

	/* DAC Digital Volume */
	SOC_DOUBLE_TLV("DAC1 Playback Volume", RT5616e_DAC1_DIG_VOL,
		       RT5616e_L_VOL_SFT, RT5616e_R_VOL_SFT,
		       255, 0, dac_vol_tlv),
	/* IN1/IN2 Control */
	SOC_SINGLE_TLV("IN1 Boost Volume", RT5616e_IN1_IN2,
		       RT5616e_BST_SFT1, 8, 0, bst_tlv),
	/* ADC Digital Volume Control */
	SOC_SINGLE_TLV("ADC L Capture Volume", RT5616e_ADC_ADJ_CTRL4,
		       RT5616e_VOL_ADCL_SFT, 127, 0, adc_vol_tlv),
	SOC_SINGLE_TLV("ADC R Capture Volume", RT5616e_ADC_ADJ_CTRL5,
		       RT5616e_VOL_ADCR_SFT, 127, 0, adc_vol_tlv),
};

static int is_sys_clk_from_pll(struct snd_soc_dapm_widget *source,
			       struct snd_soc_dapm_widget *sink)
{
	unsigned int val;
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(source->dapm);

	val = snd_soc_read(codec, RT5616e_SYS_CLK);
	val &= RT5616e_SEL_SYSCLK_MASK;
	if (val == RT5616e_SEL_SYSCLK_PLL) {
		snd_soc_update_bits(codec, RT5616e_PLL_CTRL2,
				    RT5616e_RESET_PLL_MASK,
				    RT5616e_NO_RESET_PLL);
		return 1;
	} else
		return 0;
}

/* Digital Mixer */
static const struct snd_kcontrol_new rt5616e_sto1_adc_l_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5616e_STO1_ADC_MIXER,
			RT5616e_M_STO1_ADC_L1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5616e_STO1_ADC_MIXER,
			RT5616e_M_STO1_ADC_L2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_sto1_adc_r_mix[] = {
	SOC_DAPM_SINGLE("ADC1 Switch", RT5616e_STO1_ADC_MIXER,
			RT5616e_M_STO1_ADC_R1_SFT, 1, 1),
	SOC_DAPM_SINGLE("ADC2 Switch", RT5616e_STO1_ADC_MIXER,
			RT5616e_M_STO1_ADC_R2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_dac_l_mix[] = {
	SOC_DAPM_SINGLE("Stereo ADC Switch", RT5616e_AD_DA_MIXER,
			RT5616e_M_ADCMIX_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("INF1 Switch", RT5616e_AD_DA_MIXER,
			RT5616e_M_IF1_DAC_L_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_dac_r_mix[] = {
	SOC_DAPM_SINGLE("Stereo ADC Switch", RT5616e_AD_DA_MIXER,
			RT5616e_M_ADCMIX_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("INF1 Switch", RT5616e_AD_DA_MIXER,
			RT5616e_M_IF1_DAC_R_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_sto_dac_l_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5616e_STO_DAC_MIXER,
			RT5616e_M_DAC_L1_MIXL_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5616e_STO_DAC_MIXER,
			RT5616e_M_DAC_R1_MIXL_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_sto_dac_r_mix[] = {
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5616e_STO_DAC_MIXER,
			RT5616e_M_DAC_R1_MIXR_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5616e_STO_DAC_MIXER,
			RT5616e_M_DAC_L1_MIXR_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_sto_dac_l_out_mix[] = {
	SOC_DAPM_SINGLE("STO DAC L1 Switch", RT5616e_HP_DC_OST1,
			RT5616e_M_STO_DAC_L1_MIXL_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_sto_dac_r_out_mix[] = {
	SOC_DAPM_SINGLE("STO DAC R1 Switch", RT5616e_HP_DC_OST1,
			RT5616e_M_STO_DAC_R1_MIXR_SFT, 1, 1),
};

/* Analog Input Mixer */
static const struct snd_kcontrol_new rt5616e_rec_l_mix[] = {
	SOC_DAPM_SINGLE("BST1 Switch", RT5616e_REC_L2_MIXER,
			RT5616e_M_BST1_RM_L_SFT, 1, 1),
};

/* Analog Output Mixer */

static const struct snd_kcontrol_new rt5616e_hpo_mix[] = {
	SOC_DAPM_SINGLE("DAC1 Switch", RT5616e_HPO_MIXER,
			RT5616e_M_DAC1_HM_SFT, 1, 1),
	SOC_DAPM_SINGLE("HPVOL Switch", RT5616e_HPO_MIXER,
			RT5616e_MU_IN_HP_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5616e_lout_mix[] = {
	SOC_DAPM_SINGLE("DAC L1 Switch", RT5616e_LOUT_MIXER,
			RT5616e_M_DAC_L1_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC R1 Switch", RT5616e_LOUT_MIXER,
			RT5616e_M_DAC_R1_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL L Switch", RT5616e_LOUT_MIXER,
			RT5616e_M_OV_L_LM_SFT, 1, 1),
	SOC_DAPM_SINGLE("OUTVOL R Switch", RT5616e_LOUT_MIXER,
			RT5616e_M_OV_R_LM_SFT, 1, 1),
};

static int rt5616e_adc_event(struct snd_soc_dapm_widget *w,
			    struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_update_bits(codec, RT5616e_PWR_DIG1,
				    RT5616e_PWR_ADC_R, RT5616e_PWR_ADC_R);
		msleep(90);
		snd_soc_update_bits(codec, RT5616e_ADC_ADJ_CTRL3,
				RT5616e_MU_ADC_VOL_L | RT5616e_MU_ADC_VOL_R, 0);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_update_bits(codec, RT5616e_ADC_ADJ_CTRL3,
				    RT5616e_MU_ADC_VOL_L | RT5616e_MU_ADC_VOL_R,
				    RT5616e_MU_ADC_VOL_L | RT5616e_MU_ADC_VOL_R);
		break;
	case SND_SOC_DAPM_POST_PMD:
		snd_soc_update_bits(codec, RT5616e_PWR_DIG1,
				    RT5616e_PWR_ADC_R, 0);
		break;

	default:
		return 0;
	}

	return 0;
}

static int rt5616e_dac_event(struct snd_soc_dapm_widget *w,
			    struct snd_kcontrol *kcontrol, int event)
{
	switch (event) {
	case SND_SOC_DAPM_POST_PMD:
		usleep_range(10000, 10100);
		break;

	default:
		return 0;
	}

	return 0;
}

static int rt5616e_dac_filter_event(struct snd_soc_dapm_widget *w,
			    struct snd_kcontrol *kcontrol, int event)
{
	switch (event) {
	case SND_SOC_DAPM_POST_PMD:
		usleep_range(10000, 10100);
		break;
	default:
		return 0;
	}

	return 0;
}

static int rt5616e_hp_event(struct snd_soc_dapm_widget *w,
			   struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(w->dapm);
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xa8d6);
		usleep_range(10000, 10100);
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xe8fe);
		break;
	case SND_SOC_DAPM_POST_PMU:
		/* headphone unmute sequence */
		snd_soc_update_bits(codec, RT5616e_DEPOP_M2,
				    RT5616e_DEPOP_MASK, RT5616e_DEPOP_AUTO);
		snd_soc_update_bits(codec, RT5616e_DEPOP_M1,
				    RT5616e_HP_CB_MASK | RT5616e_HP_CP_MASK |
				    RT5616e_HP_SG_MASK, RT5616e_HP_SG_DIS |
				    RT5616e_HP_CB_PU | RT5616e_HP_CP_PU);
		snd_soc_update_bits(codec, RT5616e_CHOP_DAC,
				    RT5616e_CKXEN_MASK | RT5616e_EN_CKXGEN_MASK,
				    RT5616e_CKXEN_EN | RT5616e_EN_CKXGEN_EN);
		snd_soc_update_bits(codec, RT5616e_HP_AMP_DET1,
				    RT5616e_SEL_PM_HP_MASK,
				    RT5616e_SEL_PM_HP_HIGH2);
		/* DAC Chopper / Pump +-1.8V */
		snd_soc_update_bits(codec, RT5616e_CHOP_DAC,
				    RT5616e_CKXSEL_MASK | RT5616e_CKXADC_MASK |
				    RT5616e_CKXSELADC_MASK, RT5616e_CK2_DIV8|
				    RT5616e_CKXADC_EN | RT5616e_CKXSELADC_DIV8);
		snd_soc_update_bits(codec, RT5616e_SYS_CLK,
				    RT5616e_PW_CLK_SYS_MASK |
				    RT5616e_PW_CLK_SYS_DIV_MASK |
				    RT5616e_PW_CLK_SYSLP_DIV_MASK |
				    RT5616e_PW_CLK_INT_DIV_MASK,
				    RT5616e_PW_CLK_SYS_EN |
				    RT5616e_PW_CLK_SYS_DIV_EN |
				    RT5616e_PW_CLK_SYSLP_DIV_EN |
				    RT5616e_PW_CLK_INT_DIV_EN);
		snd_soc_update_bits(codec, RT5616e_OSR_CTRL,
				    RT5616e_SEL_DAC_OSR_MASK,
				    RT5616e_SEL_DAC_OSR_DIV1);
		if (rt5616e->hw_ver == RT5616e_VERB) {
			snd_soc_update_bits(codec, RT5616e_IP_CTRL,
				    RT5616e_PWR_SVOL_MASK |
				    RT5616e_PWR_CLBR_MASK,
				    RT5616e_PWR_SVOL_ON | RT5616e_PWR_CLBR_ON);
			snd_soc_update_bits(codec, RT5616e_HP_VOL, RT5616e_VOL_L_MUTE |
					RT5616e_VOL_R_MUTE, 0);
			msleep(30);
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
					RT5616e_L_MUTE | RT5616e_R_MUTE, 0);
		} else {
			snd_soc_update_bits(codec, RT5616e_IP_CTRL,
				    RT5616e_PWR_SVOL_MASK | RT5616e_CG_CLBR_MASK |
				    RT5616e_PWR_CLBR_MASK,
				    RT5616e_PWR_SVOL_ON | RT5616e_PWR_CLBR_ON |
					RT5616e_CG_CLBR_MASK);
			/* work-around i2c access */
			snd_soc_update_bits(codec, RT5616e_DEPOP_M1, RT5616e_SMT_TRIG_MASK |
					RT5616e_HP_CO_MASK | RT5616e_HP_CP_MASK | RT5616e_HP_CB_MASK,
					RT5616e_SMT_TRIG_MASK | RT5616e_HP_CO_MASK | RT5616e_HP_CP_MASK |
					RT5616e_HP_CB_MASK);
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_RAMP_MASK |
					RT5616e_MRES_MASK | RT5616e_DIG_DP_MASK,
					RT5616e_RAMP_EN | RT5616e_MRES_25MO |
					RT5616e_DIG_DP_EN);
			msleep(20);

			snd_soc_update_bits(codec, RT5616e_HP_VOL, RT5616e_VOL_L_MUTE |
					RT5616e_VOL_R_MUTE, 0);
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
					RT5616e_L_MUTE | RT5616e_R_MUTE,
					0);
			msleep(20);

			/* work-around i2c access */
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_DEPOP_MASK,
					RT5616e_DEPOP_MASK);
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_DIG_DP_MASK,
					RT5616e_DIG_DP_DIS);
		}

		break;

	case SND_SOC_DAPM_PRE_PMD:
		/* headphone mute sequence */
		if (rt5616e->hw_ver == RT5616e_VERB) {
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
				    RT5616e_L_MUTE | RT5616e_R_MUTE,
				    RT5616e_L_MUTE | RT5616e_R_MUTE);
			msleep(30);
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
					RT5616e_VOL_L_MUTE | RT5616e_VOL_R_MUTE,
					RT5616e_VOL_L_MUTE | RT5616e_VOL_R_MUTE);
			snd_soc_update_bits(codec, RT5616e_DEPOP_M1,
					RT5616e_HP_CB_MASK, RT5616e_HP_CB_PD);
			msleep(20);
			snd_soc_update_bits(codec, RT5616e_DEPOP_M1,
					RT5616e_HP_CP_MASK, RT5616e_HP_CP_PD);
		} else {
				/* work-around i2c access */
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_DIG_DP_MASK,
					RT5616e_DIG_DP_EN);
			msleep(30);
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_RAMP_MASK |
			RT5616e_DEPOP_MASK, RT5616e_DEPOP_AUTO | RT5616e_RAMP_EN);
			msleep(30);
			/* headphone mute sequence */
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
						RT5616e_L_MUTE | RT5616e_R_MUTE,
						RT5616e_L_MUTE | RT5616e_R_MUTE);
			msleep(20);
			snd_soc_update_bits(codec, RT5616e_HP_VOL,
					RT5616e_VOL_L_MUTE | RT5616e_VOL_R_MUTE,
					RT5616e_VOL_L_MUTE | RT5616e_VOL_R_MUTE);
			/* work-around i2c access */
			snd_soc_update_bits(codec, RT5616e_DEPOP_M2, RT5616e_DIG_DP_MASK |
			RT5616e_DEPOP_MASK | RT5616e_RAMP_MASK, RT5616e_DEPOP_MAN |
			RT5616e_DIG_DP_DIS | RT5616e_RAMP_EN);
			usleep_range(10000, 10100);
		}
		break;

	default:
		return 0;
	}

	return 0;
}

static int rt5616e_lout_event(struct snd_soc_dapm_widget *w,
			     struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(w->dapm);
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		snd_soc_update_bits(codec, RT5616e_STO_DAC_MIXER,
				    RT5616e_DAC_L1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_R_VOL_MASK |
				    RT5616e_DAC_L1_STO_R_VOL_MASK,
				    RT5616e_DAC_L1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_R_VOL_MASK |
				    RT5616e_DAC_L1_STO_R_VOL_MASK);
		snd_soc_update_bits(codec, RT5616e_DEPOP_M1,
				RT5616e_HP_CB_MASK | RT5616e_HP_CP_MASK |
				RT5616e_HP_SG_MASK, RT5616e_HP_SG_DIS |
				RT5616e_HP_CB_PU | RT5616e_HP_CP_PU);
		snd_soc_update_bits(codec, RT5616e_DEPOP_M2,
				    RT5616e_DEPOP_MASK | RT5616e_RAMP_MASK |
				    RT5616e_MRES_MASK, RT5616e_DEPOP_MAN |
				    RT5616e_RAMP_EN | RT5616e_MRES_25MO);
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xb8d6);
		usleep_range(10000, 10100);
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xf8fe);
		break;
	case SND_SOC_DAPM_POST_PMU:
		/* headphone unmute sequence */
		snd_soc_update_bits(codec, RT5616e_CHOP_DAC,
				    RT5616e_CKXEN_MASK | RT5616e_EN_CKXGEN_MASK,
				    RT5616e_CKXEN_EN | RT5616e_EN_CKXGEN_EN);
		snd_soc_update_bits(codec, RT5616e_HP_AMP_DET1,
				    RT5616e_SEL_PM_HP_MASK,
				    RT5616e_SEL_PM_HP_HIGH2);
		/* DAC Chopper / Pump +-1.8V */
		snd_soc_update_bits(codec, RT5616e_CHOP_DAC,
				    RT5616e_CKXSEL_MASK | RT5616e_CKXADC_MASK |
				    RT5616e_CKXSELADC_MASK, RT5616e_CK2_DIV8|
				    RT5616e_CKXADC_EN | RT5616e_CKXSELADC_DIV8);
		snd_soc_update_bits(codec, RT5616e_SYS_CLK,
				    RT5616e_PW_CLK_SYS_MASK |
				    RT5616e_PW_CLK_SYS_DIV_MASK |
				    RT5616e_PW_CLK_SYSLP_DIV_MASK |
				    RT5616e_PW_CLK_INT_DIV_MASK,
				    RT5616e_PW_CLK_SYS_EN |
				    RT5616e_PW_CLK_SYS_DIV_EN |
				    RT5616e_PW_CLK_SYSLP_DIV_EN |
				    RT5616e_PW_CLK_INT_DIV_EN);
#ifdef VER_LRCLK_96K
		snd_soc_update_bits(codec, RT5616e_OSR_CTRL,
				    RT5616e_SEL_DAC_OSR_MASK,
				    RT5616e_SEL_DAC_OSR_DIS);
#else
		snd_soc_update_bits(codec, RT5616e_OSR_CTRL,
				    RT5616e_SEL_DAC_OSR_MASK,
				    RT5616e_SEL_DAC_OSR_DIV1);
#endif
		if (rt5616e->hw_ver == RT5616e_VERB)
			snd_soc_update_bits(codec, RT5616e_IP_CTRL, RT5616e_PWR_SVOL_MASK,
				RT5616e_PWR_SVOL_ON);
		else
			snd_soc_update_bits(codec, RT5616e_IP_CTRL,
				    RT5616e_PWR_SVOL_MASK | RT5616e_CG_CLBR_MASK,
				    RT5616e_PWR_SVOL_ON | RT5616e_CG_CLBR_MASK);
		snd_soc_update_bits(codec, RT5616e_LOUT_CTRL1,
				    RT5616e_L_MUTE | RT5616e_VOL_L_MUTE |
				    RT5616e_R_MUTE | RT5616e_VOL_R_MUTE,
				    0);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		/* headphone mute sequence */
		snd_soc_update_bits(codec, RT5616e_LOUT_CTRL1,
				    RT5616e_L_MUTE | RT5616e_VOL_L_MUTE |
				    RT5616e_R_MUTE | RT5616e_VOL_R_MUTE,
				    RT5616e_L_MUTE | RT5616e_VOL_L_MUTE |
				    RT5616e_R_MUTE | RT5616e_VOL_R_MUTE);
		snd_soc_update_bits(codec, RT5616e_STO_DAC_MIXER,
				    RT5616e_DAC_L1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_L_VOL_MASK |
				    RT5616e_DAC_R1_STO_R_VOL_MASK |
				    RT5616e_DAC_L1_STO_R_VOL_MASK, 0);
		break;

	default:
		return 0;
	}

	return 0;
}

static int rt5616e_bst1_event(struct snd_soc_dapm_widget *w,
			     struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = snd_soc_dapm_to_codec(w->dapm);

	switch (event) {
	case SND_SOC_DAPM_PRE_PMU:
		snd_soc_update_bits(codec, RT5616e_ADC_ADJ_CTRL3,
				    RT5616e_MU_ADC_VOL_L | RT5616e_MU_ADC_VOL_R,
				    RT5616e_MU_ADC_VOL_L | RT5616e_MU_ADC_VOL_R);
		snd_soc_update_bits(codec, RT5616e_REC_L2_MIXER,
				    RT5616e_M_RSV_RM_L,
				    RT5616e_M_RSV_RM_L);
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xa8d6);
		msleep(10);
		snd_soc_write(codec, RT5616e_PWR_ANLG1, 0xe81e);
		break;
	case SND_SOC_DAPM_POST_PMU:
		snd_soc_update_bits(codec, RT5616e_PWR_ANLG2,
				RT5616e_PWR_BST2_OP2 | RT5616e_PWR_BST1_OP2,
				RT5616e_PWR_BST2_OP2 | RT5616e_PWR_BST1_OP2);
		snd_soc_update_bits(codec, RT5616e_CHOP_DAC,
				RT5616e_CKXSELADC_MASK | RT5616e_CKXADC_MASK |
				RT5616e_EN_CKXADC_MASK | RT5616e_CKXSEL_MASK,
				RT5616e_CKXSELADC_DIV8 | RT5616e_CKXADC_MASK |
				RT5616e_EN_CKXADC_EN | RT5616e_CKXSEL_MASK);
		snd_soc_update_bits(codec, RT5616e_MICBIAS,
				RT5616e_MIC3_CLK_MASK | RT5616e_PWR_MB_MASK |
				RT5616e_IRQ_DEB_SRC_MASK, RT5616e_MIC3_CLK_EN |
				RT5616e_PWR_MB_PU | RT5616e_IRQ_DEB_MCLK);
		snd_soc_update_bits(codec, RT5616e_D_MISC, RT5616e_D_GATE_EN,
				RT5616e_D_GATE_EN);
		snd_soc_update_bits(codec, RT5616e_IN1_IN2,
				RT5616e_BST_MASK1, RT5616e_BST_20DB);
		snd_soc_update_bits(codec, RT5616e_RC_CTRL,
				RT5616e_RC_ON_OFF_MASK, RT5616e_RC_ON);
		snd_soc_update_bits(codec, RT5616e_OSR_CTRL,
				RT5616e_SEL_ADC_OSR_MASK,
				RT5616e_SEL_ADC_OSR_DIV2);
		break;

	case SND_SOC_DAPM_PRE_PMD:
		snd_soc_update_bits(codec, RT5616e_PWR_ANLG2,
				RT5616e_PWR_BST1_OP2, 0);
		snd_soc_update_bits(codec, RT5616e_MICBIAS,
				RT5616e_MIC3_CLK_MASK | RT5616e_PWR_MB_MASK |
				RT5616e_IRQ_DEB_SRC_MASK, RT5616e_MIC3_CLK_DIS |
				RT5616e_PWR_MB_PD | RT5616e_IRQ_DEB_RC);
		break;

	default:
		return 0;
	}

	return 0;
}

static const struct snd_soc_dapm_widget rt5616e_dapm_widgets[] = {
	SND_SOC_DAPM_SUPPLY("PLL1", RT5616e_PLL_CTRL,
			    RT5616e_PLL_ON_OFF_SFT, 0, NULL, 0),
	/* Input Side */
	/* micbias */
	SND_SOC_DAPM_SUPPLY("LDO", RT5616e_PWR_ANLG1,
			    RT5616e_PWR_LDO_BIT, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("micbias1", RT5616e_PWR_ANLG2,
			    RT5616e_PWR_MB3_BIT, 0, NULL, 0),

	/* Input Lines */
	SND_SOC_DAPM_INPUT("MIC1"),
	SND_SOC_DAPM_INPUT("IN1P"),
	SND_SOC_DAPM_INPUT("IN1N"),

	/* Boost */
	SND_SOC_DAPM_PGA_E("BST1", RT5616e_PWR_ANLG2,
			   RT5616e_PWR_BST1_BIT, 0, NULL, 0, rt5616e_bst1_event,
			   SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU |
			   SND_SOC_DAPM_PRE_PMU),

	/* REC Mixer */
	SND_SOC_DAPM_MIXER("RECMIXL", RT5616e_PWR_MIXER, RT5616e_PWR_RM_L_BIT,
			0, rt5616e_rec_l_mix, ARRAY_SIZE(rt5616e_rec_l_mix)),

	/* ADCs */
	SND_SOC_DAPM_ADC_E("ADC L", NULL, RT5616e_PWR_DIG1,
			   RT5616e_PWR_ADC_L_BIT, 0, rt5616e_adc_event,
			   SND_SOC_DAPM_POST_PMD | SND_SOC_DAPM_POST_PMU |
			   SND_SOC_DAPM_PRE_PMD),

	/* ADC Mixer */
	SND_SOC_DAPM_SUPPLY("stereo1 adc filter", RT5616e_IP_CTRL,
			    RT5616e_ADC_FIL_SFT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER("Stereo1 ADC MIXL", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto1_adc_l_mix,
			   ARRAY_SIZE(rt5616e_sto1_adc_l_mix)),
	SND_SOC_DAPM_MIXER("Stereo1 ADC MIXR", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto1_adc_r_mix,
			   ARRAY_SIZE(rt5616e_sto1_adc_r_mix)),

	/* Clock */
	SND_SOC_DAPM_SUPPLY("clock sys gating", RT5616e_SYS_CLK,
			    RT5616e_PW_CLK_SYS_SFT, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("clock sys div gating", RT5616e_SYS_CLK,
			    RT5616e_PW_CLK_SYS_DIV_SFT, 0, NULL, 0),

	/* Digital Interface */
	SND_SOC_DAPM_SUPPLY("I2S1", RT5616e_I2S_CLK_CTRL,
			    RT5616e_I2S_EN_SFT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC1 L", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 DAC1 R", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("IF1 ADC1", SND_SOC_NOPM, 0, 0, NULL, 0),

	/* Digital Interface Select */

	/* Audio Interface */
	SND_SOC_DAPM_AIF_IN("AIF1RX", "AIF1 Playback", 0, SND_SOC_NOPM, 0, 0),
	SND_SOC_DAPM_AIF_OUT("AIF1TX", "AIF1 Capture", 0, SND_SOC_NOPM, 0, 0),

	/* Output Side */
	/* DAC mixer before sound effect  */
	SND_SOC_DAPM_MIXER("DAC MIXL", SND_SOC_NOPM, 0, 0,
			   rt5616e_dac_l_mix, ARRAY_SIZE(rt5616e_dac_l_mix)),
	SND_SOC_DAPM_MIXER("DAC MIXR", SND_SOC_NOPM, 0, 0,
			   rt5616e_dac_r_mix, ARRAY_SIZE(rt5616e_dac_r_mix)),

	SND_SOC_DAPM_SUPPLY("Stereo1 DAC Power", RT5616e_IP_CTRL,
			    RT5616e_STO1_DAC_SFT, 0, NULL, 0),
	SND_SOC_DAPM_SUPPLY("stereo1 dac filter", RT5616e_IP_CTRL,
			    RT5616e_DAC_FIL_SFT, 0, rt5616e_dac_filter_event,
				SND_SOC_DAPM_POST_PMD),

	/* DAC Mixer */
	SND_SOC_DAPM_MIXER("Stereo DAC MIXL", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto_dac_l_mix,
			   ARRAY_SIZE(rt5616e_sto_dac_l_mix)),
	SND_SOC_DAPM_MIXER("Stereo DAC MIXR", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto_dac_r_mix,
			   ARRAY_SIZE(rt5616e_sto_dac_r_mix)),

	SND_SOC_DAPM_MIXER("Stereo DAC MIXL OUT", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto_dac_l_out_mix,
			   ARRAY_SIZE(rt5616e_sto_dac_l_out_mix)),
	SND_SOC_DAPM_MIXER("Stereo DAC MIXR OUT", SND_SOC_NOPM, 0, 0,
			   rt5616e_sto_dac_r_out_mix,
			   ARRAY_SIZE(rt5616e_sto_dac_r_out_mix)),
	/* DACs */
	SND_SOC_DAPM_DAC("DAC L1", NULL, RT5616e_PWR_DIG1,
			 RT5616e_PWR_DAC_L1_BIT, 0),
	SND_SOC_DAPM_DAC_E("DAC R1", NULL, RT5616e_PWR_DIG1,
		RT5616e_PWR_DAC_R1_BIT, 0, rt5616e_dac_event, SND_SOC_DAPM_POST_PMD),

	/* Output Volume */
	SND_SOC_DAPM_PGA("OUTVOL L", RT5616e_PWR_VOL,
			 RT5616e_PWR_OV_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("OUTVOL R", RT5616e_PWR_VOL,
			 RT5616e_PWR_OV_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPOVOL L", RT5616e_PWR_VOL,
			 RT5616e_PWR_HV_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPOVOL R", RT5616e_PWR_VOL,
			 RT5616e_PWR_HV_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("DAC 1", SND_SOC_NOPM,
			 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("HPOVOL", SND_SOC_NOPM,
			 0, 0, NULL, 0),
	/* HPO/LOUT/Mono Mixer */
	SND_SOC_DAPM_MIXER("HPO MIX", SND_SOC_NOPM, 0, 0,
			   rt5616e_hpo_mix, ARRAY_SIZE(rt5616e_hpo_mix)),
	SND_SOC_DAPM_MIXER("LOUT MIX", SND_SOC_NOPM, 0, 0,
			   rt5616e_lout_mix, ARRAY_SIZE(rt5616e_lout_mix)),

	SND_SOC_DAPM_PGA_S("HP amp", 1, SND_SOC_NOPM, 0, 0,
			   rt5616e_hp_event, SND_SOC_DAPM_PRE_PMD |
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMU),
	SND_SOC_DAPM_PGA_S("LOUT amp", 1, SND_SOC_NOPM, 0, 0,
			   rt5616e_lout_event, SND_SOC_DAPM_PRE_PMD |
			   SND_SOC_DAPM_POST_PMU | SND_SOC_DAPM_PRE_PMU),

	/* Output Lines */
	SND_SOC_DAPM_OUTPUT("HPOL"),
	SND_SOC_DAPM_OUTPUT("HPOR"),
	SND_SOC_DAPM_OUTPUT("LOUTL"),
	SND_SOC_DAPM_OUTPUT("LOUTR"),
};

static const struct snd_soc_dapm_route rt5616e_dapm_routes[] = {
	{"IN1P", NULL, "LDO"},
	{"IN1N", NULL, "LDO"},

	{"IN1P", NULL, "MIC1"},
	{"IN1N", NULL, "MIC1"},

	{"BST1", NULL, "IN1P"},
	{"BST1", NULL, "IN1N"},
	{"BST1", NULL, "micbias1"},
	{"BST1", NULL, "clock sys gating"},
	{"BST1", NULL, "clock sys div gating"},

	{"RECMIXL", "BST1 Switch", "BST1"},

	{"ADC L", NULL, "RECMIXL"},

	{"Stereo1 ADC MIXL", "ADC1 Switch", "ADC L"},
	{"Stereo1 ADC MIXL", "ADC2 Switch", "Stereo DAC MIXL"},
	{"Stereo1 ADC MIXL", NULL, "stereo1 adc filter"},
	{"stereo1 adc filter", NULL, "PLL1", is_sys_clk_from_pll},

	{"Stereo1 ADC MIXR", "ADC1 Switch", "ADC L"},
	{"Stereo1 ADC MIXR", "ADC2 Switch", "Stereo DAC MIXR"},
	{"Stereo1 ADC MIXR", NULL, "stereo1 adc filter"},
	{"stereo1 adc filter", NULL, "PLL1", is_sys_clk_from_pll},

	{"IF1 ADC1", NULL, "Stereo1 ADC MIXL"},
	{"IF1 ADC1", NULL, "Stereo1 ADC MIXR"},
	{"IF1 ADC1", NULL, "I2S1"},

	{"AIF1TX", NULL, "IF1 ADC1"},

	{"IF1 DAC", NULL, "AIF1RX"},
	{"IF1 DAC", NULL, "I2S1"},

	{"IF1 DAC1 L", NULL, "IF1 DAC"},
	{"IF1 DAC1 R", NULL, "IF1 DAC"},

	{"DAC MIXL", "Stereo ADC Switch", "Stereo1 ADC MIXL"},
	{"DAC MIXL", "INF1 Switch", "IF1 DAC1 L"},
	{"DAC MIXR", "Stereo ADC Switch", "Stereo1 ADC MIXR"},
	{"DAC MIXR", "INF1 Switch", "IF1 DAC1 R"},

	{"Stereo DAC MIXL", "DAC L1 Switch", "DAC MIXL"},
	{"Stereo DAC MIXL", "DAC R1 Switch", "DAC MIXR"},
	{"Stereo DAC MIXL", NULL, "Stereo1 DAC Power"},
	{"Stereo DAC MIXL", NULL, "stereo1 dac filter"},
	{"Stereo DAC MIXR", "DAC R1 Switch", "DAC MIXR"},
	{"Stereo DAC MIXR", "DAC L1 Switch", "DAC MIXL"},
	{"Stereo DAC MIXR", NULL, "Stereo1 DAC Power"},
	{"Stereo DAC MIXR", NULL, "stereo1 dac filter"},

	{"Stereo DAC MIXL OUT", "STO DAC L1 Switch", "Stereo DAC MIXL"},
	{"Stereo DAC MIXR OUT", "STO DAC R1 Switch", "Stereo DAC MIXR"},

	{"DAC L1", NULL, "Stereo DAC MIXL OUT"},
	{"DAC L1", NULL, "PLL1", is_sys_clk_from_pll},
	{"DAC R1", NULL, "Stereo DAC MIXR OUT"},
	{"DAC R1", NULL, "PLL1", is_sys_clk_from_pll},

	{"DAC 1", NULL, "DAC L1"},
	{"DAC 1", NULL, "DAC R1"},

	{"HPOVOL L", NULL, "DAC L1"},
	{"HPOVOL R", NULL, "DAC R1"},
	{"OUTVOL L", NULL, "DAC L1"},
	{"OUTVOL R", NULL, "DAC R1"},

	{"HPOVOL", NULL, "HPOVOL L"},
	{"HPOVOL", NULL, "HPOVOL R"},
	{"HPO MIX", "DAC1 Switch", "DAC 1"},
	{"HPO MIX", "HPVOL Switch", "HPOVOL"},

	{"LOUT MIX", "DAC L1 Switch", "DAC L1"},
	{"LOUT MIX", "DAC R1 Switch", "DAC R1"},
	{"LOUT MIX", "OUTVOL L Switch", "OUTVOL L"},
	{"LOUT MIX", "OUTVOL R Switch", "OUTVOL R"},

	{"HP amp", NULL, "HPO MIX"},
	{"HPOL", NULL, "HP amp"},
	{"HPOR", NULL, "HP amp"},

	{"LOUT amp", NULL, "LOUT MIX"},
	{"LOUTL", NULL, "LOUT amp"},
	{"LOUTR", NULL, "LOUT amp"},

};

static int rt5616e_get_clk_info(int sclk, int rate)
{
	int i, pd[] = {0, 1, 2, 4, 6, 8, 12, 16, 24};

	if (sclk <= 0 || rate <= 0)
		return -EINVAL;

	rate = rate << 8;
	for (i = 0; i < ARRAY_SIZE(pd); i++) {
		if (sclk == rate * pd[i])
			return i;
	}

	return -EINVAL;
}

static int rt5616e_hw_params(struct snd_pcm_substream *substream,
			    struct snd_pcm_hw_params *params,
			    struct snd_soc_dai *dai)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);
	unsigned int val_len = 0, val_adc_clk, val_dac_clk, mask_adc_clk,
		mask_dac_clk;
	int pre_div, bclk_ms, frame_size;

	rt5616e->lrck[dai->id] = params_rate(params);

	pre_div = rt5616e_get_clk_info(rt5616e->sysclk, rt5616e->lrck[dai->id]);

	if (pre_div < 0) {
		dev_err(codec->dev, "Unsupported clock setting\n");
		return -EINVAL;
	}
	frame_size = snd_soc_params_to_frame_size(params);
	if (frame_size < 0) {
		dev_err(codec->dev, "Unsupported frame size: %d\n", frame_size);
		return -EINVAL;
	}
	bclk_ms = frame_size > 32 ? 1 : 0;
	rt5616e->bclk[dai->id] = rt5616e->lrck[dai->id] * (32 << bclk_ms);

	dev_dbg(dai->dev, "bclk is %dHz and lrck is %dHz\n",
		rt5616e->bclk[dai->id], rt5616e->lrck[dai->id]);
	dev_dbg(dai->dev, "bclk_ms is %d and pre_div is %d for iis %d\n",
		bclk_ms, pre_div, dai->id);

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
		val_len |= RT5616e_I2S_DATA_LEN_16;
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		val_len |= RT5616e_I2S_DATA_LEN_20;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		val_len |= RT5616e_I2S_DATA_LEN_24;
		break;
	case SNDRV_PCM_FORMAT_S32_LE:
		val_len |= RT5616e_I2S_DATA_LEN_32;
		break;
	case SNDRV_PCM_FORMAT_S8:
		val_len |= RT5616e_I2S_DATA_LEN_8;
		break;
	default:
		return -EINVAL;
	}

	if (pre_div < 4)
		snd_soc_update_bits(codec, RT5616e_FIFO_CTRL,
			    RT5616e_FIFO_DIV_MASK, pre_div);
	else
		dev_dbg(codec->dev, "FIFO_DIV is not supported.\n");

	mask_adc_clk = RT5616e_ADC_FIL_CLK_MASK;
	mask_dac_clk = RT5616e_DAC_FIL_CLK_MASK;
	val_adc_clk = pre_div << RT5616e_ADC_FIL_CLK_SFT;
	val_dac_clk = pre_div << RT5616e_DAC_FIL_CLK_SFT;
	snd_soc_update_bits(codec, RT5616e_GPIO_CTRL2,
			    RT5616e_I2S_DATA_LEN_MASK, val_len);
	snd_soc_update_bits(codec, RT5616e_FTR_CLK_CTRL, mask_adc_clk,
		val_adc_clk);
	snd_soc_update_bits(codec, RT5616e_FTR_CLK_CTRL, mask_dac_clk,
		val_dac_clk);

	return 0;
}

static int rt5616e_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);
	unsigned int reg_val = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		rt5616e->master[dai->id] = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		reg_val |= RT5616e_I2S_MS_S;
		rt5616e->master[dai->id] = 0;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		reg_val |= RT5616e_I2S_BCLK_INV;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		reg_val |= RT5616e_I2S_FMT_LJ;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		reg_val |= RT5616e_I2S_FMT_MODEA;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		reg_val |= RT5616e_I2S_FMT_MODEB;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_update_bits(codec, RT5616e_GPIO_CTRL2,
			    RT5616e_I2S_DATA_LEN_MASK | RT5616e_I2S_MS_MASK |
			    RT5616e_I2S_FMT_MASK, reg_val);

	return 0;
}

static int rt5616e_set_dai_sysclk(struct snd_soc_dai *dai,
				 int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);
	unsigned int reg_val = 0;

	if (freq == rt5616e->sysclk && clk_id == rt5616e->sysclk_src)
		return 0;

	switch (clk_id) {
	case RT5616e_SCLK_S_MCLK:
		reg_val |= RT5616e_SEL_SYSCLK_MCLK;
		break;
	case RT5616e_SCLK_S_PLL1:
		reg_val |= RT5616e_SEL_SYSCLK_PLL;
		break;
	case RT5616e_SCLK_S_RCCLK:
		reg_val |= RT5616e_SEL_SYSCLK_RC;
		break;
	default:
		dev_err(codec->dev, "Invalid clock id (%d)\n", clk_id);
		return -EINVAL;
	}

	snd_soc_update_bits(codec, RT5616e_SYS_CLK,
			    RT5616e_SEL_SYSCLK_MASK, reg_val);
	rt5616e->sysclk = freq;
	rt5616e->sysclk_src = clk_id;

	dev_dbg(dai->dev, "Sysclk is %dHz and clock id is %d\n", freq, clk_id);

	return 0;
}

static int rt5616e_set_dai_pll(struct snd_soc_dai *dai, int pll_id, int source,
			      unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);
	struct rl6231_pll_code pll_code;
	int ret;

	if (source == rt5616e->pll_src && freq_in == rt5616e->pll_in &&
	    freq_out == rt5616e->pll_out)
		return 0;

	if (!freq_in || !freq_out) {
		dev_dbg(codec->dev, "PLL disabled\n");

		rt5616e->pll_in = 0;
		rt5616e->pll_out = 0;
		snd_soc_update_bits(codec, RT5616e_SYS_CLK,
				    RT5616e_SEL_SYSCLK_MASK,
				    RT5616e_SEL_SYSCLK_MCLK);
		return 0;
	}

	switch (source) {
	case RT5616e_PLL1_S_MCLK:
		snd_soc_update_bits(codec, RT5616e_PLL_CTRL,
				    RT5616e_SEL_PLL_SRC_MASK,
				    RT5616e_SEL_PLL_SRC_MCLK);
		break;
	case RT5616e_PLL1_S_BCLK:
		snd_soc_update_bits(codec, RT5616e_PLL_CTRL,
				    RT5616e_SEL_PLL_SRC_MASK,
				    RT5616e_SEL_PLL_SRC_BCLK);
		break;
	case RT5616e_PLL1_S_RC:
		snd_soc_update_bits(codec, RT5616e_PLL_CTRL,
				    RT5616e_SEL_PLL_SRC_MASK,
				    RT5616e_SEL_PLL_SRC_RC);
		break;
	default:
		dev_err(codec->dev, "Unknown PLL source %d\n", source);
		return -EINVAL;
	}

	ret = rl6231_pll_calc(freq_in, freq_out, &pll_code);
	if (ret < 0) {
		dev_err(codec->dev, "Unsupport input clock %d\n", freq_in);
		return ret;
	}

	dev_dbg(codec->dev, "bypass=%d m=%d n=%d k=%d\n",
		pll_code.m_bp, (pll_code.m_bp ? 0 : pll_code.m_code),
		pll_code.n_code, pll_code.k_code);

	snd_soc_write(codec, RT5616e_PLL_CTRL1,
		      pll_code.n_code << RT5616e_PLL_N_SFT | pll_code.k_code);
	snd_soc_write(codec, RT5616e_PLL_CTRL2,
		      (pll_code.m_bp ? 0 : pll_code.m_code) <<
		      RT5616e_PLL_M_SFT |
		      pll_code.m_bp << RT5616e_PLL_M_BP_SFT);

	rt5616e->pll_in = freq_in;
	rt5616e->pll_out = freq_out;
	rt5616e->pll_src = source;

	return 0;
}

static int rt5616e_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	switch (level) {

	case SND_SOC_BIAS_ON:
		break;

	case SND_SOC_BIAS_PREPARE:
		/*
		 * SND_SOC_BIAS_PREPARE is called while preparing for a
		 * transition to ON or away from ON. If current bias_level
		 * is SND_SOC_BIAS_ON, then it is preparing for a transition
		 * away from ON. Disable the clock in that case, otherwise
		 * enable it.
		 */
		if (snd_soc_codec_get_bias_level(codec) == SND_SOC_BIAS_OFF)
			regmap_write(rt5616e->regmap, RT5616e_D_MISC, 0x0091);
		break;

	case SND_SOC_BIAS_STANDBY:
		break;

	case SND_SOC_BIAS_OFF:
		regmap_write(rt5616e->regmap, RT5616e_PWR_DIG1, 0x0000);
		regmap_write(rt5616e->regmap, RT5616e_PWR_ANLG2, 0x0004);
		regmap_write(rt5616e->regmap, RT5616e_PWR_MIXER, 0x0000);
		regmap_write(rt5616e->regmap, RT5616e_PWR_VOL, 0x0000);
		break;

	default:
		break;
	}

	return 0;
}

static void rt5616e_reg_init(struct snd_soc_codec *codec)
{
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);
	unsigned int calib_val, i = 0;

	regmap_write(rt5616e->regmap, RT5616e_PWR_ANLG1, 0xa8d6);
	msleep(20);
	regmap_write(rt5616e->regmap, RT5616e_PWR_ANLG2, 0x0004);
	regmap_write(rt5616e->regmap, RT5616e_PWR_ANLG1, 0xe8fe);
	/* Enable RC */
	if (rt5616e->hw_ver == RT5616e_VERB)
		regmap_write(rt5616e->regmap, RT5616e_RC_CTRL, 0x0001);
	regmap_write(rt5616e->regmap, RT5616e_SYS_CLK, 0x0032);
	regmap_write(rt5616e->regmap, RT5616e_HP_CALIB_AMP_DET, 0x0011);
	/* Enable Calibration power */
	regmap_write(rt5616e->regmap, RT5616e_IP_CTRL, 0x0200);
	/* HP AMP Calibration */
	regmap_write(rt5616e->regmap, RT5616e_HP_OFFSET_CALI1, 0x9f00);
	msleep(100);
	while (i < 10) {
		regmap_read(rt5616e->regmap, RT5616e_HP_OFFSET_CALI1,
			&calib_val);
		calib_val = calib_val & 0x8000;
		if (calib_val == 0x8000) {
			i++;
			msleep(1);
			dev_err(codec->dev, "Calibration again!\n");
		} else {
			dev_err(codec->dev, "Calibration done!\n");
			break;
		}
	}
	regmap_write(rt5616e->regmap, RT5616e_HP_CALIB_AMP_DET, 0x0400);
	msleep(100);
	/* IRQ clock from RC */
	if (rt5616e->hw_ver == RT5616e_VERB)
		regmap_write(rt5616e->regmap, RT5616e_MICBIAS, 0x2008);
	/* Enable JDH IRQ and Sticky */
	regmap_write(rt5616e->regmap, RT5616e_IRQ_CTRL1, 0x0280);
	/* GPIO1 as IRQ */
	regmap_write(rt5616e->regmap, RT5616e_GPIO_DRV_CTRL2, 0x0155);
	if (rt5616e->hw_ver == RT5616e_VERB)
		regmap_update_bits(rt5616e->regmap, RT5616e_DEPOP_M2,
				RT5616e_RAMP_MASK | RT5616e_MRES_MASK |
				RT5616e_DIG_DP_MASK | RT5616e_DP_TH_MASK,
				RT5616e_RAMP_EN | RT5616e_MRES_25MO |
				RT5616e_DIG_DP_EN | RT5616e_DP_4096);
	else
		regmap_update_bits(rt5616e->regmap, RT5616e_DEPOP_M2,
				RT5616e_RAMP_MASK | RT5616e_MRES_MASK |
				RT5616e_DIG_DP_MASK | RT5616e_DP_TH_MASK,
				RT5616e_RAMP_EN | RT5616e_MRES_25MO |
				RT5616e_DIG_DP_DIS | RT5616e_DP_4096);
	// regmap_write(rt5616e->regmap, RT5616e_PWR_ANLG1, 0x0000);
	regmap_write(rt5616e->regmap, RT5616e_BIAS_CUR3, 0x280a);
	/* LOUT Differential enabled */
	regmap_write(rt5616e->regmap, RT5616e_LOUT_CTRL2, 0x8000);
	regmap_write(rt5616e->regmap, RT5616e_PWR_LDO_DACREF, 0xc300);
	if (rt5616e->hw_ver == RT5616e_VERA)
		regmap_write(rt5616e->regmap, RT5616e_IP_CTRL, 0x0600);
}

static int rt5616e_probe(struct snd_soc_codec *codec)
{
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	rt5616e->codec = codec;
	rt5616e_reg_init(codec);

	return 0;
}

#ifdef CONFIG_PM
static int rt5616e_suspend(struct snd_soc_codec *codec)
{
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	regcache_cache_only(rt5616e->regmap, true);
	regcache_mark_dirty(rt5616e->regmap);

	return 0;
}

static int rt5616e_resume(struct snd_soc_codec *codec)
{
	struct rt5616e_priv *rt5616e = snd_soc_codec_get_drvdata(codec);

	regcache_cache_only(rt5616e->regmap, false);
	regcache_sync(rt5616e->regmap);
	return 0;
}
#else
#define rt5616e_suspend NULL
#define rt5616e_resume NULL
#endif

#define RT5616e_STEREO_RATES SNDRV_PCM_RATE_8000_192000
#define RT5616e_FORMATS (SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE | \
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S8)

static struct snd_soc_dai_ops rt5616e_aif_dai_ops = {
	.hw_params = rt5616e_hw_params,
	.set_fmt = rt5616e_set_dai_fmt,
	.set_sysclk = rt5616e_set_dai_sysclk,
	.set_pll = rt5616e_set_dai_pll,
};

static struct snd_soc_dai_driver rt5616e_dai[] = {
	{
		.name = "rt5616e-aif1",
		.id = RT5616e_AIF1,
		.playback = {
			.stream_name = "AIF1 Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5616e_STEREO_RATES,
			.formats = RT5616e_FORMATS,
		},
		.capture = {
			.stream_name = "AIF1 Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5616e_STEREO_RATES,
			.formats = RT5616e_FORMATS,
		},
		.ops = &rt5616e_aif_dai_ops,
	},
};

static struct snd_soc_codec_driver soc_codec_dev_rt5616e = {
	.probe = rt5616e_probe,
	.suspend = rt5616e_suspend,
	.resume = rt5616e_resume,
	.set_bias_level = rt5616e_set_bias_level,
	.idle_bias_off = true,
	.component_driver = {
		.controls		= rt5616e_snd_controls,
		.num_controls		= ARRAY_SIZE(rt5616e_snd_controls),
		.dapm_widgets		= rt5616e_dapm_widgets,
		.num_dapm_widgets	= ARRAY_SIZE(rt5616e_dapm_widgets),
		.dapm_routes		= rt5616e_dapm_routes,
		.num_dapm_routes	= ARRAY_SIZE(rt5616e_dapm_routes),
	},
};

static const struct regmap_config rt5616e_regmap = {
	.reg_bits = 16,
	.val_bits = 16,
	.max_register = RT5616e_HP_AMP_DET3,
	.volatile_reg = rt5616e_volatile_register,
	.readable_reg = rt5616e_readable_register,
	.cache_type = REGCACHE_RBTREE,
	.reg_defaults = rt5616e_reg,
	.num_reg_defaults = ARRAY_SIZE(rt5616e_reg),
};

static const struct i2c_device_id rt5616e_i2c_id[] = {
	{ "rt5616e", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, rt5616e_i2c_id);

#if defined(CONFIG_OF)
static const struct of_device_id rt5616e_of_match[] = {
	{ .compatible = "realtek,rt5616e", },
	{},
};
MODULE_DEVICE_TABLE(of, rt5616e_of_match);
#endif

static int rt5616e_i2c_probe(struct i2c_client *i2c,
			    const struct i2c_device_id *id)
{
	struct rt5616e_priv *rt5616e;
	unsigned int val, hw_ver;
	int ret;

	rt5616e = devm_kzalloc(&i2c->dev, sizeof(struct rt5616e_priv),
			      GFP_KERNEL);
	if (!rt5616e)
		return -ENOMEM;

	i2c_set_clientdata(i2c, rt5616e);

	rt5616e->regmap = devm_regmap_init_i2c(i2c, &rt5616e_regmap);
	if (IS_ERR(rt5616e->regmap)) {
		ret = PTR_ERR(rt5616e->regmap);
		dev_err(&i2c->dev, "Failed to allocate register map: %d\n",
			ret);
		return ret;
	}

	regmap_read(rt5616e->regmap, RT5616e_DEVICE_ID, &val);
	if (val != 0x6680) {
		dev_err(&i2c->dev,
			"Device with ID register %#x is not rt5616e\n",
			val);
		return -ENODEV;
	}

	regmap_read(rt5616e->regmap, RT5616e_VERSION_ID, &hw_ver);
	if (hw_ver == RT5616e_VERA)
		rt5616e->hw_ver = RT5616e_VERA;
	else
		rt5616e->hw_ver = RT5616e_VERB;
	pr_info("RT5616E check hw_ver:%s\n",
		(hw_ver == RT5616e_VERA)?"VERA":"VERB");

	regmap_write(rt5616e->regmap, RT5616e_RESET, 0);

	return snd_soc_register_codec(&i2c->dev, &soc_codec_dev_rt5616e,
				      rt5616e_dai, ARRAY_SIZE(rt5616e_dai));
}

static int rt5616e_i2c_remove(struct i2c_client *i2c)
{
	snd_soc_unregister_codec(&i2c->dev);

	return 0;
}

static void rt5616e_i2c_shutdown(struct i2c_client *client)
{
	struct rt5616e_priv *rt5616e = i2c_get_clientdata(client);

	regmap_write(rt5616e->regmap, RT5616e_HP_VOL, 0xc8c8);
	regmap_write(rt5616e->regmap, RT5616e_LOUT_CTRL1, 0xc8c8);
}

static struct i2c_driver rt5616e_i2c_driver = {
	.driver = {
		.name = "rt5616e",
		.of_match_table = of_match_ptr(rt5616e_of_match),
	},
	.probe = rt5616e_i2c_probe,
	.remove = rt5616e_i2c_remove,
	.shutdown = rt5616e_i2c_shutdown,
	.id_table = rt5616e_i2c_id,
};
module_i2c_driver(rt5616e_i2c_driver);

MODULE_DESCRIPTION("ASoC RT5616e driver");
MODULE_AUTHOR("Jack Yu <jack.yu@realtek.com>");
MODULE_LICENSE("GPL v2");
