// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktmr.h"
#include "iter.h"
#include "dotsav.h"
#include "alloc.h"
#include "termas.h"
#include "barrier.h"

struct baktmr {
	timer_t *id;	/* timer ids */
	char *rtm;	/* running task map, using char
			   due to __atomic builtins */
};

static struct baktmr tmr;

static void tmr_callback(sigval_t sv)
{
	uint i = sv.sival_int;
	FEATSPEC(baktmr_callback)(i, &tmr.rtm[i]);
}

void baktmr_arm(void)
{
	FEATSPEC(bakolc_init)();

	tmr.id = xcalloc(sizeof(*tmr.id), dotsav_size);
	tmr.rtm = xcalloc(sizeof(*tmr.rtm), dotsav_size);

	int err;
	size_t i;
	struct itimerspec ts = { 0 };
	struct sigevent sev = {
		.sigev_notify           = SIGEV_THREAD,
		.sigev_notify_function  = tmr_callback,
	};

	for_each_idx(i, dotsav_size) {
		sev.sigev_value.sival_int = i;
retry:
		err = timer_create(CLOCK_REALTIME, &sev, &tmr.id[i]);
		if (!err)
			continue;
		else if (errno == EAGAIN)
			goto retry;

		die_errno(_("failed to create backup timer for sav `%s'"),
			  dotsav_array[i].name);
	}

	for_each_idx(i, dotsav_size) {
		ts.it_value.tv_sec = dotsav_array[i].period;
		ts.it_interval.tv_sec = dotsav_array[i].period;

		timer_settime(tmr.id[i], 0, &ts, NULL);
	}
}

void baktmr_disarm(void)
{
	size_t i;
	struct itimerspec ts = { 0 };

	for_each_idx(i, dotsav_size)
		timer_settime(tmr.id[i], 0, &ts, NULL);
}
