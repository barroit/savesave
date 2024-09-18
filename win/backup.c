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
#include "debug.h"

static int backup_copy_routine(struct fileiter_file *src,
			       struct strbuf *dest, int is_symlink)
{
	int err;
	int errnum = GetLastError();

	if (is_symlink)
		err = !CopyFileEx(src->absname, dest->str, NULL, NULL,
				  NULL, COPY_FILE_COPY_SYMLINK);
	else
		err = !CopyFile(src->absname, dest->str, 0);

	if (err) {
		if (GetLastError() != ERROR_PATH_NOT_FOUND)
			goto err_copy_file;
		SetLastError(errnum);

		err = strbuf_mkfdirp(dest);
		if (err)
			goto err_make_dir;

		if (is_symlink)
			err = !CopyFileEx(src->absname, dest->str, NULL, NULL,
					  NULL, COPY_FILE_COPY_SYMLINK);
		else
			err = !CopyFile(src->absname, dest->str, 0);

		if (err)
			goto err_copy_file;
	}

	return 0;

err_copy_file:
	BUG_ON(GetLastError() == ERROR_PATH_NOT_FOUND);
	return warn_errno(_("unable to copy file from `%s' to `%s'"),
			  src->absname, dest->str);
err_make_dir:
	return warn_errno(_("unable to make directory `%s'"), dest->str);
	
}

int PLATSPECOF(backup_copy_regfile)(struct fileiter_file *src,
				    struct strbuf *dest)
{
	return backup_copy_routine(src, dest, 0);
}

int PLATSPECOF(backup_copy_symlink)(struct fileiter_file *src,
				    struct strbuf *dest, struct strbuf *_)
{
	return backup_copy_routine(src, dest, 1);
}
