// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "getconf.h"
#include "barroit/io.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "alloc.h"
#include <ctype.h>
#include <inttypes.h>

#ifdef WINDOWS_NATIVE
# define access _access
# define strdup _strdup
# define R_OK 04
#endif

char *read_config(const char *path)
{
	char *defpath = NULL;
	if (!path) {
		defpath = get_default_config_path();
		path = defpath;
	}

	if (!path) {
		error("no configuration was provided");
		return NULL;
	}

	char *strconf = readfile(path);
	free(defpath);
	if (!strconf) {
		error_errno("failed to read config from ‘%s’", path);
		return NULL;
	}

	return strconf;
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

int parse_config(char *strconf, struct savesave *conf)
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
