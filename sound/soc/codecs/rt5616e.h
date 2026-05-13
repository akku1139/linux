/*
 * rt5616e.h  --  RT5616e ALSA SoC audio driver
 *
 * Copyright 2020 Realtek Microelectronics
 * Author: Jack Yu <jack.yu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __RT5616E_H__
#define __RT5616E_H__

/* Info */
#define RT5616e_RESET				0x0000
#define RT5616e_VERSION_ID			0x00fd
#define RT5616e_VENDOR_ID			0x00fe
#define RT5616e_DEVICE_ID			0x00ff
/*  I/O - Output */
#define RT5616e_HP_VOL				0x0002
#define RT5616e_LOUT_CTRL1			0x0003
#define RT5616e_LOUT_CTRL2			0x0005
/* I/O - Input */
#define RT5616e_IN1_IN2				0x000d
/* I/O - DAC */
#define RT5616e_DAC1_DIG_VOL			0x0019
/* Mixer - D-D */
#define RT5616e_STO1_ADC_MIXER			0x0027
#define RT5616e_AD_DA_MIXER			0x0029
#define RT5616e_STO_DAC_MIXER			0x002a
/* PDM */
#define RT5616e_PDM_I2C_CTL1			0x0031
#define RT5616e_PDM_I2C_CTL2			0x0032
/* Mixer - ADC */
#define RT5616e_REC_L1_MIXER			0x003b
#define RT5616e_REC_L2_MIXER			0x003c
/* Mixer - DAC */
#define RT5616e_HPO_MIXER			0x0045
#define RT5616e_LOUT_MIXER			0x0053
/* Power */
#define RT5616e_PWR_DIG1			0x0061
#define RT5616e_PWR_ANLG1			0x0063
#define RT5616e_PWR_ANLG2			0x0064
#define RT5616e_PWR_MIXER			0x0065
#define RT5616e_PWR_VOL				0x0066
#define RT5616e_PWR_LDO_DACREF			0x0067
/* Function - Analog */
#define RT5616e_PLL_CTRL1			0x0081
#define RT5616e_PLL_CTRL2			0x0082
#define RT5616e_ADC_LPF_CTRL			0x0084
#define RT5616e_HP_OVCD				0x008b
#define RT5616e_DEPOP_M1			0x008e
#define RT5616e_DEPOP_M2			0x008f
#define RT5616e_DEPOP_M3			0x0090
#define RT5616e_CHARGE_PUMP			0x0091
#define RT5616e_MICBIAS				0x0093
/* Function - Digital */
#define RT5616e_JD_CTRL1			0x00bb
#define RT5616e_JD_CTRL2			0x00bc
#define RT5616e_IRQ_CTRL1			0x00bd
#define RT5616e_IRQ_CTRL2			0x00be
#define RT5616e_INT_IRQ_ST			0x00bf
#define RT5616e_HP_CALIB_AMP_DET		0x00d6
#define RT5616e_SV_ZCD1				0x00d9
#define RT5616e_SV_ZCD2				0x00da
#define RT5616e_D_MISC				0x00fa
#define RT5616e_DUMMY1				0x00fb
#define RT5616e_DUMMY2				0x00fc

#define RT5616e_BIAS_CUR1			0x0112
#define RT5616e_BIAS_CUR2			0x0113
#define RT5616e_BIAS_CUR3			0x0114
#define RT5616e_BIAS_CUR4			0x0115
#define RT5616e_BIAS_CUR5			0x0116
#define RT5616e_VERF_ADJ			0x0117
#define RT5616e_CP_INT1				0x0124
#define RT5616e_CP_INT2				0x0125
#define RT5616e_JDH1				0x0131
#define RT5616e_SOFT_DEPOP			0x0137
#define RT5616e_PLL_INT				0x0138
#define RT5616e_TEST_CTL1			0x013a
#define RT5616e_CHOP_DAC			0x013d
#define RT5616e_SOFT_DCC			0x013e
#define RT5616e_HP_DC_OST1			0x0141
#define RT5616e_HP_DC_OST2			0x0142
#define RT5616e_HP_DC_OST3			0x0143
#define RT5616e_HP_DC_OST4			0x0144
#define RT5616e_HP_DC_OST5			0x0145
#define RT5616e_DUMMY3				0x01fb
#define RT5616e_DUMMY4				0x01fc
#define RT5616e_SYS_CLK				0x0200
#define RT5616e_PLL_CTRL			0x0201
#define RT5616e_RC_CTRL				0x0202
#define RT5616e_OSR_CTRL			0x0203
#define RT5616e_FTR_CLK_CTRL			0x0204
#define RT5616e_TRACK_CTRL			0x0205
#define RT5616e_FIFO_CTRL			0x0206
#define RT5616e_I2S_CLK_CTRL			0x0207
#define RT5616e_IP_CTRL				0x0208
#define RT5616e_GFM_CTRL1			0x0209
#define RT5616e_GFM_CTRL2			0x020a
#define RT5616e_GPIO_HL_CTRL1			0x0300
#define RT5616e_GPIO_HL_CTRL2			0x0301
#define RT5616e_GPIO_CTRL1			0x0302
#define RT5616e_GPIO_DRV_CTRL1			0x0303
#define RT5616e_GPIO_DRV_CTRL2			0x0304
#define RT5616e_GPIO_TEST1			0x0305
#define RT5616e_GPIO_TEST2			0x0306
#define RT5616e_GPIO_TEST3			0x0307
#define RT5616e_GPIO_TEST4			0x0308
#define RT5616e_ASRC_CTRL1			0x0800
#define RT5616e_ASRC_CTRL2			0x0801
#define RT5616e_ASRC_CTRL3			0x0802
#define RT5616e_ADC_ADJ_CTRL1			0x0e00
#define RT5616e_ADC_ADJ_CTRL2			0x0e01
#define RT5616e_ADC_ADJ_CTRL3			0x0e02
#define RT5616e_ADC_ADJ_CTRL4			0x0e03
#define RT5616e_ADC_ADJ_CTRL5			0x0e04
#define RT5616e_ADC_ADJ_CTRL6			0x0e05
#define RT5616e_ADC_ADJ_CTRL7			0x0e06
#define RT5616e_ADC_ADJ_CTRL8			0x0e07
#define RT5616e_ADC_ADJ_CTRL9			0x0e08
#define RT5616e_DA_FTR_CTRL1			0x1800
#define RT5616e_DA_FTR_CTRL2			0x1801
#define RT5616e_DA_FTR_CTRL3			0x1802
#define RT5616e_GPIO_CTRL2			0x2200
#define RT5616e_HP_OFFSET_CALI1			0x3000
#define RT5616e_HP_OFFSET_CALI2			0x3001
#define RT5616e_HP_OFFSET_CALI3			0x3002
#define RT5616e_HP_OFFSET_CALI4			0x3003
#define RT5616e_HP_OFFSET_CALI5			0x3004
#define RT5616e_HP_OFFSET_CALI6			0x3005
#define RT5616e_HP_OFFSET_CALI7			0x3006
#define RT5616e_HP_OFFSET_CALI8			0x3007
#define RT5616e_HP_OFFSET_CALI9			0x3008
#define RT5616e_HP_AMP_DET1			0x3100
#define RT5616e_HP_AMP_DET2			0x3101
#define RT5616e_HP_AMP_DET3			0x3102


