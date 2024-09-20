// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "debug.h"
#include "alloc.h"

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
	static char path[PATH_MAX + 1];
	if (!*path) {
		DWORD len = GetModuleFileName(NULL, path, sizeof(path));
		BUG_ON(len == 0 || len == sizeof(path));
	}

	return path;
}

const char *get_tmp_dirname(void)
{
	static char path[PATH_MAX + 1];
	if (!*path) {
		/* THIS GODDAMN MOTHERFUCKING PIECE OF SHIT WINDOWS */
		DWORD nr = GetTempPath(sizeof(path), path);
		BUG_ON(!nr);
		path[nr - 1] = 0;
	}

	return path;
}
