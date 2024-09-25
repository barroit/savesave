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

#pragma GCC diagnostic ignored "-Wswitch"

enum savtype {
	SAVENT_END,

	SAVENT_ENTSEP,

	SAVENT_STRING,
	SAVENT_TIMESPAN,
	SAVENT_UINT8,
	SAVENT_FLAG,
};

struct savent {
	enum savtype type;
	const char *name;

	size_t offset;
	int (*cb)(struct savesave *, void *);
};

#define SE_END() {		\
	.type = SAVENT_ENTSEP,	\
}

#define SAVENTOF(t, n, m, c) {			\
	.type   = t,				\
	.name   = n,				\
	.offset = offsetof(struct savesave, m),	\
	.cb     = c,				\
}

#define SE_ENTSEP(n, m, c)   SAVENTOF(SAVENT_ENTSEP, n, m, c)
#define SE_STRING(n, m, c)   SAVENTOF(SAVENT_STRING, n, m, c)
#define SE_TIMESPAN(n, m, c) SAVENTOF(SAVENT_TIMESPAN, n, m, c)
#define SE_UINT8(n, m, c)    SAVENTOF(SAVENT_UINT8, n, m, c)
#define SE_FLAG(n, m, c)     SAVENTOF(SAVENT_FLAG, n, m, c)

struct dotsav {
	char *line;

	struct savent *ent;

	struct savesave *savarr;
	size_t savnl;
	size_t savcap;
};

#define for_each_savent(ent) for (; ent->type != SAVENT_END; ent++)

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