/* global definition */
#define RT5616e_L_MUTE				(0x1 << 15)
#define RT5616e_L_MUTE_SFT			15
#define RT5616e_VOL_L_MUTE			(0x1 << 14)
#define RT5616e_VOL_L_SFT			14
#define RT5616e_R_MUTE				(0x1 << 7)
#define RT5616e_R_MUTE_SFT			7
#define RT5616e_VOL_R_MUTE			(0x1 << 6)
#define RT5616e_VOL_R_SFT			6
#define RT5616e_L_VOL_MASK			(0x3f << 8)
#define RT5616e_L_VOL_SFT			8
#define RT5616e_R_VOL_MASK			(0x3f)
#define RT5616e_R_VOL_SFT			0

/* LOUT Control 2(0x0005) */
#define RT5616e_EN_DFO				(0x1 << 15)

/* IN1 and IN2 Control (0x000d) */
/* IN3 and IN4 Control (0x000e) */
#define RT5616e_BST_MASK1			(0xf<<12)
#define RT5616e_BST_SFT1			12
#define RT5616e_BST_20DB			(0x1<<12)
#define RT5616e_IN_DF1				(0x1 << 7)
#define RT5616e_IN_SFT1				7

/* DAC1 Digital Volume (0x0019) */
#define RT5616e_DAC_L1_VOL_MASK		(0xff << 8)
#define RT5616e_DAC_L1_VOL_SFT			8
#define RT5616e_DAC_R1_VOL_MASK		(0xff)
#define RT5616e_DAC_R1_VOL_SFT			0

/* Stereo ADC1 Mixer Control (0x0027) */
#define RT5616e_M_STO1_ADC_L1			(0x1 << 14)
#define RT5616e_M_STO1_ADC_L1_SFT		14
#define RT5616e_M_STO1_ADC_L2			(0x1 << 13)
#define RT5616e_M_STO1_ADC_L2_SFT		13
#define RT5616e_M_STO1_ADC_R1			(0x1 << 6)
#define RT5616e_M_STO1_ADC_R1_SFT		6
#define RT5616e_M_STO1_ADC_R2			(0x1 << 5)
#define RT5616e_M_STO1_ADC_R2_SFT		5

/* ADC Mixer to DAC Mixer Control (0x0029) */
#define RT5616e_M_ADCMIX_L				(0x1 << 15)
#define RT5616e_M_ADCMIX_L_SFT			15
#define RT5616e_M_IF1_DAC_L				(0x1 << 14)
#define RT5616e_M_IF1_DAC_L_SFT			14
#define RT5616e_M_ADCMIX_R				(0x1 << 7)
#define RT5616e_M_ADCMIX_R_SFT			7
#define RT5616e_M_IF1_DAC_R				(0x1 << 6)
#define RT5616e_M_IF1_DAC_R_SFT			6

/* Stereo DAC Mixer Control (0x002a) */
#define RT5616e_M_DAC_L1_MIXL			(0x1 << 14)
#define RT5616e_M_DAC_L1_MIXL_SFT		14
#define RT5616e_DAC_L1_STO_L_VOL_MASK		(0x1 << 13)
#define RT5616e_DAC_L1_STO_L_VOL_SFT		13
#define RT5616e_M_DAC_R1_MIXL			(0x1 << 9)
#define RT5616e_M_DAC_R1_MIXL_SFT		9
#define RT5616e_DAC_R1_STO_L_VOL_MASK		(0x1 << 8)
#define RT5616e_DAC_R1_STO_L_VOL_SFT		8
#define RT5616e_M_DAC_R1_MIXR			(0x1 << 6)
#define RT5616e_M_DAC_R1_MIXR_SFT		6
#define RT5616e_DAC_R1_STO_R_VOL_MASK		(0x1 << 5)
#define RT5616e_DAC_R1_STO_R_VOL_SFT		5
#define RT5616e_M_DAC_L1_MIXR			(0x1 << 1)
#define RT5616e_M_DAC_L1_MIXR_SFT		1
#define RT5616e_DAC_L1_STO_R_VOL_MASK		(0x1)
#define RT5616e_DAC_L1_STO_R_VOL_SFT		0

/* I2C / Data Control 2 (0x0032) */
#define RT5616e_I2C_CLK_STR_EN_SFT		4
#define RT5616e_I2C_SCL_CLR_SFT			3
#define RT5616e_I2C_BUSRT_EN_SFT		2
#define RT5616e_I2C_TIMEOUT_SFT			1
#define RT5616e_I2C_BUS_CLEAR_SFT		0


/* REC Left Mixer Control 1 (0x003b) */
#define RT5616e_G_LN_L2_RM_L_MASK		(0x7 << 13)
#define RT5616e_G_IN_L2_RM_L_SFT			13
#define RT5616e_G_LN_L1_RM_L_MASK		(0x7 << 10)
#define RT5616e_G_IN_L1_RM_L_SFT			10
#define RT5616e_G_BST3_RM_L_MASK		(0x7 << 4)
#define RT5616e_G_BST3_RM_L_SFT			4
#define RT5616e_G_BST2_RM_L_MASK		(0x7 << 1)
#define RT5616e_G_BST2_RM_L_SFT			1

/* REC Left Mixer Control 2 (0x003c) */
#define RT5616e_G_BST1_RM_L_MASK		(0x7 << 13)
#define RT5616e_G_BST1_RM_L_SFT			13
#define RT5616e_G_OM_L_RM_L_MASK		(0x7 << 10)
#define RT5616e_G_OM_L_RM_L_SFT			10
#define RT5616e_M_IN2_L_RM_L			(0x1 << 6)
#define RT5616e_M_IN2_L_RM_L_SFT			6
#define RT5616e_M_IN1_L_RM_L			(0x1 << 5)
#define RT5616e_M_IN1_L_RM_L_SFT			5
#define RT5616e_M_RSV_RM_L			(0x1 << 4)
#define RT5616e_M_RSV_RM_L_SFT			4
#define RT5616e_M_BST3_RM_L			(0x1 << 3)
#define RT5616e_M_BST3_RM_L_SFT			3
#define RT5616e_M_BST2_RM_L			(0x1 << 2)
#define RT5616e_M_BST2_RM_L_SFT			2
#define RT5616e_M_BST1_RM_L			(0x1 << 1)
#define RT5616e_M_BST1_RM_L_SFT			1
#define RT5616e_M_OM_L_RM_L			(0x1)
#define RT5616e_M_OM_L_RM_L_SFT			0

/* HPMIX Control (0x0045) */
#define RT5616e_M_DAC1_HM_SFT			14
#define RT5616e_MU_IN_HP_MASK			(0x7 << 13)
#define RT5616e_MU_IN_HP_SFT			13
#define RT5616e_S_HP_VOL			(0x0 << 13)
#define RT5616e_S_DAC				(0x1 << 13)
#define RT5616e_S_NONE				(0x2 << 13)

