// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

timestamp_t timestamp(void)
{
	struct timespec ts;

	int err = clock_gettime(CLOCK_MONOTONIC, &ts);
	BUG_ON(err);

	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}
