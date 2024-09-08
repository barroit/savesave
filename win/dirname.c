// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

char *dirname(char *path)
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath(path, drive, dir, NULL, NULL);
	memcpy(path, drive, 2);

	size_t len = strlen(dir);
	if (dir[len - 1] == '\\' || dir[len - 1] == '/')
		dir[--len] = 0;

	memcpy(&path[2], dir, len + 1);
	return path;
}
