// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"

rlim_t getfdavail(void)
{
	return 39;
}

static SYSTEM_INFO *getsysinfo(void)
{
	static SYSTEM_INFO info;

	if (unlikely(!info.dwPageSize))
		GetSystemInfo(&info);

	return &info;
}

uint getcpucores(void)
{
	SYSTEM_INFO *info = getsysinfo();
	return info->dwNumberOfProcessors;
}
