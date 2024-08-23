// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "savconf.h"
#include "barroit/io.h"
#include "termsg.h"
#include "strbuf.h"
#include "alloc.h"
#include "list.h"
#include "text2num.h"
#include "barroit/limits.h"

struct savesave_context {
	char *line;
	char *stop;

	struct savesave *conf;
	size_t nl;
	size_t cap;
};

static char *skip_space(const char *str)
{
	while (isspace(*str))
		str++;
	return (char *)str;
}

static int skip_prefix(const char *str, const char *prefix, char **ret)
{
	do {
		if (!*prefix) {
			*ret = (char *)str;
			return 0;
		}
	} while (*str++ == *prefix++);

	return 1;
}

static int check_unique_savconf(const struct savesave *conf, size_t nr)
{
	const char *name = conf[nr - 1].name;
	size_t i;
	for_each_idx(i, nr - 1) {
		if (likely(strcmp(conf[i].name, name)))
			continue;
		return error("configuration name ‘%s’ conflicts at config indexes %zd and %zd",
			     name, i, nr - 1);
	}

	return 0;
}

static int create_new_entry(const char *name,
			    struct savesave **conf, size_t *nr, size_t *cap)
{
	if (!*name)
		return error("empty configuration name is not allowed");
	else if (strchr(name, ' ') != NULL)
		return error("space in configuration name is not allowed");

	CAP_ALLOC(conf, *nr + 1, cap);

	struct savesave *c = &(*conf)[(*nr)++];
	memset(c, 0, sizeof(*c));

	c->name = xstrdup(name);
	c->use_snapshot = -1;
	c->use_zip = -1;
	return 0;
}

static inline int is_entry(const char *line, const char *prefix, char **ret)
{
	return skip_prefix(line, prefix, ret) == 0 && isspace(**ret);
}

static int parse_save(void *__save, struct savesave *conf)
{
	const char *save = *(const char **)__save;

	int err;
	struct stat st;

	if (!is_abs_path(save))
		return error("save path ‘%s’ is not an absolute path", save);

	err = stat(save, &st);
	if (err)
		return error("save path ‘%s’ does not exist", save);

	if (S_ISREG(st.st_mode)) {
		conf->save_size = st.st_size;
		conf->is_dir_save = 0;
	} else if (S_ISDIR(st.st_mode)) {
		conf->is_dir_save = 1;
		return calc_dir_size(save, &conf->save_size);
	} else {
		return error("unsupported save file mode ‘%d’", st.st_mode);
	}

	return 0;
}

static int prepare_backup(void *__backup, struct savesave *_)
{
	const char *backup = *(const char **)__backup;

	int err;

	err = my_mkdir(backup);
	if (err && errno != EEXIST)
		return error_errno("failed to create backup directory ‘%s’",
				   backup);

	struct stat st;
	err = stat(backup, &st);
	if (err)
		return error_errno("unable to get metadata for backup directory ‘%s’",
				   backup);
	if (!S_ISDIR(st.st_mode))
		return error("file ‘%s’ is not a directory", backup);

	err = access(backup, W_OK | X_OK);
	if (err)
		return error_errno("unable to access backup directory ‘%s’",
				   backup);

	return 0;
}

static int check_stack(void *__stack, struct savesave *_)
{
	u8 stack = *(u8 *)__stack;
	if (stack == 0)
		return error("stack cannot be 0");
	return 0;
}

static int check_period(void *__period, struct savesave *_)
{
	u32 period = *(u32 *)__period;
	if (period == 0)
		return error("period cannot be 0");
	return 0;
}

enum entval {
	STRING,
	TIMESPAN,
	U8,
	FLAG,
};

typedef int (*entry_callback)(void *, struct savesave *);

struct confent {
	const char *name;
	intptr_t val;
	entry_callback cb;
	enum entval type;
};

#define SETENT(e, n, p, c, t)			\
	do {					\
		(e)->name = (n);		\
		(e)->val  = (intptr_t)(p);	\
		(e)->cb   = (c);		\
		(e)->type = (t);		\
	} while (0)

static int assign_entry(const char *line, char **rest,
			const struct savesave *conf, struct confent *ent)
{
	if (is_entry(line, "save", rest))
		SETENT(ent, "save", &conf->save, parse_save, STRING);
	else if (is_entry(line, "backup", rest))
		SETENT(ent, "backup", &conf->backup, prepare_backup, STRING);
	else if (is_entry(line, "period", rest))
		SETENT(ent, "period", &conf->period, check_period, TIMESPAN);
	else if (is_entry(line, "stack", rest))
		SETENT(ent, "stack", &conf->stack, check_stack, U8);
	else if (is_entry(line, "snapshot", rest))
		SETENT(ent, "snapshot", &conf->use_snapshot, NULL, FLAG);
	else if (is_entry(line, "zip", rest))
		SETENT(ent, "zip", &conf->use_zip, NULL, FLAG);
	else
		return 1;

	return 0;
}

static int parse_entry_value(const char *rest, struct savesave *conf,
			     const struct confent *entry)
{
	rest = skip_space(rest);
	if (!*rest)
		return error("name ‘%s’ must be defined with a value",
			     entry->name);

	int res = 0;
	switch (entry->type) {
	case STRING:
		*(char **)entry->val = xstrdup(rest);
		break;
	case TIMESPAN:
		res = str2period(rest, (u32 *)entry->val);
		break;
	case U8:
		res = str2u8(rest, (u8 *)entry->val);
		break;
	case FLAG:
		res = str2bool(rest, (int *)entry->val);
	}

	if (res == 0 && entry->cb)
		res = entry->cb((void *)entry->val, conf);

	return res;
}

