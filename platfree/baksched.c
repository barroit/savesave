// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baksched.h"
#include "alloc.h"
#include "termas.h"
#include "dotsav.h"
#include "list.h"
#include "backup.h"

#define MAX_OVERLOAD_COUNT 10
#define OVERLOAD_WINDOW    5

struct baktask {
	struct savesave *sav;
	struct list_head list;
};

static int routine(void *data)
{
	struct baksched *ctx = data;

	while (39) {
		mtx_lock(&ctx->qmtx);
retry:
		cnd_wait(&ctx->qcnd, &ctx->qmtx);
		if (list_is_empty(&ctx->queue))
			goto retry;

		struct baktask *task = list_last_entry(&ctx->queue,
						       typeof(*task), list);
		list_del(&task->list);

		struct savesave *sav = task->sav;
		ctx->task[sav->task_idx] ^= sav->task_pos;

		mtx_unlock(&ctx->qmtx);

		free(task);

		// handle compression here
		backup(sav);
	}

	BUG_ON(1);
}

static int watchdog(void *data)
{
	struct baksched *ctx = data;
	struct timespec interval = {
		.tv_sec = 1,
	};

	struct timespec lts = { 0 };
	struct timespec cts = { 0 };
	uint last = 0;

	while (39) {
		uint new = __atomic_load_n(&ctx->overload, __ATOMIC_RELAXED);
		if (!new)
			goto next;

		if (last != new) {
			last = new;
			timespec_get(&lts, TIME_UTC);
			goto next;
		}

		timespec_get(&cts, TIME_UTC);
		if (cts.tv_sec - lts.tv_sec > OVERLOAD_WINDOW) {
			__atomic_store_n(&ctx->overload, 0, __ATOMIC_RELAXED);
			last = 0;
		}

next:
		thrd_sleep(&interval, NULL);
	}

	BUG_ON(1);
}

void baksched_init(struct baksched **ctx, size_t nl)
{
	struct baksched *bs = xcalloc(sizeof(*bs), 1); 

	bs->task = xcalloc(sizeof(*bs->task),
			   nl / (sizeof(*bs->task) * CHAR_BIT) ? : 1);

	list_head_init(&bs->queue);

	xmtx_init(&bs->qmtx, mtx_plain);
	xcnd_init(&bs->qcnd);

	xthrd_create(&bs->qthrd[0], routine, bs);
	xthrd_create(&bs->qthrd[1], routine, bs);

	xthrd_create(&bs->qthrd[2], watchdog, bs);

	*ctx = bs;
}

void baksched_post(struct baksched *ctx, struct savesave *sav)
{
	struct baktask *task = xmalloc(sizeof(*task));
	task->sav = sav;

	mtx_lock(&ctx->qmtx);

	if (ctx->task[sav->task_idx] & sav->task_pos) {
		mtx_unlock(&ctx->qmtx);
		goto err_heavy_traffic;
	}

	ctx->task[sav->task_idx] |= sav->task_pos;
	list_add_tail(&task->list, &ctx->queue);

	cnd_signal(&ctx->qcnd);
	mtx_unlock(&ctx->qmtx);
	return;

err_heavy_traffic:
	free(task);
	warn(_("high traffic detected in sav `%s'"), sav->name);

	uint cnt = __atomic_add_fetch(&ctx->overload, 1, __ATOMIC_RELAXED);
	if (cnt <= MAX_OVERLOAD_COUNT)
		return;

	die(_("too many overloads occurred, savesave can't handle them anymore"));
}
