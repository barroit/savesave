/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BAKTIMER_H
#define BAKTIMER_H

struct savesave;

struct baktimer {
	size_t nl;
	struct savesave *sav;

#ifdef __unix__
	timer_t *tmr;
#else
	HANDLE *tmr;
	HANDLE tmrque;
#endif
};

typedef struct baktimer baktimer_t;

void baktimer_init(baktimer_t *bt, struct savesave *savarr, size_t savnl);

void baktimer_arm(baktimer_t *bt);

void baktimer_disarm(baktimer_t *bt);

void baktimer_destroy(baktimer_t *bt);

#endif /* BAKTIMER_H */