static int parse_entry_line(const char *line, struct savesave *conf)
{
	int err;
	char *rest;
	struct confent entry;

	err = assign_entry(line, &rest, conf, &entry);
	if (err)
		return error("unrecognized name found in ‘%s’", line);

	return parse_entry_value(rest, conf, &entry);
}

static int parse_savconf_line(struct savesave_context *ctx)
{
	char *line = ctx->line;
	char *stop = ctx->stop;

	*stop = 0;
	if (*line == 0)
		return 0;

	char *str = skip_space(line);
	if (*str == '#' || *str == 0)
		return 0;

	if (skip_prefix(line, "config ", &str) == 0) {
		if (ctx->nl && check_unique_savconf(ctx->conf, ctx->nl) != 0)
			return 1;
		return create_new_entry(str, &ctx->conf, &ctx->nl, &ctx->cap);
	} else if (skip_prefix(line, "\t", &str) == 0 && isalpha(*str)) {
		return parse_entry_line(str, &ctx->conf[ctx->nl - 1]);
	} else {
		return error("encountered an unknown line");
	}
}

static void validate_savconf(const struct savesave *c)
{
	struct strbuf sb = STRBUF_INIT;

	if (!c->save)
		strbuf_append(&sb, "\tsave\n");
	if (!c->backup)
		strbuf_append(&sb, "\tbackup\n");
	if (!c->period)
		strbuf_append(&sb, "\tperiod\n");
	if (!c->stack)
		strbuf_append(&sb, "\tstack\n");

	if (!sb.cap)
		return;

	size_t lines = strbuf_cntchr(&sb, '\n');
	die("configuration ‘%s’ missing the following field%s\n\n%s",
	    c->name, lines > 1 ? "s" : "", sb.str);
}

static void update_backup_path(struct savesave *c)
{
	char *old = c->backup;
	char *buf = xmalloc(PATH_MAX);
	int n = xsnprintf(buf, PATH_MAX, "%s/%s.", c->backup, c->name);

	if (c->use_zip) {
		size_t len = sizeof("zip.");
		if (n + len > PATH_MAX)
			goto err_overflow;

		memcpy(buf + n, "zip.", len);
		n += len - 1;
	}

	/*
	 * STRU8_MAX contains null terminator length
	 */
	if (n + STRU8_MAX > PATH_MAX)
		goto err_overflow;

	c->backup_len = n;
	free(old);
	c->backup = buf;
	return;

err_overflow:
	die("backup path ‘%s’ of configuration ‘%s’ is too long",
	    c->backup, c->name);
}

static void post_parse_savconf(struct savesave *conf, size_t nl)
{
	size_t i;
	struct savesave *c;
	for_each_idx(i, nl) {
		c = &conf[i];
		validate_savconf(c);

		if (c->use_zip == -1 &&
		    c->is_dir_save &&
		    c->save_size > CONFIG_DO_ZIP_THRESHOLD)
			c->use_zip = 1;

		if (c->use_snapshot == -1 &&
		    c->save_size > CONFIG_DO_SNAPSHOT_THRESHOLD)
			c->use_snapshot = 1;

		update_backup_path(c);
	}
}

static void do_parse_savconf(struct savesave_context *ctx)
{
	int err;

	size_t cnt = 0;
	int is_final = 0;
	do {
		cnt++;
		ctx->stop = strchrnul(ctx->line, '\n');
		if (!*ctx->stop)
			is_final = 1;

		err = parse_savconf_line(ctx);
		if (err)
			die("failed to parse savesave configuration\n"
			    "%7" PRIuMAX ":%s", cnt, ctx->line);

		if (is_final)
			break;

		ctx->line = ctx->stop + 1;
	} while (39);

	post_parse_savconf(ctx->conf, ctx->nl);

	CAP_ALLOC(&ctx->conf, ctx->nl + 1, &ctx->cap);
	memset(&ctx->conf[ctx->nl], 0, sizeof(*ctx->conf));
}

size_t parse_savconf(const char *path, struct savesave **conf)
{
	char *str = readfile(path);
	if (!str)
		die_errno("failed to read content from ‘%s’", path);
	else if (*str == 0)
		die("file ‘%s’ is empty", path);

	struct savesave_context ctx = {
		.line  = str,
	};

	do_parse_savconf(&ctx);
	free(str);

	*conf = ctx.conf;
	return ctx.nl;
}

void print_savconf(const struct savesave *conf, size_t nl)
{
	size_t i;
	const struct savesave *c;

	/*
	 * for consistency, we can’t use off_t, because there is no length
	 * modifier dedicated to this type
	 */
	ssize_t size;
	for_each_idx(i, nl) {
		c = &conf[i];
		size = c->save_size / 1000 / 1000;
		printf("%s\n"
		       "\tsave\t %s\n"
		       "\tsize\t %" PRIiMAX "M\n"
		       "\tdir\t %d\n"
		       "\tbackup\t %s\n"
		       "\tperiod\t %" PRIu32 "\n"
		       "\tstack\t %" PRIu8 "\n"
		       "\tsnapshot %d\n"
		       "\tzip\t %d\n\n",
		       c->name, c->save, size, c->is_dir_save, c->backup,
		       c->period, c->stack, c->use_snapshot, c->use_zip);
	}
}

char *get_default_savconf_path(void)
{
	struct strbuf sb = STRBUF_INIT;
	char *path = getenv(CONFIG_DEFAULT_SAVCONF_ENVNAME);

	if (path) {
		strbuf_append(&sb, path);
	} else {
		const char *home = get_home_dir();
		strbuf_printf(&sb, "%s/%s", home,
			      CONFIG_DEFAULT_SAVCONF_BASENAME);
	}

	if (access(sb.str, R_OK) == 0)
		return sb.str;

	strbuf_destroy(&sb);
	return NULL;
}
