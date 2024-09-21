// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "dotsav.h"
#include "robio.h"
#include "termas.h"
#include "strbuf.h"
#include "alloc.h"
#include "list.h"
#include "debug.h"
#include "path.h"
#include "fileiter.h"
#include "mkdir.h"

struct dotsav {
	char *line;
	char *stop;

	struct savesave *sav;
	size_t nl;
	size_t cap;
};

char *read_dotsav(const char *name)
{
	int fd = open(name, O_RDONLY);
	if (fd == -1)
		goto err_open_file;

	int ret;
	struct stat st;

	ret = fstat(fd, &st);
	if (ret == -1)
		goto err_stat_file;

	char *buf = xmalloc(st.st_size + 1);
	buf[st.st_size] = 0;

	ret = robread(fd, buf, st.st_size);
	if (ret == -1)
		goto err_read_file;

	close(fd);
	return buf;

	if (0) {
	err_open_file:
		error_errno(ERRMAS_OPEN_FILE(name));
	} else if (0) {
	err_stat_file:
		error_errno(ERRMAS_STAT_FILE(name));
	} else if (0) {
	err_read_file:
		error_errno(ERRMAS_READ_FILE(name));
	}

	return NULL;
}

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

static int check_unique_savconf(const struct savesave *sav, size_t nr)
{
	size_t i;
	const char *name = sav[nr - 1].name;

	for_each_idx(i, nr - 1) {
		if (strcmp(sav[i].name, name))
			continue;

		return error("name `%s' has collisions at indexes %zu and %zu",
			     name, i, nr - 1);
	}

	return 0;
}

static int create_new_entry(const char *name,
			    struct savesave **sav, size_t *nr, size_t *cap)
{
	if (!*name)
		return error(_("empty sav name is not allowed"));
	else if (strchr(name, ' ') != NULL)
		return error(_("space in sav name is not allowed"));

	CAP_ALLOC(sav, *nr + 1, cap);

	struct savesave *c = &(*sav)[(*nr)++];
	memset(c, 0, sizeof(*c));

	c->name = xstrdup(name);
	c->use_snapshot = -1;
	c->use_compress = -1;
	return 0;
}

static inline int is_entry(const char *line, const char *prefix, char **ret)
{
	return skip_prefix(line, prefix, ret) == 0 && isspace(**ret);
}

static int parse_save(void *__save, struct savesave *sav)
{
	const char *save = *(const char **)__save;
	int err;
	struct stat st;

	if (!is_absolute_path(save))
		return error(_("save path `%s' is not absolute"), save);

	err = stat(save, &st);
	if (err)
		return error_errno(ERRMAS_STAT_FILE(save));

	if (S_ISREG(st.st_mode)) {
		sav->save_size = st.st_size;
		sav->is_dir_save = 0;
	} else if (S_ISDIR(st.st_mode)) {
		sav->is_dir_save = 1;
		return calc_dir_size(save, &sav->save_size);
	} else if (!st.st_size) {
		return error(_("unsupported save file `%s'"), save);
	}

	return 0;
}

static int check_stack(void *__stack, struct savesave *sav)
{
	u8 stack = *(u8 *)__stack;
	if (stack != 0)
		return 0;

	return error(_("stack cannot be 0"));
}

static int check_period(void *__period, struct savesave *sav)
{
	u32 period = *(u32 *)__period;
	if (period != 0)
		return 0;

	return error(_("period cannot be 0"));
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

#define SETENT(e, n, p, c, t)		\
do {					\
	(e)->name = (n);		\
	(e)->val  = (intptr_t)(p);	\
	(e)->cb   = (c);		\
	(e)->type = (t);		\
} while (0)

static int assign_entry(const char *line, char **rest,
			const struct savesave *sav, struct confent *ent)
{
	if (is_entry(line, "save", rest))
		SETENT(ent, "save", &sav->save_prefix, parse_save, STRING);
	else if (is_entry(line, "backup", rest))
		SETENT(ent, "backup", &sav->backup_prefix, NULL, STRING);
	else if (is_entry(line, "period", rest))
		SETENT(ent, "period", &sav->period, check_period, TIMESPAN);
	else if (is_entry(line, "stack", rest))
		SETENT(ent, "stack", &sav->stack, check_stack, U8);
	else if (is_entry(line, "snapshot", rest))
		SETENT(ent, "snapshot", &sav->use_snapshot, NULL, FLAG);
	else if (is_entry(line, "zip", rest))
		SETENT(ent, "zip", &sav->use_compress, NULL, FLAG);
	else
		return 1;

	return 0;
}

static int parse_entry_value(const char *rest, struct savesave *sav,
			     const struct confent *entry)
{
	rest = skip_space(rest);
	if (!*rest)
		return error(_("value is missing"));

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
		res = entry->cb((void *)entry->val, sav);

	return res;
}

static int parse_entry_line(const char *line, struct savesave *sav)
{
	int err;
	char *rest;
	struct confent entry;

	err = assign_entry(line, &rest, sav, &entry);
	if (err)
		return error(_("unrecognized sav key"));

	return parse_entry_value(rest, sav, &entry);
}

static int parse_savconf_line(struct dotsav *ctx)
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
		if (ctx->nl && check_unique_savconf(ctx->sav, ctx->nl) != 0)
			return 1;
		return create_new_entry(str, &ctx->sav, &ctx->nl, &ctx->cap);
	} else if (skip_prefix(line, "\t", &str) == 0 && isalpha(*str)) {
		return parse_entry_line(str, &ctx->sav[ctx->nl - 1]);
	} else {
		return error(_("encountered an unknown line"));
	}
}

