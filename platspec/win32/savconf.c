// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "dotsav.h"
#include "fileiter.h"
#include "iter.h"

void dotsav_format(struct savesave *dotsav, size_t n)
{
	size_t i;
	for_each_idx(i, n) {
		struct savesave *c = &dotsav[i];

		strrepl(c->save_prefix, '\\', '/');
		strrepl(c->backup_prefix, '\\', '/');
	}
}
