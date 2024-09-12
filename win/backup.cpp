// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/backup.hpp"
#include "savconf.h"
#include "win/termas.hpp"
#include "backup.h"
#include "strbuf.h"
#include "debug.h"
#include "fileiter.h"

backup::backup(size_t nl)
{
	timer = new HANDLE[nl];
	queue = CreateTimerQueue();

	use_defque = !queue;
	if (use_defque)
		warn_winerr(
_("failed to create timer queue (fallback to default timer queue)"));
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

	err = backup_routine(c);
	if (err)
		error(
_("an error occurred while making backup for configuration `%s'"), c->name);
}

void backup::create_backup_task(const struct savesave *conf)
{
	int pass;

	HANDLE *p = timer;
	DWORD countdown;
	ULONG flag;
	while (conf->name != NULL) {
		countdown = conf->period * 1000;
		if (conf->save_size > CONFIG_COMPRESSING_THRESHOLD)
			flag = WT_EXECUTELONGFUNCTION;
		else
			flag = WT_EXECUTEDEFAULT;

		pass = CreateTimerQueueTimer(p++, queue, do_backup,
					     (void *)conf++, countdown,
					     countdown, flag);
		if (!pass)
			die_winerr(
_("failed to create a timer for configuration `%s'"), conf->name);
	}
}

extern "C" {
int backup_copy_regfile(struct fileiter_file *src, struct strbuf *dest);
int backup_copy_symlink(const char *src,
			struct strbuf *dest, struct strbuf *__buf);
}

int backup_copy_regfile(struct fileiter_file *src, struct strbuf *dest)
{
	int err;

	ERROR_PATH_NOT_FOUND;
	err = !CopyFile(src->absname, dest->str, false);
	if (err) {
		if (GetLastError() != ERROR_PATH_NOT_FOUND)
			goto err_copy_file;

		err = strbuf_mkfdirp(dest);
		if (err)
			goto err_make_dir;

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

int backup_copy_symlink(const char *src,
			struct strbuf *dest, struct strbuf *__buf)
{
	int err;

	err = !CopyFileEx(src, dest->str, NULL, NULL,
			  NULL, COPY_FILE_COPY_SYMLINK);
	if (err) {
		if (GetLastError() != ERROR_PATH_NOT_FOUND)
			goto err_copy_file;

		err = strbuf_mkfdirp(dest);
		if (err)
			goto err_make_dir;

		err = !CopyFileEx(src, dest->str, NULL, NULL,
				  NULL, COPY_FILE_COPY_SYMLINK);
		if (err)
			goto err_copy_file;
	}

	return 0;

err_copy_file:
	BUG_ON(GetLastError() == ERROR_PATH_NOT_FOUND);
	return warn_errno(_("unable to copy file from `%s' to `%s'"),
			  src, dest->str);
err_make_dir:
	return warn_errno(_("unable to make directory `%s'"), dest->str);
}
