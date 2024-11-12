// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

char *basename(char *path)
{
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	_splitpath(path, NULL, NULL, fname, ext);

	size_t namlen = strlen(fname);
	size_t extlen = strlen(ext);

	memcpy(path, fname, namlen);
	memcpy(&path[namlen], ext, extlen + 1);

	return path;
}