/* LOUT Mixer Control (0x0053) */
#define RT5616e_M_DAC_L1_LM			(0x1 << 15)
#define RT5616e_M_DAC_L1_LM_SFT			15
#define RT5616e_M_DAC_R1_LM			(0x1 << 14)
#define RT5616e_M_DAC_R1_LM_SFT			14
#define RT5616e_M_OV_L_LM			(0x1 << 13)
#define RT5616e_M_OV_L_LM_SFT			13
#define RT5616e_M_OV_R_LM			(0x1 << 12)
#define RT5616e_M_OV_R_LM_SFT			12
#define RT5616e_G_LOUTMIX_MASK			(0x1 << 11)
#define RT5616e_G_LOUTMIX_SFT			11

/* Power Management for Digital 1 (0x0061) */
#define RT5616e_PWR_DAC_L1			(0x1 << 12)
#define RT5616e_PWR_DAC_L1_BIT			12
#define RT5616e_PWR_DAC_R1			(0x1 << 11)
#define RT5616e_PWR_DAC_R1_BIT			11
#define RT5616e_PWR_ADC_L			(0x1 << 2)
#define RT5616e_PWR_ADC_L_BIT			2
#define RT5616e_PWR_ADC_R			(0x1 << 1)
#define RT5616e_PWR_ADC_R_BIT			1

/* Power Management for Analog 1 (0x0063) */
#define RT5616e_PWR_VREF1			(0x1 << 15)
#define RT5616e_PWR_VREF1_BIT			15
#define RT5616e_PWR_FV1				(0x1 << 14)
#define RT5616e_PWR_FV1_BIT			14
#define RT5616e_PWR_MB				(0x1 << 13)
#define RT5616e_PWR_MB_BIT			13
#define RT5616e_PWR_LM				(0x1 << 12)
#define RT5616e_PWR_LM_BIT			12
#define RT5616e_PWR_BG				(0x1 << 11)
#define RT5616e_PWR_BG_BIT			11
#define RT5616e_PWR_HP_L			(0x1 << 7)
#define RT5616e_PWR_HP_L_BIT			7
#define RT5616e_PWR_HP_R			(0x1 << 6)
#define RT5616e_PWR_HP_R_BIT			6
#define RT5616e_PWR_HA				(0x1 << 5)
#define RT5616e_PWR_HA_BIT			5
#define RT5616e_PWR_VREF2			(0x1 << 4)
#define RT5616e_PWR_VREF2_BIT			4
#define RT5616e_PWR_FV2				(0x1 << 3)
#define RT5616e_PWR_FV2_BIT			3
#define RT5616e_PWR_LDO				(0x1 << 2)
#define RT5616e_PWR_LDO_BIT			2
#define RT5616e_PWR_LDO_DVO_MASK		(0x3)
#define RT5616e_PWR_LDO_DVO_1_0V		0
#define RT5616e_PWR_LDO_DVO_1_1V		1
#define RT5616e_PWR_LDO_DVO_1_2V		2
#define RT5616e_PWR_LDO_DVO_1_4V		3

/* Power Management for Analog 2 (0x0064) */
#define RT5616e_PWR_BST1			(0x1 << 15)
#define RT5616e_PWR_BST1_BIT			15
#define RT5616e_PWR_MB3				(0x1 << 11)
#define RT5616e_PWR_MB3_BIT			11
#define RT5616e_PWR_BST1_OP2			(0x1 << 5)
#define RT5616e_PWR_BST1_OP2_BIT		5
#define RT5616e_PWR_BST2_OP2			(0x1 << 4)
#define RT5616e_PWR_BST2_OP2_BIT		4
#define RT5616e_PWR_JD_M			(0x1 << 2)
#define RT5616e_PWM_JD_M_BIT			2

/* Power Management for Mixer (0x0065) */
#define RT5616e_PWR_RM_L			(0x1 << 11)
#define RT5616e_PWR_RM_L_BIT			11

/* Power Management for Volume (0x0066) */
#define RT5616e_PWR_OV_L			(0x1 << 13)
#define RT5616e_PWR_OV_L_BIT			13
#define RT5616e_PWR_OV_R			(0x1 << 12)
#define RT5616e_PWR_OV_R_BIT			12
#define RT5616e_PWR_HV_L			(0x1 << 11)
#define RT5616e_PWR_HV_L_BIT			11
#define RT5616e_PWR_HV_R			(0x1 << 10)
#define RT5616e_PWR_HV_R_BIT			10

/* Power Management for Internal LDO DACREF (0x0067) */
#define RT5616e_DL_DACREF_MASK			(0x3 << 14)
#define RT5616e_DL_1_607			(0x0 << 14)
#define RT5616e_DL_1_5				(0x1 << 14)
#define RT5616e_DL_1_406			(0x2 << 14)
#define RT5616e_DL_1_731			(0x3 << 14)
#define RT5616e_LDO_ISEL			(0x1 << 12)
#define RT5616e_LDO_PL				(0x1 << 11)
#define RT5616e_LDO_OCP				(0x1 << 9)
#define RT5616e_LDO_DACREF			(0x1 << 8)

/* PLL M/N/K Code Control 1 (0x0081) */
#define RT5616e_PLL_N_MAX			0x1ff
#define RT5616e_PLL_N_MASK			(RT5616e_PLL_N_MAX << 7)
#define RT5616e_PLL_N_SFT			7
#define RT5616e_PLL_K_MAX			0x1f
#define RT5616e_PLL_K_MASK			(RT5616e_PLL_K_MAX)
#define RT5616e_PLL_K_SFT			0

/* PLL M/N/K Code Control 2 (0x0082) */
#define RT5616e_PLL_M_MAX			0xf
#define RT5616e_PLL_M_MASK			(RT5616e_PLL_M_MAX << 12)
#define RT5616e_PLL_M_SFT			12
#define RT5616e_PLL_M_BP			(0x1 << 11)
#define RT5616e_PLL_M_BP_SFT			11
#define RT5616e_RESET_PLL_MASK			(0x1 << 10)
#define RT5616e_RESET_PLL			(0x0 << 10)
#define RT5616e_NO_RESET_PLL			(0x1 << 10)

/* HPOUT Over Current Detection (0x008b) */
#define RT5616e_HP_OVCD_MASK			(0x1 << 10)
#define RT5616e_HP_OVCD_SFT			10
#define RT5616e_HP_OVCD_DIS			(0x0 << 10)
#define RT5616e_HP_OVCD_EN			(0x1 << 10)
#define RT5616e_HP_OC_TH_MASK			(0x3 << 8)
#define RT5616e_HP_OC_TH_SFT			8
#define RT5616e_HP_OC_TH_90			(0x0 << 8)
#define RT5616e_HP_OC_TH_105			(0x1 << 8)
#define RT5616e_HP_OC_TH_120			(0x2 << 8)
#define RT5616e_HP_OC_TH_135			(0x3 << 8)

