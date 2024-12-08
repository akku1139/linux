/*
 * metrics.c
 *
 * Copyright 2017 Amazon Technologies, Inc. All Rights Reserved
 *
 * The code contained herein is licensed under the GNU General Public
 * License Version 2. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/of.h>




static int __init check_device_coming_out_of_powersave(char *str)
{
	get_option(&str, &powersave_flag);

	return 0;
}

__setup("powersave_metric=", check_device_coming_out_of_powersave);
