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
#include "maincmd.h"

static const char *find_data_dirname(void)
{
	int err;
	size_t i;
	const char *dir[] = DATA_DIRLIST_INIT;

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

const char *get_data_dirname(void)
{
	static const char *dir;

	if (!dir)
		dir = find_data_dirname();

	return dir;
}

static char *filename_at_datadir(const char *name)
{
	const char *prefix = get_data_dirname();
	struct strbuf sb = STRBUF_INIT;

	strbuf_concat_path(&sb, prefix, name);
	return sb.str;
}

const char *get_procid_filename(void)
{
	static const char *path;

	if (!path)
		path = filename_at_datadir(PROCID_NAME);

	return path;
}

const char *get_locale_dirname(void)
{
	static const char *path;

	if (!path) {
		const char *dir = get_executable_dirname();
		struct strbuf sb = STRBUF_INIT;

		strbuf_concat_path(&sb, dir, "locale");

		/*
		 * gettext expect separator is shash
		 */
		strbuf_normalize_path(&sb);
		path = sb.str;
	}

	return path;
}

const char *get_dotsav_filename(void)
{
	static const char *path;

	if (!path)
		path = getenv("SAVESAVE");

	if (!path) {
		struct strbuf sb = STRBUF_INIT;
		const char *home = get_home_dirname();

		strbuf_concat_path(&sb, home, ".savesave");
		path = sb.str;
	}

	return path;
}

const char *get_log_filename(void)
{
	static const char *path;

	if (!path)
		path = userspec.lr_log_path;
	if (!path)
		path = filename_at_datadir(SAVLOG_NAME);
	BUG_ON(!path);

	return path;
}
