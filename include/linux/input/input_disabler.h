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

typedef struct {
	struct input_dev *tp_dev;
	struct input_dev *tk_dev;
} input_disabler_data_t;

input_disabler_data_t input_disabler_data;

void inputdisabler_set_touch(bool status);

#endif
