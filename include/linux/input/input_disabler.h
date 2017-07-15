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

#ifndef INPUT_DISABLER_H
#define INPUT_DISABLER_H

struct input_disabler_tp {
	struct input_dev *dev;
	int (*input_open)(struct input_dev *);
	int (*input_close)(struct input_dev *);
}

struct input_disabler_tp input_disabler_tp_data;

void inputdisabler_set_touch(bool status);

#endif
