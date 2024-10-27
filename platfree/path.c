// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "command.h"
#include "path.h"
#include "list.h"
#include "strbuf.h"
#include "termas.h"
#include "noleak.h"

static const char *find_data_dirname(void)
{
	int err;
	size_t i;
	const char *dir[] = DATA_DIR_LIST_INIT;

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

	/*
	 * Even the executable directory is not available !?
	 * And, make compiler happy :(
	 */
	bug("???");
}

const char *data_dir(void)
{
	static const char *dir;

	if (!dir)
		dir = find_data_dirname();

	return dir;
}

static char *filename_at_datadir(const char *name)
{
	const char *prefix = data_dir();
	struct strbuf sb = STRBUF_INIT;

	strbuf_concat_pathname(&sb, prefix, name);
	return sb.str;
}

const char *pid_path(void)
{
	static const char *path;

	if (!path)
		path = filename_at_datadir(PROCID_NAME);

	return path;
}

const char *locale_dir(void)
{
	static const char *path;

	if (!path) {
		const char *dir = exec_dir();
		struct strbuf sb = STRBUF_INIT;

		strbuf_concat_pathname(&sb, dir, "locale");

		/*
		 * gettext expect separator is shash
		 */
		strbuf_normalize_path(&sb);
		path = sb.str;
	}

	return path;
}

const char *__dotsav_path(void)
{
	static const char *path;

	if (!path) {
		struct strbuf sb = STRBUF_INIT;
		const char *home = home_dir();

		strbuf_concat_pathname(&sb, home, ".savesave");
		path = sb.str;
	}

	return path;
}

const char *dotsav_path(void)
{
	static const char *path;

	if (!path)
		path = cm_dotsav_path;

	if (!path)
		path = getenv("SAVESAVE");

	if (!path)
		path = __dotsav_path();

	return path;
}

const char *__output_path(void)
{
	static const char *path;

	if (!path)
		path = filename_at_datadir(SAVLOG_NAME);

	return path;
}

const char *output_path(void)
{
	if (!cm_has_output)
		return NULL_DEVICE;

	static const char *path;

	if (!path)
		path = cm_output_path;

	if (!path)
		path = __output_path();

	return path;
}
