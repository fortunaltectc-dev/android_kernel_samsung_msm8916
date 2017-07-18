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

static struct touch_disabler_data *g_data;

static void _touch_disabler_set_touch_status(bool status);

static ssize_t touch_disabler_get_enabled(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	if (g_data->enabled) {
		return sprintf(buf, "%s\n", "true");
	}
	return sprintf(buf, "%s\n", "false");
}

static ssize_t touch_disabler_set_enabled(struct kobject *kobj,
		struct kobj_attribute *attr, const char *buf,
		size_t count)
{
	/* only set the variable if mode is set to manual */
	if (g_data->mode) {
		if (!strncmp(buf, "true", 4) || !strncmp(buf, "1", 1)) {
			pr_info("%s: touch devices are enabled.\n", __func__);
			_touch_disabler_set_touch_status(true);
			return count;
		}
		else if (!strncmp(buf, "false", 5) || !strncmp(buf, "0", 1)) {
			pr_info("%s: touch devices are disabled.\n", __func__);
			_touch_disabler_set_touch_status(false);
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
	if (g_data->mode) {
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
		g_data->mode = 1;
		return count;
	}
	else if (!strncmp(buf, MODE_AUTO, strlen(MODE_AUTO)) ||
			!strncmp(buf, "0", 1)) {
		pr_info("%s: auto mode is enabled.\n", __func__);
		g_data->mode = 0;
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
void touch_disabler_set_touch_status(bool status)
{
	/* let mdss trigger the enaling/disabling */
	if (g_data && !g_data->mode) {
		_touch_disabler_set_touch_status(status);
	}
}

void touch_disabler_set_ts_dev(struct input_dev *ts_dev)
{
	if (g_data) {
		g_data->ts_dev = ts_dev;
	}
}

void touch_disabler_set_tk_dev(struct input_dev *tk_dev)
{
	if (g_data) {
		g_data->tk_dev = tk_dev;
	}
}

static void _touch_disabler_set_touch_status(bool status)
{
	/* set the enabled variable */
	if (g_data) {
		g_data->enabled = status;
	}

	/* check if the struct has been initialised by the touch driver */
	if (g_data && g_data->ts_dev) {
		if (status) {
			pr_info("%s: Enabling %s touch panel...\n", __func__,
					g_data->ts_dev->name);
			g_data->ts_dev->open(g_data->ts_dev);
		} else {
			pr_info("%s: Disabling %s touch panel...\n", __func__,
					g_data->ts_dev->name);
			g_data->ts_dev->close(g_data->ts_dev);
		}
	} else {
		pr_warn("%s: Touch panel data struct is uninitialised!\n", __func__);
	}

	if (g_data && g_data->tk_dev) {
		if (status) {
			pr_info("%s: Enabling %s touch keys...\n", __func__,
					g_data->tk_dev->name);
			g_data->tk_dev->open(g_data->tk_dev);
		} else {
			pr_info("%s: Disabling %s touch keys...\n", __func__,
					g_data->tk_dev->name);
			g_data->tk_dev->close(g_data->tk_dev);
		}
	} else {
		pr_warn("%s: Touch key data struct is uninitialised!\n", __func__);
	}
}

static int touch_disabler_init_sysfs(void)
{
	struct touch_disabler_data *data;
	int ret = 0;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data) {
		pr_err("%s: Failed to alloc data\n", __func__);
		ret = -ENOMEM;
		goto err_alloc_data;
	}

	data->dev = NULL; // set to NULL for now
	data->enabled = 0;
	data->mode = 0;
	data->ts_dev = NULL;
	data->tk_dev = NULL;
	g_data = data;

	data->disabler_kobject = kobject_create_and_add("touch_disabler",
						 kernel_kobj);
	if(!data->disabler_kobject) {
		pr_err("%s: Failed to create kobject\n", __func__);
		ret = -ENOMEM;
		goto err_create_kobject;
	}

	ret = sysfs_create_file(data->disabler_kobject, &enabled_attribute.attr);

	if (ret) {
		pr_err("%s: Failed to create enabled\n", __func__);
		goto err_create_enabled;
	}

	ret = sysfs_create_file(data->disabler_kobject, &mode_attribute.attr);

	if (ret) {
		pr_err("%s: Failed to create mode\n", __func__);
		goto err_create_enabled;
	}

	pr_debug("%s: Initialised sysfs interface.\n", __func__);
	return 0;

err_create_enabled:
	kobject_put(data->disabler_kobject);
err_create_kobject:
	kfree(data);
	g_data = NULL;
err_alloc_data:
	pr_err("%s: Failed to initialise sysfs interface.\n", __func__);
	return ret;
}

static void touch_disabler_free_sysfs(void)
{
	struct touch_disabler_data *data = g_data;

	kobject_put(data->disabler_kobject);
	kfree(data);
	g_data = NULL;
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
