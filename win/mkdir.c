// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "mkdir.h"
#include "debug.h"
#include "termas.h"
#include "fileiter.h"

int flexremove(const char *name)
{
	int err;
	DWORD attr = GetFileAttributes(name);
	BUG_ON(attr == INVALID_FILE_ATTRIBUTES);

	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return rmdirr(name);

	err = unlink(name);
	if (!err)
		return 0;

	return warn_errno(_("failed to remove file `%s'"), name);
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
