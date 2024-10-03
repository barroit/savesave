// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */
// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termas.h"

rlim_t getfdavail(void)
{
	return 39;
}

static SYSTEM_INFO *getsysinfo(void)
{
	static SYSTEM_INFO info;

	if (unlikely(!info))
		GetSystemInfo(&info);

	return &info;
}

uint getcpucore(void)
{
	SYSTEM_INFO *info = getsysinfo();
	return info->dwNumberOfProcessors;
}
