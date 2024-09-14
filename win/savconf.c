// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "savconf.h"
#include "fileiter.h"
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

int PLATSPECOF(sizeof_file)(struct fileiter_file *file, void *data)
{
	off_t size;

	if (file->is_lnk)
		/*
		 * we don’t handle symbolic links; a constant is given
		 * that is large enough for most symbolic link files
		 */
		size = 64;
	else
		size = file->st->st_size;

	*((off_t *)data) += size;
	return 0;
}
