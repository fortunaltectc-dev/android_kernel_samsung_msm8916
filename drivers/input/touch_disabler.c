/*
 * Copyright (c) 2017 The Lineage Project,
 *                    Vincent Zvikaramba <zvikovincent@gmail.com>,
 *                    Vladimir Bely <vlwwwwww@gmail.com>,
 *                    Emery Tang <emerytang@gmail.com>,
 *                    Sean Hoyt <deadman96385@gmail.com>
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

#include <linux/kobject.h>
#include <linux/sysfs.h>

#include <linux/input.h>

#include <linux/input/touch_disabler.h>

static touch_disabler_data_t touch_disabler_data;

static void _touch_disabler_set_touch_mode(bool status);

static ssize_t touch_disabler_get_enabled(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", touch_disabler_data.enabled);
}

static ssize_t touch_disabler_set_enabled(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf,
		size_t count)
{
	/* only set the variable if mode is set to manual */
	if (touch_disabler_data.mode) {
		if (!strncmp(buf, "0", 1) || !strncmp(buf, "1", 1)) {
			sscanf(buf, "%du", &touch_disabler_data.enabled);
			_touch_disabler_set_touch_mode(touch_disabler_data.enabled);
			return count;
		} else {
			pr_err("%s: Invalid input passed\n", __func__);
			return -EINVAL;
		}
	}
	pr_warn("%s: Input ignored since auto mode is enabled.\n",
			__func__);
	return -EINVAL;
}

static struct kobj_attribute enabled_attribute =__ATTR(enabled, 0660, touch_disabler_get_enabled,
						   touch_disabler_set_enabled);

static ssize_t touch_disabler_get_mode(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	if (touch_disabler_data.mode) {
		return sprintf(buf, "%s\n", MODE_MANUAL);
	}
	return sprintf(buf, "%s\n", MODE_AUTO);
}

static ssize_t touch_disabler_set_mode(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf,
		size_t count)
{
	if (!strncmp(buf, MODE_MANUAL, strlen(MODE_MANUAL)) ||
			!strncmp(buf, "1", 1)) {
		pr_info("%s: manual mode is enabled.\n", __func__);
		touch_disabler_data.mode = 1;
		return count;
	}
	else if (!strncmp(buf, MODE_AUTO, strlen(MODE_AUTO)) ||
			!strncmp(buf, "0", 1)) {
		pr_info("%s: auto mode is enabled.\n", __func__);
		touch_disabler_data.mode = 0;
		return count;
	}
	pr_err("%s: Invalid input passed\n", __func__);
	return -EINVAL;
}

static struct kobj_attribute mode_attribute =__ATTR(mode, 0660, touch_disabler_get_mode,
						   touch_disabler_set_mode);

/*
 * Touch key/panel enabler/disabler for samsung touch keys/panel drivers.
 *
 * This function is called by mdss_fb for primary panel.
 * When panel blank or unblank, touch devices will enabled or disabled.
 *
 */
void touch_disabler_set_touch_mode(bool status)
{
	/* let mdss trigger the enaling/disabling */
	if (!touch_disabler_data.mode)
		_touch_disabler_set_touch_mode(status);
}

void touch_disabler_set_ts_dev(struct input_dev *ts_dev)
{
	touch_disabler_data.ts_dev = ts_dev;
}

void touch_disabler_set_tk_dev(struct input_dev *tk_dev)
{
	touch_disabler_data.tk_dev = tk_dev;
}

static void _touch_disabler_set_touch_mode(bool status)
{
	/* set the enabled variable */
	touch_disabler_data.enabled = status;

	/* check if the struct has been initialised by the touch driver */
	if (touch_disabler_data.ts_dev) {
		if (status) {
			pr_info("%s: Enabling %s touch panel...\n", __func__,
					touch_disabler_data.ts_dev->name);
			touch_disabler_data.ts_dev->open(touch_disabler_data.ts_dev);
		} else {
			pr_info("%s: Disabling %s touch panel...\n", __func__,
					touch_disabler_data.ts_dev->name);
			touch_disabler_data.ts_dev->close(touch_disabler_data.ts_dev);
		}
	} else {
		pr_warn("%s: Touch panel data struct is uninitialised!\n", __func__);
	}

	if (touch_disabler_data.tk_dev) {
		if (status) {
			pr_info("%s: Enabling %s touch keys...\n", __func__,
					touch_disabler_data.tk_dev->name);
			touch_disabler_data.tk_dev->open(touch_disabler_data.tk_dev);
		} else {
			pr_info("%s: Disabling %s touch keys...\n", __func__,
					touch_disabler_data.tk_dev->name);
			touch_disabler_data.tk_dev->close(touch_disabler_data.tk_dev);
		}
	} else {
		pr_warn("%s: Touch key data struct is uninitialised!\n", __func__);
	}
}

static int touch_disabler_init_sysfs(void)
{
	int ret;

	touch_disabler_data.enabled = 0;
	touch_disabler_data.mode = 0;

	touch_disabler_data.ts_dev = NULL;
	touch_disabler_data.tk_dev = NULL;

	touch_disabler_data.disabler_kobject = kobject_create_and_add("touch_disabler",
						 kernel_kobj);
	if(!touch_disabler_data.disabler_kobject) {
		pr_err("%s: Failed to create kobject\n", __func__);
		ret = -ENOMEM;
		goto err_create_kobject;
	}

	ret = sysfs_create_file(touch_disabler_data.disabler_kobject, &enabled_attribute.attr);

	if (ret) {
		pr_err("%s: Failed to create enabled\n", __func__);
		goto err_create_enabled;
	}

	ret = sysfs_create_file(touch_disabler_data.disabler_kobject, &mode_attribute.attr);

	if (ret) {
		pr_err("%s: Failed to create mode\n", __func__);
		goto err_create_enabled;
	}

	pr_debug("%s: Initialised sysfs interface.\n", __func__);
	return 0;

err_create_enabled:
        kobject_put(touch_disabler_data.disabler_kobject);
err_create_kobject:
	pr_err("%s: Failed to initialise sysfs interface.\n", __func__);
	return ret;
}

static void touch_disabler_free_sysfs(void)
{
        kobject_put(touch_disabler_data.disabler_kobject);
}

static int touch_disabler_remove(void)
{
	touch_disabler_free_sysfs();
	return 0;
}

static int __init touch_disabler_init(void)
{
	return touch_disabler_init_sysfs();
}

static void __exit touch_disabler_exit (void)
{
	touch_disabler_remove();
}

module_init(touch_disabler_init);
module_exit(touch_disabler_exit);

MODULE_DESCRIPTION("Touch key/panel disabler for Samsung devices");
MODULE_AUTHOR("Vladimir Bely <vlwwwwww@gmail.com>, Vincent Zvikaramba <zvikovincent@gmail.com>");
MODULE_LICENSE("GPL");
