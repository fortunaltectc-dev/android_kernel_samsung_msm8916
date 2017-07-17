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

#ifndef TOUCH_DISABLER_H
#define TOUCH_DISABLER_H

typedef struct {
	struct device *dev;
	struct input_dev *ts_dev;
	struct input_dev *tk_dev;
	struct class *disabler_class;
} touch_disabler_data_t;

void touch_disabler_set_tk_dev(struct input_dev *ts_dev);
void touch_disabler_set_touch_mode(bool status);
void touch_disabler_set_ts_dev(struct input_dev *ts_dev);

#endif /* TOUCH_DISABLER_H */