/* Depop Mode Control 1 (0x008e) */
#define RT5616e_SMT_TRIG_MASK			(0x1 << 15)
#define RT5616e_SMT_TRIG_SFT			15
#define RT5616e_SMT_TRIG_DIS			(0x0 << 15)
#define RT5616e_SMT_TRIG_EN			(0x1 << 15)
#define RT5616e_HP_L_SMT_MASK			(0x1 << 9)
#define RT5616e_HP_L_SMT_SFT			9
#define RT5616e_HP_L_SMT_DIS			(0x0 << 9)
#define RT5616e_HP_L_SMT_EN			(0x1 << 9)
#define RT5616e_HP_R_SMT_MASK			(0x1 << 8)
#define RT5616e_HP_R_SMT_SFT			8
#define RT5616e_HP_R_SMT_DIS			(0x0 << 8)
#define RT5616e_HP_R_SMT_EN			(0x1 << 8)
#define RT5616e_HP_CD_PD_MASK			(0x1 << 7)
#define RT5616e_HP_CD_PD_SFT			7
#define RT5616e_HP_CD_PD_DIS			(0x0 << 7)
#define RT5616e_HP_CD_PD_EN			(0x1 << 7)
#define RT5616e_RSTN_MASK			(0x1 << 6)
#define RT5616e_RSTN_SFT				6
#define RT5616e_RSTN_DIS				(0x0 << 6)
#define RT5616e_RSTN_EN				(0x1 << 6)
#define RT5616e_RSTP_MASK			(0x1 << 5)
#define RT5616e_RSTP_SFT				5
#define RT5616e_RSTP_DIS				(0x0 << 5)
#define RT5616e_RSTP_EN				(0x1 << 5)
#define RT5616e_HP_CO_MASK			(0x1 << 4)
#define RT5616e_HP_CO_SFT			4
#define RT5616e_HP_CO_DIS			(0x0 << 4)
#define RT5616e_HP_CO_EN			(0x1 << 4)
#define RT5616e_HP_CP_MASK			(0x1 << 3)
#define RT5616e_HP_CP_SFT			3
#define RT5616e_HP_CP_PD			(0x0 << 3)
#define RT5616e_HP_CP_PU			(0x1 << 3)
#define RT5616e_HP_SG_MASK			(0x1 << 2)
#define RT5616e_HP_SG_SFT			2
#define RT5616e_HP_SG_DIS			(0x0 << 2)
#define RT5616e_HP_SG_EN			(0x1 << 2)
#define RT5616e_HP_DP_MASK			(0x1 << 1)
#define RT5616e_HP_DP_SFT			1
#define RT5616e_HP_DP_PD			(0x0 << 1)
#define RT5616e_HP_DP_PU			(0x1 << 1)
#define RT5616e_HP_CB_MASK			(0x1)
#define RT5616e_HP_CB_SFT			0
#define RT5616e_HP_CB_PD			(0x0)
#define RT5616e_HP_CB_PU			(0x1)

/* Depop Mode Control 2 (0x008f) */
#define RT5616e_DEPOP_MASK			(0x1 << 13)
#define RT5616e_DEPOP_SFT			13
#define RT5616e_DEPOP_AUTO			(0x0 << 13)
#define RT5616e_DEPOP_MAN			(0x1 << 13)
#define RT5616e_RAMP_MASK			(0x1 << 12)
#define RT5616e_RAMP_SFT			12
#define RT5616e_RAMP_DIS			(0x0 << 12)
#define RT5616e_RAMP_EN				(0x1 << 12)
#define RT5616e_BPS_MASK			(0x1 << 11)
#define RT5616e_BPS_SFT				11
#define RT5616e_BPS_DIS				(0x0 << 11)
#define RT5616e_BPS_EN				(0x1 << 11)
#define RT5616e_FAST_UPDN_MASK			(0x1 << 10)
#define RT5616e_FAST_UPDN_SFT			10
#define RT5616e_FAST_UPDN_DIS			(0x0 << 10)
#define RT5616e_FAST_UPDN_EN			(0x1 << 10)
#define RT5616e_MRES_MASK			(0x3 << 8)
#define RT5616e_MRES_SFT				8
#define RT5616e_MRES_15MO			(0x0 << 8)
#define RT5616e_MRES_25MO			(0x1 << 8)
#define RT5616e_MRES_35MO			(0x2 << 8)
#define RT5616e_MRES_45MO			(0x3 << 8)
#define RT5616e_VLO_MASK			(0x1 << 7)
#define RT5616e_VLO_SFT				7
#define RT5616e_VLO_3V				(0x0 << 7)
#define RT5616e_VLO_32V				(0x1 << 7)
#define RT5616e_DIG_DP_MASK			(0x1 << 6)
#define RT5616e_DIG_DP_SFT			6
#define RT5616e_DIG_DP_DIS			(0x0 << 6)
#define RT5616e_DIG_DP_EN			(0x1 << 6)
#define RT5616e_DP_TH_MASK			(0x3 << 4)
#define RT5616e_DP_TH_SFT			4
#define RT5616e_DP_4096				(0x2 << 4)

/* Depop Mode Control 3 (0x0090) */
#define RT5616e_CP_SYS_MASK			(0x7 << 12)
#define RT5616e_CP_SYS_SFT			12
#define RT5616e_CP_FQ1_MASK			(0x7 << 8)
#define RT5616e_CP_FQ1_SFT			8
#define RT5616e_CP_FQ2_MASK			(0x7 << 4)
#define RT5616e_CP_FQ2_SFT			4
#define RT5616e_CP_FQ3_MASK			(0x7)
#define RT5616e_CP_FQ3_SFT			0
#define RT5616e_CP_FQ_1_5_KHZ			0
#define RT5616e_CP_FQ_3_KHZ			1
#define RT5616e_CP_FQ_6_KHZ			2
#define RT5616e_CP_FQ_12_KHZ			3
#define RT5616e_CP_FQ_24_KHZ			4
#define RT5616e_CP_FQ_48_KHZ			5
#define RT5616e_CP_FQ_96_KHZ			6
#define RT5616e_CP_FQ_192_KHZ			7

/* HPOUT charge pump (0x0091) */
#define RT5616e_OSW_L_MASK			(0x1 << 11)
#define RT5616e_OSW_L_SFT			11
#define RT5616e_OSW_L_DIS			(0x0 << 11)
#define RT5616e_OSW_L_EN			(0x1 << 11)
#define RT5616e_OSW_R_MASK			(0x1 << 10)
#define RT5616e_OSW_R_SFT			10
#define RT5616e_OSW_R_DIS			(0x0 << 10)
#define RT5616e_OSW_R_EN			(0x1 << 10)
#define RT5616e_IB_HP_MASK			(0x3 << 6)
#define RT5616e_IB_HP_SFT			6
#define RT5616e_IB_HP_125IL			(0x0 << 6)
#define RT5616e_IB_HP_25IL			(0x1 << 6)
#define RT5616e_IB_HP_5IL			(0x2 << 6)
#define RT5616e_IB_HP_1IL			(0x3 << 6)

