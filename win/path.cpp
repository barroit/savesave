// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"

int is_abs_path(const char *path)
{
	std::filesystem::path p = path;
	return p.is_absolute();
}

const char *get_home_dir()
{
	return getenv("USERPROFILE");
}
