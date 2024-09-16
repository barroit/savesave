// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "debug.h"

timestamp_t gettimestamp(void)
{
	int err;
	static uint64_t hns, lns;
	static int scale;


	if (!scale) {
		LARGE_INTEGER freq;
		err = !QueryPerformanceFrequency(&freq)
		BUG_ON(err);

		hns = (1000000000ULL << 32) / (uint64_t)freq.QuadPart;
		lns = hns;
		scale = 32;

		while (lns >= 0x100000000ULL) {
			lns >>= 1;
			scale--;
		}
	}

	LARGE_INTEGER tick;
	err = !QueryPerformanceCounter(&tick);

	return (hns * tick.HighPart) + ((lns * tick.LowPart) >> scale);
}
