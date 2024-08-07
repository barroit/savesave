// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "getconf.h"

const char *get_home_dir()
{
	return getenv("USERPROFILE");
}