/* Micbias Control (0x0093) */
#define RT5616e_MIC3_BS_MASK			(0x3 << 14)
#define RT5616e_MIC3_BS_SFT			14
#define RT5616e_MIC3_BS_2_7V			(0x0 << 14)
#define RT5616e_MIC3_BS_2_4V			(0x1 << 14)
#define RT5616e_MIC3_BS_2_25V			(0x2 << 14)
#define RT5616e_MIC3_BS_1_8V			(0x3 << 14)
#define RT5616e_MIC3_CLK_MASK			(0x1 << 13)
#define RT5616e_MIC3_CLK_SFT			13
#define RT5616e_MIC3_CLK_DIS			(0x0 << 13)
#define RT5616e_MIC3_CLK_EN			(0x1 << 13)
#define RT5616e_MIC3_OVCD_MASK			(0x1 << 11)
#define RT5616e_MIC3_OVCD_SFT			11
#define RT5616e_MIC3_OVCD_DIS			(0x0 << 11)
#define RT5616e_MIC3_OVCD_EN			(0x1 << 11)
#define RT5616e_MIC3_OVTH_MASK			(0x3 << 9)
#define RT5616e_MIC3_OVTH_SFT			9
#define RT5616e_MIC3_OVTH_600UA			(0x0 << 9)
#define RT5616e_MIC3_OVTH_1500UA		(0x1 << 9)
#define RT5616e_MIC3_OVTH_2000UA		(0x2 << 9)
#define RT5616e_PWR_MB_MASK			(0x1 << 5)
#define RT5616e_PWR_MB_SFT			5
#define RT5616e_PWR_MB_PD			(0x0 << 5)
#define RT5616e_PWR_MB_PU			(0x1 << 5)
#define RT5616e_IRQ_DEB_SRC_MASK		(0x1 << 3)
#define RT5616e_IRQ_DEB_SRC_SFT			3
#define RT5616e_IRQ_DEB_MCLK			(0x0 << 3)
#define RT5616e_IRQ_DEB_RC			(0x1 << 3)

/* Jack Detect Control 1 (0x00bb) */
#define RT5616e_GPIO_JD_MASK			(0x1 << 15)
#define RT5616e_JD_GPIO1			(0x0 << 15)
#define RT5616e_JD_GPIO2			(0x1 << 15)
#define RT5616e_JD_HP_MASK			(0x1 << 11)
#define RT5616e_JD_HP_SFT			11
#define RT5616e_JD_HP_DIS			(0x0 << 11)
#define RT5616e_JD_HP_EN			(0x1 << 11)
#define RT5616e_JD_HP_TRG_MASK			(0x1 << 10)
#define RT5616e_JD_HP_TRG_SFT			10
#define RT5616e_JD_HP_TRG_LO			(0x0 << 10)
#define RT5616e_JD_HP_TRG_HI			(0x1 << 10)
#define RT5616e_JD_LO_MASK			(0x1 << 3)
#define RT5616e_JD_LO_SFT			3
#define RT5616e_JD_LO_DIS			(0x0 << 3)
#define RT5616e_JD_LO_EN			(0x1 << 3)
#define RT5616e_JD_LO_TRG_MASK			(0x1 << 2)
#define RT5616e_JD_LO_TRG_SFT			2
#define RT5616e_JD_LO_TRG_LO			(0x0 << 2)
#define RT5616e_JD_LO_TRG_HI			(0x1 << 2)

/* Jack Detect Control 2 (0x00bc) */
#define RT5616e_JD_TRG_SEL_MASK			(0x1 << 9)
#define RT5616e_JD_TRG_SEL_SFT			9
#define RT5616e_JD_STA_GPIO_JD			(0x0 << 9)
#define RT5616e_JD_STA_JDH1			(0x1 << 9)

/* IRQ Control 1 (0x00bd) */
#define RT5616e_IRQ_JD_MASK			(0x1 << 15)
#define RT5616e_IRQ_JD_SFT			15
#define RT5616e_IRQ_JD_BP			(0x0 << 15)
#define RT5616e_IRQ_JD_NOR			(0x1 << 15)
#define RT5616e_JD_STKY_MASK			(0x1 << 13)
#define RT5616e_JD_STKY_SFT			13
#define RT5616e_JD_STKY_DIS			(0x0 << 13)
#define RT5616e_JD_STKY_EN			(0x1 << 13)
#define RT5616e_JD_P_MASK			(0x1 << 11)
#define RT5616e_JD_P_SFT				11
#define RT5616e_JD_P_NOR			(0x0 << 11)
#define RT5616e_JD_P_INV			(0x1 << 11)
#define RT5616e_JD1_1_IRQ_EN			(0x1 << 9)
#define RT5616e_JD1_1_IRQ_EN_SFT		9
#define RT5616e_JD1_1_EN_STKY			(0x1 << 8)
#define RT5616e_JD1_1_EN_STKY_SFT		8
#define RT5616e_JD1_1_INV			(0x1 << 7)
#define RT5616e_JD1_1_INV_SFT			7

/* IRQ Control 2 (0x00be) */
#define RT5616e_IRQ_MB1_OC_MASK			(0x1 << 15)
#define RT5616e_IRQ_MB1_OC_SFT			15
#define RT5616e_IRQ_MB1_OC_BP			(0x0 << 15)
#define RT5616e_IRQ_MB1_OC_NOR			(0x1 << 15)
#define RT5616e_MB1_OC_STKY_MASK		(0x1 << 11)
#define RT5616e_MB1_OC_STKY_SFT			11
#define RT5616e_MB1_OC_STKY_DIS			(0x0 << 11)
#define RT5616e_MB1_OC_STKY_EN			(0x1 << 11)
#define RT5616e_MB1_OC_P_MASK			(0x1 << 7)
#define RT5616e_MB1_OC_P_SFT			7
#define RT5616e_MB1_OC_P_NOR			(0x0 << 7)
#define RT5616e_MB1_OC_P_INV			(0x1 << 7)
#define RT5616e_MB1_OC_CLR			(0x1 << 3)

/* Internal Status and GPIO status (0x00bf) */
#define RT5616e_STA_JD1_1			(0x1 << 12)
#define RT5616e_STA_JD1_1_BIT			12
#define RT5616e_STA_GP_JD			(0x1 << 4)
#define RT5616e_STA_GP_JD_BIT			4

/* Soft volume and zero cross control 1 (0x00d9) */
#define RT5616e_OUT_SV_MASK			(0x1 << 13)
#define RT5616e_OUT_SV_SFT			13
#define RT5616e_OUT_SV_DIS			(0x0 << 13)
#define RT5616e_OUT_SV_EN			(0x1 << 13)
#define RT5616e_HP_SV_MASK			(0x1 << 12)
#define RT5616e_HP_SV_SFT			12
#define RT5616e_HP_SV_DIS			(0x0 << 12)
#define RT5616e_HP_SV_EN			(0x1 << 12)
#define RT5616e_ZCD_DIG_MASK			(0x1 << 11)
#define RT5616e_ZCD_DIG_SFT			11
#define RT5616e_ZCD_DIG_DIS			(0x0 << 11)
#define RT5616e_ZCD_DIG_EN			(0x1 << 11)
#define RT5616e_ZCD_MASK			(0x1 << 10)
#define RT5616e_ZCD_SFT				10
#define RT5616e_ZCD_PD				(0x0 << 10)
#define RT5616e_ZCD_PU				(0x1 << 10)
#define RT5616e_M_ZCD_MASK			(0x3f << 4)
#define RT5616e_M_ZCD_SFT			4
#define RT5616e_M_ZCD_OM_L			(0x1 << 7)
#define RT5616e_M_ZCD_OM_R			(0x1 << 6)
#define RT5616e_M_ZCD_RM_L			(0x1 << 5)
#define RT5616e_M_ZCD_RM_R			(0x1 << 4)
#define RT5616e_SV_DLY_MASK			(0xf)
#define RT5616e_SV_DLY_SFT			0