static int interpret_save(struct savesave *sav, void *data)
{
	const char *save = *(const char **)data;
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

static int check_stack(struct savesave *sav, void *data)
{
	u8 stack = *(u8 *)data;
	if (stack != 0)
		return 0;

	return error(_("stack cannot be 0"));
}

static int check_period(struct savesave *sav, void *data)
{
	u32 period = *(u32 *)data;
	if (period != 0)
		return 0;

	return error(_("period cannot be 0"));
}

static size_t find_savent_collision(struct dotsav *ctx, const char *val)
{
	size_t i;
	for_each_idx(i, ctx->savnl) {
		if (strcmp(ctx->savarr[i].name, val))
			continue;
		break;
	}

	return i;
}

static void append_savesave(struct dotsav *ctx)
{
	CAP_ALLOC(&ctx->savarr, ctx->savnl + 1, &ctx->savcap);

	memset(&ctx->savarr[ctx->savnl++], 0, sizeof(*ctx->savarr));
}

static int apply_savent(struct savent *ent,
			struct savesave *sav, const char *val)
{
	int ret = 0;
	intptr_t addr = (intptr_t)sav + ent->offset;

	/*
	 * damn, I made this shit too flexible
	 */
	switch (ent->type) {
	case SAVENT_ENTSEP:
		if (strnxtws(val))
			goto err_inv_name;
	case SAVENT_STRING:
		char *cp = xstrdup(val);
		memcpy((void *)addr, &cp, sizeof(cp));
		break;
	case SAVENT_TIMESPAN:
		ret = str2timespan(val, (u32 *)addr);
		break;
	case SAVENT_UINT8:
		ret = str2u8(val, (u8 *)addr);
		break;
	case SAVENT_FLAG:
		ret = str2bool(val, (int *)addr);
		break;
	}

	if (!ret && ent->cb)
		ret = ent->cb(sav, (void *)addr);
	return ret;

err_inv_name:
	return error(_("whitespace is not allowed in savent name"));
}

static int parse_line(struct dotsav *ctx)
{
	const char *line = ctx->line;
	if (*line == 0)
		return 0;

	line = strskipws(line);
	if (*line == '#' || *line == 0)
		return 0;

	struct savent *ent = ctx->ent;
	for_each_savent(ent) {
		if (!ent->name)
			continue;

		if (strskip(line, ent->name, &line))
			continue;

		char *val = strskipws(line);
		if (val == line)
			continue;

		val = strtrimend(val);
		if (!*val)
			return error(_("sav entry is missing a value"));

		if (ent->type == SAVENT_ENTSEP) {
			size_t idx = find_savent_collision(ctx, val);
			if (idx != ctx->savnl)
				return error(_("name `%s' has collisions at indexes %zu and %zu"),
					     val, idx, ctx->savnl);

			append_savesave(ctx);
		}

		return apply_savent(ent, &ctx->savarr[ctx->savnl - 1], val);
	}

	return error(_("sav entry contains an unrecognized key"));
}

static void check_valid_savent(struct savesave *sav)
{
	struct strbuf sb = STRBUF_INIT;

	if (!sav->save_prefix)
		strbuf_concat(&sb, "\tsave\n");
	if (!sav->backup_prefix)
		strbuf_concat(&sb, "\tbackup\n");
	if (!sav->period)
		strbuf_concat(&sb, "\tperiod\n");
	if (!sav->stack)
		strbuf_concat(&sb, "\tstack\n");

	if (!sb.cap)
		return;

	size_t lines = strbuf_cntchr(&sb, '\n');
	die(lines > 1 ?
	    _("savent `%s' missing the following fields\n\n%s") :
	    _("savent `%s' missing the following field\n\n%s"),
	    sav->name, sb.str);
}

static void update_backup_prefix(struct savesave *sav)
{
	struct strbuf sb = STRBUF_INIT;

	strbuf_concat_path(&sb, sav->backup_prefix, sav->name);
	if (sav->use_compress)
		strbuf_printf(&sb, ".%s.", CONFIG_ARCHIVE_EXTENTION);
	else
		strbuf_concat(&sb, ".");

	free(sav->backup_prefix);
	sav->backup_prefix = sb.str;
}

static void finalize_parsing(struct savesave *sav)
{
	for_each_sav(sav) {
		check_valid_savent(sav);

		if (sav->use_compress == -1 &&
		    sav->is_dir_save &&
		    sav->save_size > CONFIG_COMPRESSING_THRESHOLD)
			sav->use_compress = 1;

		if (sav->use_snapshot == -1 &&
		    sav->save_size > CONFIG_SNAPSHOT_THRESHOLD)
			sav->use_snapshot = 1;

		update_backup_prefix(sav);
	}
}

static void do_parse(struct dotsav *ctx)
{
	int err;
	char *next;
	uint cnt = 0;

	do {
		cnt++;
		next = strchrnul(ctx->line, '\n');
		if (likely(*next))
			*next = 0;
		else
			next = NULL;

		err = parse_line(ctx);
		if (err)
			die(_("failed to parse dotsav\n"
			    "%7u:%s"), cnt, ctx->line);

		if (unlikely(!next))
			break;
		ctx->line = next + 1;
	} while (39);

	CAP_ALLOC(&ctx->savarr, ctx->savnl + 1, &ctx->savcap);
	memset(&ctx->savarr[ctx->savnl], 0, sizeof(*ctx->savarr));

	finalize_parsing(ctx->savarr);
}

size_t dotsav_parse(char *savstr, struct savesave **sav)
{
	struct savent ent[] = {
		SE_ENTSEP("config", name, NULL),

		SE_STRING("save", save_prefix, interpret_save),
		SE_STRING("backup", backup_prefix, NULL),

		SE_FLAG("snapshot", use_snapshot, NULL),
		SE_FLAG("compress", use_compress, NULL),

		SE_TIMESPAN("period", period, check_period),
		SE_UINT8("stack", stack, check_stack),

		SE_END(),
	};

	struct dotsav ctx = {
		.line = savstr,
		.ent  = ent,
	};

	do_parse(&ctx);

	*sav = ctx.savarr;
	return ctx.savnl;
}

void dotsav_print(struct savesave *sav)
{
	for_each_sav(sav) {
		ssize_t size = sav->save_size / 1000 / 1000;

		printf("%s\n", sav->name);
		printf("	save	 %s\n", sav->save_prefix);
		printf("	backup	 %s\n", sav->backup_prefix);
		putchar('\n');

		printf("	size	 %zdM\n", size);
		printf("	dirsave	 %d\n", sav->is_dir_save);
		putchar('\n');

		printf("	compress %d\n", sav->use_compress);
		printf("	snapshot %d\n", sav->use_snapshot);
		putchar('\n');

		printf("	period	 %" PRIu32 "\n", sav->period);
		printf("	stack	 %" PRIu8 "\n", sav->stack);
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
