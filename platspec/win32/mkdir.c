// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "mkdir.h"
#include "termas.h"

int flexremove(const char *name)
{
	/* _unlink does not set errno to EISDIR */
	DWORD attr = GetFileAttributes(name);
	BUG_ON(attr == INVALID_FILE_ATTRIBUTES);

	if (attr & FILE_ATTRIBUTE_DIRECTORY) /* FUCK */
		return rmdirr(name);

	int err = unlink(name);
	if (err)
		return warn_errno(ERRMAS_REMOVE_FILE(name));

	return 0;
}