/* Soft volume and zero cross control 2 (0x00da) */
#define RT5616e_ZCD_HP_MASK			(0x1 << 15)
#define RT5616e_ZCD_HP_SFT			15
#define RT5616e_ZCD_HP_DIS			(0x0 << 15)
#define RT5616e_ZCD_HP_EN			(0x1 << 15)

/* Digital Misc Control (0x00fa) */
#define RT5616e_AMP_DET_EN			(0x1 << 1)
#define RT5616e_AMP_DET_EN_SFT			1
#define RT5616e_D_GATE_EN			(0x1)
#define RT5616e_D_GATE_EN_SFT			0

/* Chopper control for DAC (0x013d) */
#define RT5616e_CKXSELADC_MASK			(0x1 << 15)
#define RT5616e_CKXSELADC_DIV8			(0x1 << 15)
#define RT5616e_CKXSELADC_DIV4			(0x0 << 15)
#define RT5616e_CKXADC_MASK			(0x1 << 13)
#define RT5616e_CKXADC_EN			(0x1 << 13)
#define RT5616e_CKXADC_DIS			(0x0 << 13)
#define RT5616e_EN_CKXADC_MASK			(0x1 << 12)
#define RT5616e_EN_CKXADC_EN			(0x1 << 12)
#define RT5616e_EN_CKXADC_DIS			(0x0 << 12)
#define RT5616e_CKXSEL_MASK			(0x1 << 11)
#define RT5616e_CK2_DIV8			(0x1 << 11)
#define RT5616e_CK2_DIV4			(0x0 << 11)
#define RT5616e_CKXEN_MASK			(0x1 << 10)
#define RT5616e_CKXEN_EN			(0x1 << 10)
#define RT5616e_CKXEN_DIS			(0x0 << 10)
#define RT5616e_EN_CKXGEN_MASK			(0x1 << 9)
#define RT5616e_EN_CKXGEN_EN			(0x1 << 9)
#define RT5616e_EN_CKXGEN_DIS			(0x0 << 9)

/* HP DC offset 1  (0x0141) */
#define RT5616e_M_STO_DAC_L1_MIXL_SFT		11
#define RT5616e_M_STO_DAC_R1_MIXR_SFT		10
#define RT5616e_M_HP_MANU_L_SFT			9
#define RT5616e_M_HP_MANU_R_SFT			8

/* System Clock Control  (0x0200) */
#define RT5616e_CLK_SYS_DET_MASK		(0x1 << 8)
#define RT5616e_CLK_SYS_DET_DIS			(0x0 << 8)
#define RT5616e_CLK_SYS_DET_EN			(0x1 << 8)
#define RT5616e_PW_CLK_INT_DIV_MASK		(0x1 << 7)
#define RT5616e_PW_CLK_INT_DIV_DIS		(0x0 << 7)
#define RT5616e_PW_CLK_INT_DIV_EN		(0x1 << 7)
#define RT5616e_PW_CLK_SYSLP_DIV_MASK		(0x1 << 6)
#define RT5616e_PW_CLK_SYSLP_DIV_DIS		(0x0 << 6)
#define RT5616e_PW_CLK_SYSLP_DIV_EN		(0x1 << 6)
#define RT5616e_PW_CLK_SYS_DIV_MASK		(0x1 << 5)
#define RT5616e_PW_CLK_SYS_DIV_DIS		(0x0 << 5)
#define RT5616e_PW_CLK_SYS_DIV_EN		(0x1 << 5)
#define RT5616e_PW_CLK_SYS_DIV_SFT		5
#define RT5616e_PW_CLK_SYS_MASK			(0x1 << 4)
#define RT5616e_PW_CLK_SYS_DIS			(0x0 << 4)
#define RT5616e_PW_CLK_SYS_EN			(0x1 << 4)
#define RT5616e_PW_CLK_SYS_SFT			4
#define RT5616e_SEL_SYSCLK_MASK			0x3
#define RT5616e_SEL_SYSCLK_MCLK			0x0
#define RT5616e_SEL_SYSCLK_PLL			0x1
#define RT5616e_SEL_SYSCLK_RC			0x2

/* PLL Control  (0x0201) */
#define RT5616e_SEL_PLL_SRC_MASK		(0x3 << 4)
#define RT5616e_SEL_PLL_SRC_MCLK		(0x0 << 4)
#define RT5616e_SEL_PLL_SRC_BCLK		(0x1 << 4)
#define RT5616e_SEL_PLL_SRC_RC			(0x2 << 4)
#define RT5616e_PLL_ON_OFF_MASK			0x1
#define RT5616e_PLL_ON_OFF_SFT			0
#define RT5616e_PLL_OFF				0x0
#define RT5616e_PLL_ON				0x1

/* RC Control  (0x0202) */
#define RT5616e_RC_ON_OFF_MASK			0x1
#define RT5616e_RC_ON_OFF_SFT			0
#define RT5616e_RC_OFF				0x0
#define RT5616e_RC_ON				0x1

/* OSR Control  (0x0203) */
#define RT5616e_SEL_ADC_OSR_MASK		(0xf << 4)
#define RT5616e_SEL_ADC_OSR_DIS			(0x0 << 4)
#define RT5616e_SEL_ADC_OSR_DIV1		(0x1 << 4)
#define RT5616e_SEL_ADC_OSR_DIV2		(0x2 << 4)
#define RT5616e_SEL_ADC_OSR_DIV4		(0x3 << 4)
#define RT5616e_SEL_ADC_OSR_DIV6		(0x4 << 4)
#define RT5616e_SEL_ADC_OSR_DIV8		(0x5 << 4)
#define RT5616e_SEL_ADC_OSR_DIV12		(0x6 << 4)
#define RT5616e_SEL_ADC_OSR_DIV16		(0x7 << 4)
#define RT5616e_SEL_ADC_OSR_DIV24		(0x8 << 4)
#define RT5616e_SEL_ADC_OSR_LPDIV1		(0x9 << 4)
#define RT5616e_SEL_ADC_OSR_LPDIV2		(0xa << 4)
#define RT5616e_SEL_DAC_OSR_MASK		0xf
#define RT5616e_SEL_DAC_OSR_DIS			0x0
#define RT5616e_SEL_DAC_OSR_DIV1		0x1
#define RT5616e_SEL_DAC_OSR_DIV2		0x2
#define RT5616e_SEL_DAC_OSR_DIV4		0x3
#define RT5616e_SEL_DAC_OSR_DIV6		0x4
#define RT5616e_SEL_DAC_OSR_DIV8		0x5
#define RT5616e_SEL_DAC_OSR_DIV12		0x6
#define RT5616e_SEL_DAC_OSR_DIV16		0x7
#define RT5616e_SEL_DAC_OSR_DIV24		0x8
#define RT5616e_SEL_DAC_OSR_LPDIV1		0x9
#define RT5616e_SEL_DAC_OSR_LPDIV2		0xa

