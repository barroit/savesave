/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef PATH_H
#define PATH_H

int path_is_abs(const char *path);

static inline int path_is_dir_ind(const char *name)
{
	return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

const char *home_dir(void);

const char *exec_path(void);

const char *exec_dir(void);

const char *tmp_dir(void);

#define PROCID_NAME "savesave.pid"
#define SAVLOG_NAME "savesave.log"

#define DATA_DIR_LIST_INIT {			\
	[0] = getenv("XDG_RUNTIME_DIR"),	\
	[1] = tmp_dir(),			\
	[2] = exec_path(),		\
}

const char *data_dir(void);

const char *pid_path(void);

const char *locale_dir(void);

/*
 * Return default (built-in) path of dotsav.
 */
const char *__dotsav_path(void);

/*
 * Similar to __dotsav_path(), but return value is affected by cm_dotsav_path.
 */
const char *dotsav_path(void);

/*
 * Return default (built-in) path of log file.
 */
const char *__output_path(void);

/*
 * Similar to __output_path(), but return value is affected by cm_output_path.
 */
const char *output_path(void);

/*
 * Same as readlink(2) but no truncation happens (target is intact)
 */
int readlink_nt(const char *name, char **target);
#define READLINK readlink_nt

#endif /* PATH_H */
