// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktimer.h"
#include "baksched.h"
#include "list.h"
#include "dotsav.h"
#include "alloc.h"
#include "termas.h"
#include "debug.h"

static void tmrfunc(sigval_t sv)
{
	struct savesave *sav = ((void **)sv.sival_ptr)[0];
	baksched_t bs = ((void **)sv.sival_ptr)[1];

	baksched_post(bs, sav);
}

void PLATSPECOF(baktimer_init)(struct baktimer *ctx, struct baksched *bs)
{
	struct sigevent sev = {
		.sigev_notify           = SIGEV_THREAD,
		.sigev_notify_function  = tmrfunc,
	};

	size_t i;
	for_each_idx(i, ctx->nl) {
		void **data = xcalloc(sizeof(void *), 2);

		data[0] = &ctx->sav[i];
		data[1] = bs;
		sev.sigev_value.sival_ptr = data;

		int err = timer_create(CLOCK_REALTIME, &sev, &ctx->tmr[i]);

		if (err)
			die_errno(_("failed to create backup timer for sav `%s'"),
				  ctx->sav[i].name);
	}
}

void baktimer_arm(struct baktimer *ctx)
{
	struct itimerspec ts = { 0 };

	size_t i;
	for_each_idx(i, ctx->nl) {
		ts.it_value.tv_sec = ctx->sav[i].period;
		ts.it_interval.tv_sec = ctx->sav[i].period;

		int err = timer_settime(ctx->tmr[i], 0, &ts, NULL);
		BUG_ON(err);
	}
}

// void baktimer_disarm(struct baktimer *ctx)
// {
// 	struct itimerspec ts = { 0 };

// 	size_t i;
// 	for_each_idx(i, ctx->nl) {
// 		int err = timer_settime(ctx->tmr[i], 0, &ts, NULL);
// 		BUG_ON(err);
// 	}
// }

// void baktimer_destroy(struct baktimer *ctx)
// {
// 	size_t i;
// 	for_each_idx(i, ctx->nl) {
// 		int err = timer_delete(ctx->tmr[i]);
// 		BUG_ON(err);
// 	}

// 	free(ctx->tmr);
// 	free(ctx);
// }