/* Filter Clock Control  (0x0204) */
#define RT5616e_ADC_FIL_CLK_MASK		(0xf << 4)
#define RT5616e_ADC_FIL_CLK_SFT			4
#define RT5616e_ADC_FIL_CLK_DIS			(0x0 << 4)
#define RT5616e_ADC_FIL_CLK_DIV1		(0x1 << 4)
#define RT5616e_ADC_FIL_CLK_DIV2		(0x2 << 4)
#define RT5616e_ADC_FIL_CLK_DIV4		(0x3 << 4)
#define RT5616e_ADC_FIL_CLK_DIV6		(0x4 << 4)
#define RT5616e_ADC_FIL_CLK_DIV8		(0x5 << 4)
#define RT5616e_ADC_FIL_CLK_DIV12		(0x6 << 4)
#define RT5616e_ADC_FIL_CLK_DIV16		(0x7 << 4)
#define RT5616e_ADC_FIL_CLK_DIV24		(0x8 << 4)
#define RT5616e_ADC_FIL_CLK_LPDIV1		(0x9 << 4)
#define RT5616e_ADC_FIL_CLK_LPDIV2		(0xa << 4)
#define RT5616e_DAC_FIL_CLK_MASK		0xf
#define RT5616e_DAC_FIL_CLK_SFT			0
#define RT5616e_DAC_FIL_CLK_DIS			0x0
#define RT5616e_DAC_FIL_CLK_DIV1		0x1
#define RT5616e_DAC_FIL_CLK_DIV2		0x2
#define RT5616e_DAC_FIL_CLK_DIV4		0x3
#define RT5616e_DAC_FIL_CLK_DIV6		0x4
#define RT5616e_DAC_FIL_CLK_DIV8		0x5
#define RT5616e_DAC_FIL_CLK_DIV12		0x6
#define RT5616e_DAC_FIL_CLK_DIV16		0x7
#define RT5616e_DAC_FIL_CLK_DIV24		0x8
#define RT5616e_DAC_FIL_CLK_LPDIV1		0x9
#define RT5616e_DAC_FIL_CLK_LPDIV2		0xa

/* FIFO Clock Control   (0x0206) */
#define RT5616e_FIFO_DIV_MASK			0x7
#define RT5616e_FIFO_DIV_DIS			0x0
#define RT5616e_FIFO_DIV_DIV1			0x1
#define RT5616e_FIFO_DIV_DIV2			0x2
#define RT5616e_FIFO_DIV_DIV4			0x3
#define RT5616e_FIFO_DIV_DIV8			0x4

/* I2S Clock Control   (0x0207) */
#define RT5616e_I2S_EN_MASK			(0x1 << 9)
#define RT5616e_I2S_EN_SFT			9
#define RT5616e_I2S_DIS				(0x0 << 9)
#define RT5616e_I2S_EN				(0x1 << 9)
#define RT5616e_SEL_MAS_I2S_MASK		(0xf << 4)
#define RT5616e_SEL_MAS_I2S_SFT			4
#define RT5616e_SEL_MAS_I2S_DIS			(0x0 << 4)
#define RT5616e_SEL_MAS_I2S_DIV2		(0x1 << 4)
#define RT5616e_SEL_MAS_I2S_DIV4		(0x2 << 4)
#define RT5616e_SEL_MAS_I2S_DIV8		(0x3 << 4)
#define RT5616e_SEL_MAS_I2S_DIV12		(0x4 << 4)
#define RT5616e_SEL_MAS_I2S_DIV16		(0x5 << 4)
#define RT5616e_SEL_MAS_I2S_DIV24		(0x6 << 4)
#define RT5616e_SEL_MAS_I2S_DIV32		(0x7 << 4)
#define RT5616e_SEL_MAS_I2S_DIV48		(0x8 << 4)
#define RT5616e_I2S_TCON_ROOT_MASK		0x3
#define RT5616e_I2S_TCON_ROOT_SFT		0
#define RT5616e_I2S_TCON_ROOT_MCLK		0x0
#define RT5616e_I2S_TCON_ROOT_PLL		0x1
#define RT5616e_I2S_TCON_ROOT_RC		0x2

/* IP Control   (0x0208) */
#define RT5616e_CG_CLBR_MASK			(0x1 << 10)
#define RT5616e_CG_CLBR_SFT			10
#define RT5616e_CG_CLBR_DIS			(0x0 << 10)
#define RT5616e_CG_CLBR_EN			(0x1 << 10)
#define RT5616e_PWR_CLBR_MASK			(0x1 << 9)
#define RT5616e_PWR_CLBR_SFT			9
#define RT5616e_PWR_CLBR_OFF			(0x0 << 9)
#define RT5616e_PWR_CLBR_ON			(0x1 << 9)
#define RT5616e_PWR_SVOL_MASK			(0x1 << 8)
#define RT5616e_PWR_SVOL_SFT			8
#define RT5616e_PWR_SVOL_OFF			(0x0 << 8)
#define RT5616e_PWR_SVOL_ON			(0x1 << 8)
#define RT5616e_STO1_DAC_MASK			(0x1 << 5)
#define RT5616e_STO1_DAC_SFT			5
#define RT5616e_STO1_DAC_OFF			(0x0 << 5)
#define RT5616e_STO1_DAC_ON			(0x1 << 5)
#define RT5616e_ADC_ASRC_MASK			(0x1 << 4)
#define RT5616e_ADC_ASRC_SFT			4
#define RT5616e_ADC_ASRC_DIS			(0x0 << 4)
#define RT5616e_ADC_ASRC_EN			(0x1 << 4)
#define RT5616e_DAC_ASRC_MASK			(0x1 << 3)
#define RT5616e_DAC_ASRC_SFT			3
#define RT5616e_DAC_ASRC_DIS			(0x0 << 3)
#define RT5616e_DAC_ASRC_EN			(0x1 << 3)
#define RT5616e_ASRC_EN_MASK			(0x1 << 2)
#define RT5616e_ASRC_EN_SFT			2
#define RT5616e_ASRC_EN_DIS			(0x0 << 2)
#define RT5616e_ASRC_EN_EN			(0x1 << 2)
#define RT5616e_ADC_FIL_MASK			(0x1 << 1)
#define RT5616e_ADC_FIL_SFT			1
#define RT5616e_ADC_FIL_DIS			(0x0 << 1)
#define RT5616e_ADC_FIL_EN			(0x1 << 1)
#define RT5616e_DAC_FIL_MASK			0x1
#define RT5616e_DAC_FIL_SFT			0
#define RT5616e_DAC_FIL_DIS			0x0
#define RT5616e_DAC_FIL_EN			0x1

