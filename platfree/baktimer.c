// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktimer.h"
#include "alloc.h"
#include "noleak.h"

void baktimer_init(baktimer_t *ctx,
		   struct baksched *bs, struct savesave *sav, size_t nl)
{
	struct baktimer *bt = xcalloc(sizeof(*bt), 1);

	bt->tmr = xcalloc(sizeof(*bt->tmr), nl);
	bt->sav = sav;
	bt->nl = nl;

	PLATSPECOF(baktimer_init)(bt, bs);

	*ctx = bt;
	NOLEAK(bt);
}
