// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "debug.h"
#include "alloc.h"

static char *executable_dirname;

CONSTRUCTOR(setup_executable_dirname)
{
	char exe[PATH_MAX];
	DWORD len = GetModuleFileName(NULL, exe, sizeof(exe));

	BUG_ON(len == 0);

	char *dir = dirname(exe);
	executable_dirname = xstrdup(dir);
}

int is_absolute_path(const char *path)
{
	return !PathIsRelative(path);
}

const char *get_home_dirname(void)
{
	return getenv("USERPROFILE");
}

const char *get_executable_dirname(void)
{
	return executable_dirname;
}
