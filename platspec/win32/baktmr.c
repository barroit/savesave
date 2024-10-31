// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktmr.h"
#include "dotsav.h"
#include "alloc.h"
#include "iter.h"
#include "termas.h"

struct baktmr {
	HANDLE q;	/* timer queue */
	HANDLE *id;	/* timer ids */
	char *rtm;	/* running task map, using char
			   due to __atomic builtins */
};

static struct baktmr tmr;

static void tmr_callback(void *data,
			 BOOLEAN fuck_this_BOOLEAN_type_fuck_windows)
{
	uint i = (uintptr_t)data;
	puts("111");
	// FEATSPEC(baktmr_callback)(i, &tmr.rtm[i]);
}

void baktmr_arm(void)
{
	FEATSPEC(bakolc_init)();

	tmr.q = CreateTimerQueue();
	tmr.id = xcalloc(sizeof(*tmr.id), dotsav_size);
	tmr.rtm = xcalloc(sizeof(*tmr.rtm), dotsav_size);

	uint i;
	for_each_idx(i, dotsav_size) {
		struct savesave *sav = &dotsav_array[i];
		DWORD cd = sav->period * 1000;
		uintptr_t __i = i;

		int err = !CreateTimerQueueTimer(&tmr.id[i], tmr.q,
						 tmr_callback, (void *)__i, cd,
						 cd, WT_EXECUTELONGFUNCTION);
		if (err)
			die_winerr(_("failed to create timer for sav `%s'"),
				   sav->name);
	}
}

void baktmr_disarm(void)
{
	DeleteTimerQueueEx(tmr.q, NULL);

	if (tmr.q)
		DeleteTimerQueue(tmr.q);
}
