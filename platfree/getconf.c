// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "getconf.h"
#include "barroit/io.h"
#include "alloc.h"
#include "termsg.h"

#define errconf(...) \
	__error_routine("error:config: ", 0, __VA_ARGS__)

#define errconf_errno(...) \
	__error_routine("error:config: ", getstrerror(), __VA_ARGS__)

#define dieconf(...) \
	__die_routine("fatal:config: ", 0, __VA_ARGS__)

#define dieconf_errno(...) \
	__die_routine("fatal:config: ", getstrerror(), __VA_ARGS__)

static char *append_conf_path(const char *prefix, const char *conf)
{
	size_t len = strlen(prefix) + 1 + strlen(conf) + 1;
	char *path = xmalloc(len);

	if (snprintf(path, len, "%s/%s", prefix, conf) != len - 1)
		dieconf("unable to construct path");

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

/* access() with error handling */
static int access_directory(const char *path, int type)
{
	int err;

	err = access(path, type);
	if (err)
		return errconf_errno("can’t access directory ‘%s’", path);
	return 0;
}

/* access() with error handling */
static int access_regular(const char *path, int type)
{
	int err;

	err = access(path, type);
	if (err)
		return errconf_errno("can’t access regular file ‘%s’", path);
	return 0;
}

/* stat with error handling */
static int stat_file(const char *path, struct stat *f)
{
	int err;

	err = stat(path, f);
	if (err)
		return errconf_errno("can’t access ‘%s’", path);
	return 0;
}

static void parse_dir_save(const char *path, struct savesave *conf)
{
	int err;

	err = access_directory(path, R_OK | X_OK);
	EXIT_ON(err);

	/* TODO: check user defined use_snapshot compatibility here */

	conf->use_snapshot = 1;
	conf->use_zip = 1;
}

static void parse_reg_save(const char *path, off_t size, struct savesave *conf)
{
	int err;

	err = access_regular(path, R_OK);
	EXIT_ON(err);

	/* TODO: check user defined use_snapshot compatibility here */

	if (size > CONFIG_SNAPSHOT_THRESHOLD_SIZE_KB * 1024)
		conf->use_snapshot = 1;
	if (size > CONFIG_ZIP_THRESHOLD_SIZE_KB * 1024)
		conf->use_zip = 1;
}

int parse_save_path(const char *path, struct savesave *conf)
{
	int err;
	struct stat save;

	err = stat_file(path, &save);
	EXIT_ON(err);

	if (S_ISDIR(save.st_mode)) {
		parse_dir_save(path, conf);
	} else if (S_ISREG(save.st_mode)) {
		parse_reg_save(path, save.st_size, conf);
	} else {
		return errconf("unsupported save file mode ‘%d’",
			       save.st_mode);
	}

	conf->save = strdup(path);
	return 0;
}

int parse_backup_path(const char *path, struct savesave *conf)
{
	int err;
	struct stat backup;

	err = stat_file(path, &backup);
	EXIT_ON(err);

	if (!S_ISDIR(backup.st_mode)) {
		return errconf("path ‘%s’ does not point to a directory",
			       path);
	}

	err = access_directory(path, X_OK | W_OK);
	EXIT_ON(err);

	conf->backup = strdup(path);
	return 0;
}

static unsigned long str2ulong(const char *str, size_t len)
{
	if (isspace(*str) || *str == '-')
		dieconf("unexpected leading char in unsigned value ‘%s’", str);

	char *tail;
	errno = 0;
	unsigned long val = strtoul(str, &tail, 10);
	if (errno != 0)
		dieconf_errno("invalid unsigned value ‘%s’", str);
	else if (tail - str != len)
		dieconf("unsigned value ‘%s’ contains an invalid char", str);

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
		dieconf("unknown time unit specifier ‘%c’", c);
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
		dieconf("interval cannot be 0");

	val *= mult;
	if (val > 2592000 /* 30 days */)
		dieconf("interval ‘%s’ is too long (up to 30 days)", str);

	return val;
}

static u8 str2stack(const char *str)
{
	unsigned long val = str2ulong(str, strlen(str));
	if (val == 0)
		dieconf("stack cannot be 0");
	if (val > 255)
		dieconf("stack ‘%s’ is too large (up to 255 files)", str);

	return val;
}

static int parse_line(const char *line, size_t klen, const char *val,
		      unsigned lnum, struct savesave *conf)
{
	if (!strncmp(line, "save", klen)) {
		return parse_save_path(val, conf);

	} else if (!strncmp(line, "backup", klen)) {
		return parse_backup_path(val, conf);

	} else if (!strncmp(line, "interval", klen)) {
		conf->interval = str2interval(val);

	} else if (!strncmp(line, "stack", klen)) {
		conf->stack = str2stack(val);

	} else if (!strncmp(line, "snapshot", klen)) {
		conf->use_snapshot = !!str2ulong(val, strlen(val));

	} else if (!strncmp(line, "zip", klen)) {
		conf->use_zip = !!str2ulong(val, strlen(val));

	} else {
		return errconf("an unrecognized key was found at\n"
			       "line%8u: %s", lnum, line);
	}

	return 0;
}

static int parse_config(char *strconf, struct savesave *conf)
{
	unsigned lnum = 0;
	char *line = strconf;
	char *next = strchr(strconf, '\n');
	if (!next)
		return errconf("not a valid file");
	*next = 0;
	next += 1;

	int err;
	do {
		lnum++;
		if (*line == 0) /* empty line */
			goto get_next_line;
		else if (*line == '#') /* comment */
			goto get_next_line;
		else if (isspace(*line))
			goto err_incomp_conf;

		char *col2 = strchr(line, ' ');
		if (!col2)
			goto err_incomp_conf;
		size_t klen = col2 - line;

		while (isspace(*col2))
			col2++;
		if (*col2 == 0)
			goto err_incomp_conf;

		err = parse_line(line, klen, col2, lnum, conf);
		EXIT_ON(err);

get_next_line:
		line = next;
		next = strchr(next, '\n');
		if (unlikely(!next)) {
			line = NULL;
		} else {
			*next = 0;
			next += 1;
		}
	} while (line != NULL);

	return 0;

err_incomp_conf:
	return errconf("an invalid line was found at\n"
		       "line%8u: %s", lnum, line);
}

int parse_savesave_config(const char *path, struct savesave *conf)
{
	char *defpath = NULL;
	if (!path) {
		defpath = get_default_saveconf_path();
		path = defpath;
	}

	if (!path)
		return errconf("no configuration was provided");

	char *strconf = readfile(path);
	free(defpath);
	if (!strconf)
		return errconf_errno("failed to read content from ‘%s’", path);
	else if (*strconf == 0)
		return errconf("file is empty");

	int err = parse_config(strconf, conf);
	EXIT_ON(err);
	free(strconf);

	return 0;
}
