// SPDX-License-Identifier: GPL-2.0
/*
 * IIO compatibility layer for TSL2540 driver.
 * Copyright (c) 2025-2026 Ben Grisdale <bengris32@protonmail.ch>
 */

#ifndef __TSL2540_IIO_H
#define __TSL2540_IIO_H

int tsl2540_initialize_iio(struct tsl2540_chip *chip, const char *name);

#endif /* __TSL2540_IIO_H */
