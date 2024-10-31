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
	HANDLE que;	/* timer queue */
	HANDLE *id;	/* timer ids */
	char *rtm;	/* running task map, using char
			   due to __atomic builtins */
};

static struct baktmr tmr;

static void tmr_callback(void *data,
			 BOOLEAN fuck_this_BOOLEAN_type_fuck_windows)
{
	uint i = (uintptr_t)data;
	FEATSPEC(baktmr_callback)(i, &tmr.rtm[i]);
}

void baktmr_arm(void)
{
	FEATSPEC(bakolc_init)();

	tmr.que = CreateTimerQueue();
	tmr.id = xcalloc(sizeof(*tmr.id), dotsav_size);
	tmr.rtm = xcalloc(sizeof(*tmr.rtm), dotsav_size);

	size_t i;
	for_each_idx(i, dotsav_size) {
		struct savesave *sav = &dotsav_array[i];
		DWORD cd = sav->period * 1000;

		int err = !CreateTimerQueueTimer(&tmr.id[i], tmr.que,
						 tmr_callback, (void *)i, cd,
						 cd, WT_EXECUTELONGFUNCTION);
		if (err)
			die_winerr(_("failed to create timer for sav `%s'"),
				   sav->name);
	}
}

void baktmr_disarm(void)
{
	FEATSPEC(bakolc_destroy)();
}

// void baktimer_disarm(struct baktimer *ctx)
// {
// 	/*
// 	 * no disarm on window
// 	 */
// 	BUG_ON(1);
// }

// void baktimer_destroy(struct baktimer *ctx)
// {
// 	size_t i;
// 	int err;

// 	for_each_idx(i, ctx->nl) {
// retry:
// 		err = !DeleteTimerQueueTimer(ctx->tmrque, ctx->tmr[i], NULL);
// 		if (!err)
// 			continue;
// 		else if (GetLastError() == ERROR_IO_PENDING)
// 			continue;
// 		else
// 			goto retry;
// 	}

// 	free(ctx->tmr);
// 	free(ctx);
// }
