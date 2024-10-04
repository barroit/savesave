// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktimer.h"
#include "baksched.h"
#include "dotsav.h"
#include "alloc.h"
#include "list.h"
#include "termas.h"
#include "debug.h"

void PLATSPECOF(baktimer_init)(struct baktimer *ctx, struct baksched *bs)
{
	ctx->tmrque = CreateTimerQueue();
	ctx->bs = bs;
}

static void tmrfunc(void *data, BOOLEAN fuck_this_BOOLEAN_type_fuck_microsoft)
{
	struct savesave *sav = ((void **)data)[0];
	baksched_t bs = ((void **)data)[1];

	baksched_post(bs, sav);
}

void baktimer_arm(struct baktimer *ctx)
{
	size_t i;
	for_each_idx(i, ctx->nl) {
		struct savesave *sav = &ctx->sav[i];
		void **data = xcalloc(sizeof(void *), 2);
		DWORD cd = sav->period * 1000;

		data[0] = &ctx->sav[i];
		data[1] = ctx->bs;

		int err = !CreateTimerQueueTimer(&ctx->tmr[i], ctx->tmrque,
						 tmrfunc, data, cd, cd,
						 WT_EXECUTELONGFUNCTION);

		if (err)
			die_winerr(_("failed to create timer for sav `%s'"),
				   sav->name);
	}
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
