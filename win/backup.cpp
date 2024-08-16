// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/backup.hpp"
#include "alloc.h"
#include "savconf.h"
#include "win/termsg.hpp"
#include "list.h"

backup::backup(size_t nl)
{
	timer = new HANDLE[nl];
	queue = CreateTimerQueue();

	use_defque = !queue;
	if (use_defque)
		warn_winerr("fallback to default timer queue due to a failure when creating timer queue");
}

backup::~backup()
{
	delete[] timer;

	if (!use_defque)
		DeleteTimerQueue(queue);
}

static void CALLBACK do_backup(void *conf, unsigned char)
{
	const struct savesave *c = (struct savesave *)conf;
	if (c->use_snapshot); /* create a snapshot here */

	if (c->use_zip)
		;
}

void backup::create_backup_task(const struct savesave *conf)
{
	int pass;

	HANDLE *p = timer;
	DWORD countdown;
	ULONG flag;
	while (conf->name != NULL) {
		countdown = conf->period * 1000;
		if (conf->save_size > CONFIG_DO_ZIP_THRESHOLD)
			flag = WT_EXECUTELONGFUNCTION;
		else
			flag = WT_EXECUTEDEFAULT;

		pass = CreateTimerQueueTimer(p++, queue, do_backup,
					     (void *)conf++, countdown,
					     countdown, flag);
		if (!pass)
			die_winerr("failed to create a timer for configuration ‘%s’",
				   conf->name);
	}
}
