// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termas.h"

rlim_t getfdavail(void)
{
	static rlim_t soft;

	if (unlikely(!soft)) {
		struct rlimit lim;
		int err = getrlimit(RLIMIT_NOFILE, &lim);

		if (err) {
			warn(_("failed to resource limit of file descriptor"));
			lim.rlim_cur = 512;
		}

		soft = lim.rlim_cur;
	}

	return soft - cntio_fdcnt;
}

uint getcpucores(void)
{
	static uint cores;

	if (unlikely(!cores))
		cores = sysconf(_SC_NPROCESSORS_ONLN);

	return cores;
}
