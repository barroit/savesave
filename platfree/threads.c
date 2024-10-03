// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termas.h"

int xmtx_init(mtx_t *mutex, int type)
{
	int ret = mtx_init(mutex, type);

	if (likely(ret == thrd_success))
		return ret;

	die(_("failed to create mutex"));
}

int xcnd_init(cnd_t *cond)
{
	int ret = cnd_init(cond);

	if (likely(ret == thrd_success))
		return ret;

	const char *mas = _("failed to create condition variable");

	if (ret == thrd_nomem) {
		errno = ENOMEM;
		die_errno(mas);
	} else {
		die(mas);
	}
}

int xthrd_create(thrd_t *thr, thrd_start_t func, void *arg)
{
	int ret = thrd_create(thr, func, arg);

	if (likely(ret == thrd_success))
		return ret;

	const char *mas = _("failed to create thread");

	if (ret == thrd_nomem) {
		errno = ENOMEM;
		die_errno(mas);
	} else {
		die(mas);
	}
}
