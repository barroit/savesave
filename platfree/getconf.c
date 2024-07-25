// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "getconf.h"
#include "barroit/io.h"
#include "alloc.h"

static char *append_conf_path(const char *prefix, const char *conf)
{
	size_t len = strlen(prefix) + 1 + strlen(conf) + 1;
	char *path = xmalloc(len);

	if (snprintf(path, len, "%s/%s", prefix, conf) != len - 1)
		die("unable to construct config path string");

	return path;
}

static char *get_default_saveconf_path(void)
{
	char *path = getenv(DEFAULT_CONFIG_ENV);
	if (path) {
		path = strdup(path);
		goto check_path;
	}

	const char *home = get_home_dir();

	path = append_conf_path(home, DEFAULT_CONF_NAME);

check_path:
	if (access(path, R_OK) == 0)
		return path;

	free(path);
	return NULL;

}

static char *read_config(const char *path)
{
	char *strconf = readfile(path);
	if (!strconf) {
		error_errno("failed to read config from ‘%s’", path);
		return NULL;
	}

	return strconf;
}

/* access() with error handling */
static int access_directory(const char *path, int type)
{
	int err;

	err = access(path, type);
	if (err)
		return error_errno("can’t access directory ‘%s’", path);
	return 0;
}

/* stat with error handling */
static int stat_file(const char *path, struct stat *f)
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




static unsigned long str2ulong(const char *str, size_t len)
{
	if (isspace(*str) || *str == '-')
		die("unexpected leading char in unsigned value ‘%s’", str);

	char *tail;
	errno = 0;
	unsigned long val = strtoul(str, &tail, 10);
	if (errno != 0)
		die_errno("invalid unsigned value ‘%s’", str);
	else if (tail - str != len)
		die("unsigned value ‘%s’ contains an invalid char", str);

	return val;
}

static u16 interval_multiplier(char c)
{
	u16 mult = 1;
	switch (c) {
	case 'H':
	case 'h':
		mult *= 60;
	case 'M':
	case 'm':
		mult *= 60;
	case 'S':
	case 's':
		break;
	default:
		die("unknown time unit specifier ‘%c’", c);
	}

	return mult;
}

static u32 str2interval(const char *str)
{
	size_t n = strlen(str);
	u16 mult = 1;

	if (!isdigit(str[n - 1])) {
		mult = interval_multiplier(str[n - 1]);
		n -= 1;
	}

	unsigned long val = str2ulong(str, n);
	if (val == 0)
		die("interval cannot be 0");

	val *= mult;
	if (val > 2592000 /* 30 days */)
		die("interval ‘%s’ is too long (up to 30 days)", str);

	return val;
}

static u8 str2stack(const char *str)
{
	unsigned long val = str2ulong(str, strlen(str));
	if (val == 0)
		die("stack cannot be 0");
	if (val > 255)
		die("stack ‘%s’ is too large (up to 255 files)", str);
	return val;
}

static int parse_line(const char *line, size_t klen, const char *val,
		      unsigned lnum, struct savesave *conf)
{
	if (!strncmp(line, "save", klen)) {
		if (parse_save_path(val, conf))
			return 1;

	} else if (!strncmp(line, "backup", klen)) {
		if (parse_backup_path(val, conf))
			return 1;

	} else if (!strncmp(line, "interval", klen)) {
		conf->interval = str2interval(val);

	} else if (!strncmp(line, "stack", klen)) {
		conf->stack = str2stack(val);

	} else if (!strncmp(line, "snapshot", klen)) {
		conf->use_snapshot = !!str2ulong(val, strlen(val));

	} else if (!strncmp(line, "zip", klen)) {
		conf->use_zip = !!str2ulong(val, strlen(val));

	} else {
		return error("config contains an unrecognized key at %u:‘%s’",
			     lnum, line);
	}

	return 0;
}

static int parse_config(char *strconf, struct savesave *conf)
{
	unsigned lnum = 0;
	char *line = strtok(strconf, "\n");
	if (!line)
		return error("config is empty");

	int err;
	do {
		lnum++;
		if (*line == '#') /* comment */
			continue;

		char *col2 = strchr(line, ' ');
		if (!col2)
			goto err_incomp_conf;
		size_t klen = col2 - line;

		while (isspace(*col2))
			col2++;
		if (*col2 == 0)
			goto err_incomp_conf;

		err = parse_line(line, klen, col2, lnum, conf);
		if (err)
			return err;
	} while ((line = strtok(NULL, "\n")) != NULL);

	return 0;

err_incomp_conf:
	return error("config contains an incomplete line at %u:‘%s’",
		     lnum, line);
}

int parse_savesave_config(const char *path, struct savesave *conf)
{
	char *defpath = NULL;
	if (!path) {
		defpath = get_default_saveconf_path();
		path = defpath;
	}

	if (!path)
		return error("no configuration was provided");

	char *strconf = read_config(path);
	free(defpath);
	if (!strconf)
		return 1;

	int err = parse_config(strconf, conf);
	free(strconf);
	if (err)
		return 1;

	return 0;
}
