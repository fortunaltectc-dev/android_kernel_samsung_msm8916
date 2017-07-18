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

#ifndef TOUCH_DISABLER_H
#define TOUCH_DISABLER_H

#define MODE_AUTO "auto"
#define MODE_MANUAL "manual"

typedef struct {
	struct device *dev;
	struct input_dev *ts_dev;
	struct input_dev *tk_dev;
	struct class *disabler_class;
	int enabled; /* enable (1) /disable (0) touch devices */
	int mode;    /* driver mode, between auto (0) and manual (1) */
} touch_disabler_data_t;

void touch_disabler_set_tk_dev(struct input_dev *ts_dev);
void touch_disabler_set_touch_mode(bool status);
void touch_disabler_set_ts_dev(struct input_dev *ts_dev);

#endif /* TOUCH_DISABLER_H */
