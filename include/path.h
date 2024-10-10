/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PATH_H
#define PATH_H

int is_absolute_path(const char *path);

static inline int is_dir_indicator(const char *name)
{
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

const char *get_home_dirname(void);

const char *get_executable_dirname(void);

const char *get_tmp_dirname(void);

#define PROCID_NAME "savesave.pid"
#define SAVLOG_NAME "savesave.log"

#define DATA_DIRLIST_INIT {			\
	[0] = getenv("XDG_RUNTIME_DIR"),	\
	[1] = get_tmp_dirname(),		\
	[2] = get_executable_dirname(),		\
}

const char *get_data_dirname(void);

const char *get_procid_filename(void);

const char *get_locale_dirname(void);

const char *get_dotsav_filename(void);

const char *get_log_filename(void);

int readlink_notrunc(const char *name, char **target);
#define READLINK readlink_notrunc

#endif /* PATH_H */
