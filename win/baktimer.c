// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktimer.h"
#include "dotsav.h"
#include "alloc.h"
#include "list.h"
#include "termas.h"
#include "debug.h"

void baktimer_init(struct baktimer *bt, struct savesave *sav, size_t nl)
{
	bt->nl = nl;
	bt->sav = sav;

	bt->tmr = xcalloc(sizeof(*bt->tmr), nl);
	bt->tmrque = CreateTimerQueue();
}

static void tmrfunc(void *data, BOOLEAN fuck_this_BOOLEAN_type_fuck_microsoft)
{
	struct savesave *sav = data;

	puts(sav->name);
}

void baktimer_arm(struct baktimer *bt)
{
	size_t i;
	for_each_idx(i, bt->nl) {
		struct savesave *sav = &bt->sav[i];
		DWORD cd = sav->period * 1000;
		ULONG flag = sav->save_size > CONFIG_COMPRESSING_THRESHOLD ?
			     WT_EXECUTELONGFUNCTION : WT_EXECUTEDEFAULT;

		int err = !CreateTimerQueueTimer(&bt->tmr[i], bt->tmrque,
						 tmrfunc, sav, cd, cd, flag);

		if (err)
			die_winerr(_("failed to create timer for sav `%s'"),
				   sav->name);
	}
}

void baktimer_disarm(struct baktimer *bt)
{
	/*
	 * no disarm on window
	 */
	BUG_ON(1);
}

void baktimer_destroy(struct baktimer *bt)
{
	size_t i;
	int err;

	for_each_idx(i, bt->nl) {
retry:
		err = !DeleteTimerQueueTimer(bt->tmrque, bt->tmr[i], NULL);
		if (!err)
			continue;
		else if (GetLastError() == ERROR_IO_PENDING)
			continue;
		else
			goto retry;
	}
	
}
