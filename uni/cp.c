// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "cp.h"
#include "strbuf.h"
#include "termas.h"

int copyfile(int src, int dest, off_t len)
{
	ssize_t copied;
	do {
		copied = copy_file_range(src, NULL, dest, NULL, len, 0);
		if (copied == -1)
			return -1;

		len -= copied;
	} while (len > 0 && copied > 0);

	return 0;
}

int get_link_target2(const char *name, struct strbuf *__buf)
{
	int err;
	struct stat st;

	err = lstat(name, &st);
	if (err)
		return -1;

	size_t len = st.st_size ? st.st_size + 1 : PATH_MAX;
	strbuf_require_cap(__buf, len);

	ssize_t nr = readlink(name, __buf->str, len);
	if (nr == -1)
		return -1;

	__buf->str[nr] = 0;
	__buf->len = nr;
	if (nr == len)
		warn(_("path `%s' may have been truncated"), __buf->str);

	return 0;
}
