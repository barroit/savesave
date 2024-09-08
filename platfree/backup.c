// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "backup.h"
#include "savconf.h"
#include "debug.h"
#include "strbuf.h"
#include "list.h"
#include "termas.h"
#include "fileiter.h"
#include "alloc.h"

#define ERRMAS_SORT_BACKUP \
	"unable to sort backup `%s' of configuration `%s'"

static char stru8_map[UINT8_MAX + 1][STRU8_MAX];

CONSTRUCTOR(init_u8tstr_table)
{
	size_t i;
	int n;

	for_each_idx(i, UINT8_MAX + 1) {
		n = snprintf(stru8_map[i], sizeof(*stru8_map), "%u", (u8)i);
		BUG_ON(n < 0);
	}
		
}

static int sort_backup(struct strbuf *src, struct strbuf *dest, u8 stack)
{
	int err;
	u8 i;
	int room = -1;

	for_each_idx(i, stack) {
		strbuf_concatat(src, src->initlen, stru8_map[i]);

		err = access(src->str, F_OK);
		if (!err) {
			if (room == -1)
				continue;

			strbuf_concatat(dest, src->initlen, stru8_map[room]);
			err = rename(src->str, dest->str);
			if (err)
				return warn_errno(
_("failed to rename file `%s' to `%s'"), src->str, dest->str);
			room++;
		} else if (errno != ENOENT) {
			return warn_errno(_(ERRMAS_ACCESS_FILE), src->str);
		}

		if (room == -1)
			room = i;
	}

	return room != -1;
}

static int drop_deprecated_backup(struct strbuf *path)
{
	int err;

	strbuf_concatat(path, path->initlen, "0");
	err = remove(path->str);
	if (err)
		return warn_errno(_("failed to remove file `%s'"), path->str);

	return 0;
}

static int find_next_room(struct strbuf *next, u8 stack)
{
	int err;
	u8 i;

	for_each_idx_back(i, stack - 1) {
		strbuf_concatat(next, next->initlen, stru8_map[i]);

		err = access(next->str, F_OK);
		if (!err) {
			strbuf_concatat(next, next->initlen, stru8_map[i + 1]);
			return 0;
		} else if (errno != ENOENT) {
			return warn_errno(_(ERRMAS_ACCESS_FILE), next->str);
		}
	}

	BUG_ON(1);
}

static char *get_next_backup_name(const struct savesave *c)
{
	int ret;
	int has_room;
	struct strbuf src = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);
	struct strbuf dest = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);

	has_room = sort_backup(&src, &dest, c->stack);
	if (has_room == -1) {
		strbuf_concatat(&src, src.initlen, "*");
		ret = error(_(ERRMAS_SORT_BACKUP), src.str, c->name);
		goto cleanup;
	}

	if (!has_room) {
		ret = drop_deprecated_backup(&dest);
		if (ret) {
			error(
_("unable to drop deprecated backup of configuration `%s'"), c->name);
			goto cleanup;
		}

		has_room = sort_backup(&src, &dest, c->stack);
		if (has_room == -1) {
			strbuf_concatat(&src, src.initlen, "*");
			ret = error(_(ERRMAS_SORT_BACKUP), src.str, c->name);
			goto cleanup;
		}
	}

	ret = find_next_room(&dest, c->stack);
	if (ret)
		error(
_("unable to determine next backup file name of configuration `%s'"), c->name);

cleanup:
	strbuf_destroy(&src);

	if (ret) {
		strbuf_destroy(&dest);
		return NULL;
	}

	return dest.str;
}

int copy_file(const char *src, int fd1, struct stat *st, const char *dest);

static FILE_ITER_CALLBACK(backup_file_compress)
{
	const struct savesave *c = data;

	return 0;
}

static FILE_ITER_CALLBACK(backup_file_copy)
{
	const struct savesave *c = data;

	// puts(src->absname);
	// copy_file(src, fd, st, c->backup);
	return 0;
}

static int do_backup(struct savesave *c)
{
	int ret;
	struct file_iter ctx;
	if (c->use_compress)
		file_iter_init(&ctx, c->save_prefix, backup_file_compress, c);
	else
		file_iter_init(&ctx, c->save_prefix, backup_file_copy, c);

	ret = file_iter_exec(&ctx);
	file_iter_destroy(&ctx);

	return ret;
}

static char *tmpdir_of_backup(const char *backup)
{
	struct strbuf sb = strbuf_from2(backup, 0,
					sizeof(CONFIG_TEMPORARY_EXTENTION));

	strbuf_concat(&sb, CONFIG_TEMPORARY_EXTENTION);
	return sb.str;
}

int backup_routine(struct savesave *c)
{
	int ret;

	char *dest = get_next_backup_name(c);
	if (!dest)
		return 1;

	char *temp = tmpdir_of_backup(c->backup_prefix);

	DEBUG_RUN() {
		printf(_("next backup name\n\t%s\n"), dest);
		fflush(stdout);
	}

	if (c->use_snapshot) {
		/* create snapshot here */
	}

	ret = do_backup(c);

	free(dest);
	free(temp);
	return ret;
}
