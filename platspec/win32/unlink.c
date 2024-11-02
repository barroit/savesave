// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

int __unlink(const char *name)
{
	int ret;

retry:
	ret = _unlink(name);

	if (unlikely(ret && errno == EACCES)) {
		SetFileAttributes(name, FILE_ATTRIBUTE_NORMAL);
		goto retry;
	}

	return ret;
}
