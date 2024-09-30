/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BAKTIMER_H
#define BAKTIMER_H

struct savesave;
struct baksched;

struct baktimer {
	size_t nl;
	struct savesave *sav;

#ifdef __unix__
	timer_t *tmr;
#else
	HANDLE *tmr;
	HANDLE tmrque;
	struct baksched *bs;
#endif
};

typedef struct baktimer *baktimer_t;

void baktimer_init(baktimer_t *ctx,
		   struct baksched *bs, struct savesave *sav, size_t nl);

void PLATSPECOF(baktimer_init)(struct baktimer *ctx, struct baksched *bs);

void baktimer_arm(baktimer_t ctx);

// void baktimer_disarm(baktimer_t ctx);

// void baktimer_destroy(baktimer_t ctx);

#endif /* BAKTIMER_H */
