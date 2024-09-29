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

static void tmrfunc(sigval_t sv)
{
	struct savesave *sav = sv.sival_ptr;
	puts(sav->name);
}

void baktimer_init(struct baktimer *bt, struct savesave *savarr, size_t savnl)
{
	bt->tmr = xcalloc(sizeof(timer_t), savnl);
	bt->sav = savarr;
	bt->nl = savnl;

	struct sigevent sev = {
		.sigev_notify           = SIGEV_THREAD,
		.sigev_notify_function  = tmrfunc,
	};

	size_t i;
	for_each_idx(i, bt->nl) {
		sev.sigev_value.sival_ptr = &bt->sav[i];

		int err = timer_create(CLOCK_REALTIME, &sev, &bt->tmr[i]);
		if (err)
			die_errno(_("failed to create backup timer for sav `%s'"),
				  bt->sav[i].name);
	}
}

void baktimer_arm(struct baktimer *bt)
{
	struct itimerspec ts = { 0 };

	size_t i;
	for_each_idx(i, bt->nl) {
		ts.it_value.tv_sec = bt->sav[i].period;
		ts.it_interval.tv_sec = bt->sav[i].period;

		int err = timer_settime(bt->tmr[i], 0, &ts, NULL);
		BUG_ON(err);
	}
}

void baktimer_disarm(struct baktimer *bt)
{
	struct itimerspec ts = { 0 };

	size_t i;
	for_each_idx(i, bt->nl) {
		int err = timer_settime(bt->tmr[i], 0, &ts, NULL);
		BUG_ON(err);
	}
}

void baktimer_destroy(struct baktimer *bt)
{
	size_t i;
	for_each_idx(i, bt->nl) {
		int err = timer_delete(bt->tmr[i]);
		BUG_ON(err);
	}

	free(bt->tmr);
}
