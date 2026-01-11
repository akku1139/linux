// SPDX-License-Identifier: GPL-2.0-only
/*
 * ASoC platform driver for Amazon Dough FPGA found on Echo devices.
 *
 * Copyright (c) 2016 Amazon Lab126
 */

#ifndef _AMAZON_DOUGH_FPGA_H

#define SPI_SETUP_BUF_SIZE 32
#define FPGA_VCC_DELAY_MS 10

#define DOUGH_AUDIO_SAMPLE_WIDTH 3

/* TODO: This differs per f/w and revision */
#define DOUGH_AUDIO_NUM_CHANNELS 9
#define DOUGH_AUDIO_FRAME_BUF    255

#define DOUGH_AUDIO_FRAME_BYTES  ((DOUGH_AUDIO_SAMPLE_WIDTH)*\
				 (DOUGH_AUDIO_NUM_CHANNELS))
#define DOUGH_FPGA_REV_MIN       30
#define DOUGH_FPGA_REV_MAX       251

#define FIRMWARE_MAX_BYTES     (10*4096)
#define FPGA_FIRMWARE_REV       30
#define FPGA_FIRMWARE_NAME      CONFIG_EXTRA_FIRMWARE

#define SPI_SPEED_HZ            50000000 /* Maximum speed 50 MHz */
#define SPI_READ_WAIT_MIN_USEC  6000
#define SPI_READ_WAIT_MAX_USEC  7000
#define FPGA_DELAY_MS           100
#define PINCTRL_DELAY_MS        100
#define MARGIN_USEC             500

#define SAMPLING_RATE           16000

#define SPI_HEADER              1
#define SPI_HEADER_DISABLE      0

#define SPI_N_CHANNELS          9

#define SPI_BYTES_PER_CHANNEL   3
#define SPI_BYTES_PER_FRAME     (SPI_N_CHANNELS * SPI_BYTES_PER_CHANNEL)

/* = 27 for 9 channels, = 18 for 6 channels */
#define SPI_BYTES_PER_PERIOD    (SPI_BYTES_PER_FRAME *\
				(DOUGH_AUDIO_FRAME_BUF+SPI_HEADER))

/* = 6912 for 9 channels, = 4608 for 6 channels */
#define SPI_N_PERIODS_MIN       1
#define SPI_N_PERIODS_MAX       10
#define SPI_PERIOD_BYTES_MIN    (SPI_BYTES_PER_PERIOD * SPI_N_PERIODS_MIN)
#define SPI_PERIOD_BYTES_MAX    (SPI_BYTES_PER_PERIOD * SPI_N_PERIODS_MAX)
#define SPI_BUFFER_BYTES_MAX    SPI_PERIOD_BYTES_MAX
#define SPI_DMA_BYTES_MAX       (SPI_PERIOD_BYTES_MAX * 2)

#define MAX_SCHEDULED_WORK_Q    3
#define MAX_FLUSHED_CYCLES      10

enum dough_fw_cmd {
	dough_fw_nop = 0x00,
	dough_fw_off = 0x80,
	dough_fw_i2s = 0x81,
	dough_fw_tpg = 0x83,
};

struct dough_audio_frame {
	uint8_t audio_data[DOUGH_AUDIO_FRAME_BYTES];
};

/* TODO: This differs per f/w and revision */
struct __attribute__((__packed__)) dough_status_frame {
	uint8_t rsvd0[15];          /* bytes 12-26 */
	uint32_t timestamp_48mhz;   /* bytes 8-11 */
	uint16_t num_audio_frames;  /* bytes 6-7 */
	uint8_t rsvd1[1];           /* bytes 5 */
	uint8_t mode;               /* byte 4 */
	uint8_t dac_inactive;       /* byte 3 */
	uint8_t i2s_inactive;       /* byte 2 */
	uint8_t overrun;            /* byte 1 */
	uint8_t fpga_rev;           /* byte 0 */
};

struct __attribute__((__packed__)) dough_frame {
	struct dough_status_frame dsf;
	struct dough_audio_frame daf[DOUGH_AUDIO_FRAME_BUF];
};

//BUILD_BUG_ON(sizeof(struct dough_frame) != (DOUGH_AUDIO_FRAME_BYTES * (1 + DOUGH_AUDIO_FRAME_BUF)));
#endif /* _AMAZON_DOUGH_FPGA_H */
