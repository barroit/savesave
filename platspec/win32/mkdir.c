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
	DWORD attr = GetFileAttributes(name);
	BUG_ON(attr == INVALID_FILE_ATTRIBUTES);

	if (attr & FILE_ATTRIBUTE_DIRECTORY)
		return rmdirr(name);

	err = unlink(name);
	if (!err)
		return 0;

	return warn_errno(_("failed to remove file `%s'"), name);
}
