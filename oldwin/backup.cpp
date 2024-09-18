// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/backup.hpp"
#include "dotsav.h"
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
