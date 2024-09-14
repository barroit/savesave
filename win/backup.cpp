// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/backup.hpp"
#include "savconf.h"
#include "termas.h"
#include "strbuf.h"
#include "debug.h"
#include "fileiter.h"

backup::backup(size_t nl)
{
	timer = new HANDLE[nl];
	queue = CreateTimerQueue();

	use_defque = !queue;
	if (use_defque)
		warn_winerr(_("failed to create timer queue, fallback to default one"));
}

backup::~backup()
{
	delete[] timer;

	if (!use_defque)
		DeleteTimerQueue(queue);
}

static void do_backup(void *conf, unsigned char)
{
	int err;
	struct savesave *c = (struct savesave *)conf;

	err = backup(c);
	if (!err)
		error(_("cannot make backup for configuration `%s'"), c->name);
}

void backup::create_backup_task(const struct savesave *conf)
{
	int err;
	HANDLE *p = timer;

	while (conf->name != NULL) {
		DWORD countdown = conf->period * 1000;
		ULONG flag = conf->save_size > CONFIG_COMPRESSING_THRESHOLD ?
				WT_EXECUTELONGFUNCTION : WT_EXECUTEDEFAULT;

		err = !CreateTimerQueueTimer(p++, queue, do_backup,
					     (void *)conf++, countdown,
					     countdown, flag);
		if (err)
			goto err_create_timer;
	}

err_create_timer:
	die_winerr(_("failed to create timer for configuration `%s'"),
		   conf->name);
}

static int backup_copy_routine(struct fileiter_file *src,
			       struct strbuf *dest, int is_symlink)
{
	int err;
	int errnum = GetLastError();

	if (is_symlink)
		err = !CopyFileEx(src->absname, dest->str, NULL, NULL,
				  NULL, COPY_FILE_COPY_SYMLINK);
	else
		err = !CopyFile(src->absname, dest->str, false);

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
			err = !CopyFile(src->absname, dest->str, false);

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
				    struct strbuf *dest, struct strbuf *)
{
	return backup_copy_routine(src, dest, 1);
}