/* GPIO Control   (0x0302) */
#define RT5616e_SEL_GPIO4_MASK			(0x1 << 7)
#define RT5616e_SEL_GPIO4_INPUT			(0x0 << 7)
#define RT5616e_SEL_GPIO4_OUTPUT		(0x1 << 7)
#define RT5616e_GPIO4_LOGIC_MASK		(0x1 << 6)
#define RT5616e_GPIO4_LOGIC_LOW			(0x0 << 6)
#define RT5616e_GPIO4_LOGIC_HIGH		(0x1 << 6)
#define RT5616e_SEL_GPIO3_MASK			(0x1 << 5)
#define RT5616e_SEL_GPIO3_INPUT			(0x0 << 5)
#define RT5616e_SEL_GPIO3_OUTPUT		(0x1 << 5)
#define RT5616e_GPIO3_LOGIC_MASK		(0x1 << 4)
#define RT5616e_GPIO3_LOGIC_LOW			(0x0 << 4)
#define RT5616e_GPIO3_LOGIC_HIGH		(0x1 << 4)
#define RT5616e_SEL_GPIO2_MASK			(0x1 << 3)
#define RT5616e_SEL_GPIO2_INPUT			(0x0 << 3)
#define RT5616e_SEL_GPIO2_OUTPUT		(0x1 << 3)
#define RT5616e_GPIO2_LOGIC_MASK		(0x1 << 2)
#define RT5616e_GPIO2_LOGIC_LOW			(0x0 << 2)
#define RT5616e_GPIO2_LOGIC_HIGH		(0x1 << 2)
#define RT5616e_SEL_GPIO1_MASK			(0x1 << 1)
#define RT5616e_SEL_GPIO1_INPUT			(0x0 << 1)
#define RT5616e_SEL_GPIO1_OUTPUT		(0x1 << 1)
#define RT5616e_GPIO1_LOGIC_MASK		0x1
#define RT5616e_GPIO1_LOGIC_LOW			0x0
#define RT5616e_GPIO1_LOGIC_HIGH		0x1

/* GPIO Driving Control 1   (0x0304) */
#define RT5616e_SEL_GPIO2_TYPE_MASK		(0x1 << 9)
#define RT5616e_GPIO2_TYPE_GPIO1		(0x0 << 9)
#define RT5616e_GPIO2_TYPE_IRQ_OUT		(0x1 << 9)
#define RT5616e_SEL_GPIO1_TYPE_MASK		(0x1 << 8)
#define RT5616e_GPIO1_TYPE_GPIO1		(0x0 << 8)
#define RT5616e_GPIO1_TYPE_IRQ_OUT		(0x1 << 8)

/* ADC Filter Adjustable High pass filter windf control 1   (0x0e00) */
#define RT5616e_AD_BST_GL_MASK			(0x3 << 6)
#define RT5616e_AD_BST_GL_SFT			6
#define RT5616e_AD_BST_GR_MASK			(0x3 << 4)
#define RT5616e_AD_BST_GR_SFT			4
#define RT5616e_AD_COMP_MASK			(0x3 << 2)
#define RT5616e_AD_COMP_SFT			2

/* ADC Filter Adjustable High pass filter windf control 3   (0x0e02) */
#define RT5616e_MU_ADC_VOL_L			(0x1 << 7)
#define RT5616e_MU_ADC_VOL_L_SFT		7
#define RT5616e_MU_ADC_VOL_R			(0x1 << 4)
#define RT5616e_MU_ADC_VOL_R_SFT		4

/* ADC Filter Adjustable High pass filter windf control 4    (0x0e03) */
#define RT5616e_VOL_ADCL_MASK			0x7f
#define RT5616e_VOL_ADCL_SFT			0

/* ADC Filter Adjustable High pass filter windf control 5    (0x0e04) */
#define RT5616e_VOL_ADCR_MASK			0x7f
#define RT5616e_VOL_ADCR_SFT			0

/* GPIO Control 1   (0x2200) */
#define RT5616e_I2S_BCLK_MS_MASK		(0x1 << 15)
#define RT5616e_I2S_BCLK_MS_SFT			15
#define RT5616e_I2S_BCLK_MS_32FS		(0x0 << 15)
#define RT5616e_I2S_BCLK_MS_64FS		(0x1 << 15)
#define RT5616e_I2S_BCLK_POL			(0x1 << 13)
#define RT5616e_I2S_BCLK_NORM			(0x0 << 13)
#define RT5616e_I2S_BCLK_INV			(0x1 << 13)
#define RT5616e_I2S_TX_CH_MASK			(0x2 << 10)
#define RT5616e_I2S_TX_CH_SFT			10
#define RT5616e_I2S_TX_CH_LR			(0x0 << 10)
#define RT5616e_I2S_TX_CH_RL			(0x1 << 10)
#define RT5616e_I2S_TX_CH_LL			(0x2 << 10)
#define RT5616e_I2S_TX_CH_RR			(0x3 << 10)
#define RT5616e_I2S_RX_CH_MASK			(0x2 << 8)
#define RT5616e_I2S_RX_CH_SFT			8
#define RT5616e_I2S_RX_CH_LR			(0x0 << 8)
#define RT5616e_I2S_RX_CH_RL			(0x1 << 8)
#define RT5616e_I2S_RX_CH_LL			(0x2 << 8)
#define RT5616e_I2S_RX_CH_RR			(0x3 << 8)
#define RT5616e_I2S_DATA_LEN_MASK		(0x7 << 4)
#define RT5616e_I2S_DATA_LEN_8			(0x0 << 4)
#define RT5616e_I2S_DATA_LEN_16			(0x1 << 4)
#define RT5616e_I2S_DATA_LEN_20			(0x2 << 4)
#define RT5616e_I2S_DATA_LEN_24			(0x3 << 4)
#define RT5616e_I2S_DATA_LEN_32			(0x4 << 4)
#define RT5616e_I2S_MS_MASK			(0x1 << 3)
#define RT5616e_I2S_MS_SFT			3
#define RT5616e_I2S_MS_M			(0x0 << 3)
#define RT5616e_I2S_MS_S			(0x1 << 3)
#define RT5616e_I2S_FMT_MASK			0x7
#define RT5616e_I2S_FMT_I2S			0x0
#define RT5616e_I2S_FMT_LJ			0x1
#define RT5616e_I2S_FMT_MODEA			0x2
#define RT5616e_I2S_FMT_MODEB			0x3
#define RT5616e_I2S_FMT_AN			0x6
#define RT5616e_I2S_FMT_BN			0x7

/* Headphone Amp Detection Control 1  (0x3100) */
#define RT5616e_SEL_PM_HP_MASK			(0x3 << 2)
#define RT5616e_SEL_PM_HP_LOW			(0x0 << 2)
#define RT5616e_SEL_PM_HP_MED			(0x1 << 2)
#define RT5616e_SEL_PM_HP_HIGH			(0x2 << 2)
#define RT5616e_SEL_PM_HP_HIGH2			(0x3 << 2)

/* Volume Rescale */
#define RT5616e_VOL_RSCL_MAX 0x27
#define RT5616e_VOL_RSCL_RANGE 0x1F
/* Debug String Length */
#define RT5616e_REG_DISP_LEN 10

/* frequency times of BCK */
#define RT5616e_FREQ_IN_BCK_MULTIPLIER 32
#define RT5616e_FREQ_OUT_BCK_MULTIPLIER_256 256
#define RT5616e_FREQ_OUT_BCK_MULTIPLIER 512

/* System Clock Source */
enum {
	RT5616e_SCLK_S_MCLK,
	RT5616e_SCLK_S_PLL1,
	RT5616e_SCLK_S_RCCLK,
};

/* PLL1 Source */
enum {
	RT5616e_PLL1_S_MCLK,
	RT5616e_PLL1_S_BCLK,
	RT5616e_PLL1_S_RC,
};

enum {
	RT5616e_AIF1,
	RT5616e_AIFS,
};

enum {
	RT5616e_VERA = 0,
	RT5616e_VERB,
};


#endif /* __RT5616E_H__ */
