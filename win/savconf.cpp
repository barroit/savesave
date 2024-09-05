// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/savconf.hpp"
#include "savconf.h"
#include "list.h"

void format_savconf(struct savesave *savconf, size_t n)
{
	size_t i;
	for_each_idx(i, n) {
		struct savesave *c = &savconf[i];

		strrepl(c->save_prefix, '\\', '/');
		strrepl(c->backup_prefix, '\\', '/');
	}
}
