// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "alloc.h"
#include "termas.h"

int path_is_abs(const char *path)
{
	return !PathIsRelative(path);
}

const char *home_dir(void)
{
	return getenv("USERPROFILE");
}

const char *exec_path(void)
{
	static char path[PATH_MAX + 1];
	if (!*path) {
		DWORD len = GetModuleFileName(NULL, path, sizeof(path));
		BUG_ON(len == 0 || len == sizeof(path));
	}

	return path;
}

const char *tmp_dir(void)
{
	static char path[PATH_MAX + 1];
	if (!*path) {
		/* THIS GODDAMN MOTHERFUCKING PIECE OF SHIT WINDOWS */
		DWORD nr = GetTempPath(sizeof(path), path);
		if (!nr)
			die_winerr(_("failed to retrieve temporary directory name"));
		path[nr - 1] = 0;
	}

	return path;
}
