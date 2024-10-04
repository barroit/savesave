// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "termas.h"
#include "fileiter.h"

int flexremove(const char *name)
{
	int err;

	err = unlink(name);
	if (!err)
		return 0;
	else if (errno == EISDIR)
		return rmdirr(name);

	return warn_errno(_("failed to remove file `%s'"), name);
}

int PLATSPECOF(sizeof_file)(struct iterfile *file, void *data)
{
	*((off_t *)data) += file->st->st_size;
	return 0;
}