static void validate_savconf(const struct savesave *c)
{
	struct strbuf sb = STRBUF_INIT;

	if (!c->save_prefix)
		strbuf_concat(&sb, "\tsave\n");
	if (!c->backup_prefix)
		strbuf_concat(&sb, "\tbackup\n");
	if (!c->period)
		strbuf_concat(&sb, "\tperiod\n");
	if (!c->stack)
		strbuf_concat(&sb, "\tstack\n");

	if (!sb.cap)
		return;

	size_t lines = strbuf_cntchr(&sb, '\n');
	die(lines > 1 ?
	    _("sav `%s' missing the following fields\n\n%s") :
	    _("sav `%s' missing the following field\n\n%s"),
	    c->name, sb.str);
}

static void update_backup_prefix(struct savesave *c)
{
	struct strbuf sb = STRBUF_INIT;
	if (c->use_compress)
		strbuf_printf(&sb, "%s/%s.%s.", c->backup_prefix,
			      c->name, CONFIG_ARCHIVE_EXTENTION);
	else
		strbuf_printf(&sb, "%s/%s.", c->backup_prefix, c->name);

	free(c->backup_prefix);
	c->backup_prefix = sb.str;
}

static void post_parse_savconf(struct savesave *sav, size_t nl)
{
	size_t i;
	struct savesave *c;
	for_each_idx(i, nl) {
		c = &sav[i];
		validate_savconf(c);

		if (c->use_compress == -1 &&
		    c->is_dir_save &&
		    c->save_size > CONFIG_COMPRESSING_THRESHOLD)
			c->use_compress = 1;

		if (c->use_snapshot == -1 &&
		    c->save_size > CONFIG_SNAPSHOT_THRESHOLD)
			c->use_snapshot = 1;

		update_backup_prefix(c);
	}
}

static void do_parse_savconf(struct dotsav *ctx)
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
			die(_("failed to parse dotsav\n"
			    "%7zu:%s"), cnt, ctx->line);

		if (is_final)
			break;

		ctx->line = ctx->stop + 1;
	} while (39);

	post_parse_savconf(ctx->sav, ctx->nl);

	CAP_ALLOC(&ctx->sav, ctx->nl + 1, &ctx->cap);
	memset(&ctx->sav[ctx->nl], 0, sizeof(*ctx->sav));
}

size_t parse_dotsav(char *savstr, struct savesave **sav)
{
	struct dotsav ctx = {
		.line  = savstr,
	};

	do_parse_savconf(&ctx);

	*sav = ctx.sav;
	return ctx.nl;
}

void print_dotsav(struct savesave *sav, size_t nl)
{
	size_t i;
	const struct savesave *c;

	/*
	 * for consistency, we can't use off_t, because there is no length
	 * modifier dedicated to this type
	 */
	ssize_t size;
	for_each_idx(i, nl) {
		c = &sav[i];
		size = c->save_size / 1000 / 1000;

		printf("%s\n", c->name);
		printf("	save	 %s\n", c->save_prefix);
		printf("	backup	 %s\n", c->backup_prefix);
		putchar('\n');

		printf("	size	 %" PRIdMAX "M\n", size);
		printf("	dirsave	 %d\n", c->is_dir_save);
		putchar('\n');

		printf("	compress %d\n", c->use_compress);
		printf("	snapshot %d\n", c->use_snapshot);
		putchar('\n');

		printf("	period	 %" PRIu32 "\n", c->period);
		printf("	stack	 %" PRIu8 "\n", c->stack);
		putchar('\n');
	}
}

char *get_dotsav_defpath(void)
{
	struct strbuf sb = STRBUF_INIT;
	char *path = getenv("SAVESAVE");

	if (path) {
		strbuf_concat(&sb, path);
	} else {
		const char *home = get_home_dirname();
		strbuf_concat_path(&sb, home, ".savesave");
	}

	if (access(sb.str, R_OK) == 0) {
		strbuf_normalize_path(&sb);
		return sb.str;
	}

	strbuf_destroy(&sb);
	return NULL;
}
