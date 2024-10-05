// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "list.h"

void thrd_affinity_all(size_t n)
{
	int err;
	size_t i;
	cpu_set_t cpuset;
	pthread_t thread = pthread_self();

	CPU_ZERO(&cpuset);
	for_each_idx(i, n)
		CPU_SET(i, &cpuset);

	err = pthread_setaffinity_np(thread, sizeof(cpuset), &cpuset);
	BUG_ON(err);

	err = pthread_getaffinity_np(thread, sizeof(cpuset), &cpuset);
	BUG_ON(err);
}
