/*
 * Copyright (c) 2017 The Lineage Project, Samsung MSM8916 Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/slab.h>

#include <linux/input/touch_disabler.h>

/*
 * Touch key/panel enabler/disabler for samsung touch keys/panel drivers.
 *
 * This function is called by mdss_fb for primary panel.
 * When panel blank or unblank, touch devices will enabled or disabled.
 *
 */
void touch_disabler_set_touch(bool status)
{
	/* check if the struct has been initialised by the touch driver */
	if (touch_disabler_data.ts_dev) {
		if (status) {
			pr_info("%s: Enabling touch panel...\n", __func__);
			touch_disabler_data.ts_dev->open(touch_disabler_data.ts_dev);
		} else {
			pr_info("%s: Disabling touch panel...\n", __func__);
			touch_disabler_data.ts_dev->close(touch_disabler_data.ts_dev);
		}
	} else {
		pr_error("%s: Touch panel data struct is uninitialised!\n", __func__);
	}

	if (touch_disabler_data.tk_dev) {
		if (status) {
			pr_info("%s: Enabling touch keys...\n", __func__);
			touch_disabler_data.tk_dev->open(touch_disabler_data.tk_dev);
		} else {
			pr_info("%s: Disabling touch keys...\n", __func__);
			touch_disabler_data.tk_dev->close(touch_disabler_data.tk_dev);
		}
	} else {
		pr_error("%s: Touch key data struct is uninitialised!\n", __func__);
	}
}

static int __init touch_disabler_init(void)
{
	touch_disabler_data.ts_dev = NULL;
	touch_disabler_data.tk_dev = NULL;
	return 0;
}

module_init(touch_disabler_init);

MODULE_DESCRIPTION("Touch key/panel disabler for Samsung devices");
MODULE_LICENSE("GPL");
