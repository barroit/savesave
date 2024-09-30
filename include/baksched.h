/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BAKSCHED_H
#define BAKSCHED_H

#include "list.h"

struct baksched {
	ullong *task;	/* bit set of task */
	uint overload;	/* high traffic count */

	thrd_t qthrd[3];
	cnd_t  qcnd;
	mtx_t  qmtx;
	struct list_head queue;
};

typedef struct baksched *baksched_t;

void baksched_init(baksched_t *ctx, size_t nl);

struct savesave;
void baksched_post(baksched_t ctx, struct savesave *sav);

#endif /* BAKSCHED_H */
