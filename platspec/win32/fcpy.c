// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "fcpy.h"
#include "termas.h"

int copy_regfile(const char *srcname, const char *destname)
{
	int err = !CopyFile(srcname,destname, 0);

	if (unlikely(err))
		return warn_errno(ERRMAS_COPY_FILE(srcname, destname));

	return 0;
}

int copy_symlink(const char *srcname, const char *destname)
{
	int err = !CopyFileEx(srcname, destname, NULL,
			      NULL, NULL, COPY_FILE_COPY_SYMLINK);

	if (unlikely(err))
		return warn_errno(ERRMAS_CREAT_LINK(srcname, destname));

	return 0;
}
