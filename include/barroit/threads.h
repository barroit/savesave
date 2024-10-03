/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_THREADS_H
#define BRT_THREADS_H

int xmtx_init(mtx_t *mutex, int type);

int xcnd_init(cnd_t *cond);

int xthrd_create(thrd_t *thr, thrd_start_t func, void *arg);

#endif /* BRT_THREADS_H */
