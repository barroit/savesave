// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "list.h"
#include "debug.h"
#include "strbuf.h"

static const char *get_procid_dirname(void)
{
	int err;
	size_t i;
	const char *dir[] = PROCID_DIRLIST_INIT;

	for_each_idx(i, sizeof_array(dir)) {
		const char *name = dir[i];
		if (name == NULL)
			continue;

		struct stat st;
		err = stat(name, &st);
		if (err)
			continue;

		if (!S_ISDIR(st.st_mode))
			continue;

		err = access(name, X_OK | W_OK);
		if (err)
			continue;

		return name;
	}

	BUG_ON(1); /* even executable directory is not available !? */
}

const char *get_procid_filename(void)
{
	static const char *path;
	if (!path) {
		const char *prefix = get_procid_dirname();
		struct strbuf sb = STRBUF_INIT;

		strbuf_printf(&sb, "%s/%s", prefix, PROCID_NAME);
		path = sb.str;
	}

	return path;
}
