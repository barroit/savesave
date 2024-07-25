// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include <stdlib.h>
#include "getconf.h"
#include <pwd.h>
#include "termsg.h"
#include <string.h>
#include "alloc.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

static char *get_sudo_user_dir(void)
{
	const char *user = getenv("SUDO_USER");
	if (!user)
		return NULL;

	struct passwd *pw = getpwnam(user);
	if (!pw) {
		warn_errno("can’t get home for ‘%s’ who invoked sudo", user);
		return NULL;
	}

	return pw->pw_dir;
}

static char *append_conf_path(const char *prefix, const char *conf)
{
	size_t len = strlen(prefix) + 1 + strlen(conf) + 1;
	char *path = xmalloc(len);

	if (snprintf(path, len, "%s/%s", prefix, conf) != len - 1)
		die("unable to construct config path string");

	return path;
}

char *get_default_config_path(void)
{
	char *path = getenv(DEFAULT_CONFIG_ENV);
	if (path) {
		path = strdup(path);
		goto check_path;
	}

	const char *home = getenv("HOME");
	if (!home)
		home = get_sudo_user_dir();
	if (!home)
		return NULL;

	path = append_conf_path(home, DEFAULT_CONF_NAME);

check_path:
	if (access(path, R_OK)) {
		free(path);
		return NULL;
	}

	return path;
}

/* access() with error handling */
static inline int access_directory(const char *path, int type)
{
	int err;

	err = access(path, type);
	if (err)
		return error_errno("can’t access directory ‘%s’", path);
	return 0;
}

/* stat with error handling */
static inline int stat_file(const char *path, struct stat *f)
{
	int err;

	err = stat(path, f);
	if (err)
		return error_errno("can’t access ‘%s’", path);
	return 0;
}

static int parse_dir_save(const char *path, struct savesave *conf)
{
	int err;

	err = access_directory(path, R_OK | X_OK);
	if (err)
		return 1;

	/* TODO: check user defined use_snapshot compatibility here */

	conf->use_snapshot = 1;
	conf->use_zip = 1;

	return 0;
}

static int parse_reg_save(const char *path, off_t size, struct savesave *conf)
{
	int err;

	err = access(path, R_OK);
	if (err)
		return error_errno("can’t access file ‘%s’", path);

	/* TODO: check user defined use_snapshot compatibility here */

	if (size > CONFIG_SNAPSHOT_THRESHOLD_SIZE_KB * 1024)
		conf->use_snapshot = 1;
	if (size > CONFIG_ZIP_THRESHOLD_SIZE_KB * 1024)
		conf->use_zip = 1;

	return 0;
}

int parse_save_path(const char *path, struct savesave *conf)
{
	int err;
	struct stat save;

	err = stat_file(path, &save);
	if (err)
		return 1;

	if (S_ISDIR(save.st_mode))
		err = parse_dir_save(path, conf);
	else if (S_ISREG(save.st_mode))
		err = parse_reg_save(path, save.st_size, conf);
	else
		err = error("unsupported save file mode ‘%d’", save.st_mode);

	if (err)
		return 1;

	conf->save = strdup(path);
	return 0;
}

int parse_backup_path(const char *path, struct savesave *conf)
{
	int err;
	struct stat backup;

	err = stat_file(path, &backup);
	if (err)
		return 1;

	if (!S_ISDIR(backup.st_mode))
		return error("path ‘%s’ does not point to a directory", path);

	err = access_directory(path, X_OK | W_OK);
	if (err)
		return 1;

	conf->backup = strdup(path);
	return 0;
}
