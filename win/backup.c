// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "backup.h"
#include "fileiter.h"
#include "termas.h"
#include "strbuf.h"

int PLATSPECOF(copy_regfile_block)(struct iterfile *src, struct strbuf *dest)
{
	int err = !CopyFile(src->absname, dest->str, 0);

	if (!err)
		return 0;

	return warn_errno(ERRMAS_COPY_FILE(src->absname, dest->str));
}

int PLATSPECOF(copy_symlink_block)(struct iterfile *src,
				   struct strbuf *dest, struct strbuf *_)
{
	int err = !CopyFileEx(src->absname, dest->str, NULL,
			      NULL, NULL, COPY_FILE_COPY_SYMLINK);

	if (!err)
		return 0;

	return warn_errno(ERRMAS_CREAT_LINK(dest->str, src->absname));
}
