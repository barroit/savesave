// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "backup.h"
#include "dotsav.h"
#include "strbuf.h"
#include "list.h"
#include "termas.h"
#include "fileiter.h"
#include "mkdir.h"
#include "alloc.h"
#include "constructor.h"

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

#define SRT_NOROOM -2

/**
 * sort_backup - sort backup files
 *
 * return: -1 on error, SRT_NOROOM if there is no room, 0 on success
 */
static int backup_fetch_sort(struct strbuf *src,
			     struct strbuf *dest, u8 stack, u8 *nsrt)
{
	int err;
	u8 i;
	int room = -1;
	u8 base;

	for_each_idx(i, stack) {
		strbuf_concatat_base(src, stru8_map[i]);

		err = access(src->str, F_OK);
		if (!err) {
			if (room == -1)
				continue;

			strbuf_concatat_base(dest, stru8_map[room]);
			err = rename(src->str, dest->str);
			if (err)
				goto err_rename_file;
			room++;
		} else if (errno != ENOENT) {
			goto err_access_file;
		} else if (room == -1) {
			room = i;
			base = i;
		}
	}

	if (nsrt)
		*nsrt = room != -1 ? room - base : 0;

	return room == -1 ? SRT_NOROOM : 0;

err_rename_file:
	return warn_errno(ERRMAS_RENAME_FILE(src->str, dest->str));
err_access_file:
	return warn_errno(ERRMAS_ACCESS_FILE(src->str));
}

static int find_next_room(struct strbuf *next, u8 stack)
{
	int err;
	u8 i;

	for_each_idx_back(i, stack - 1) {
		strbuf_concatat_base(next, stru8_map[i]);

		err = access(next->str, F_OK);
		if (!err) {
			strbuf_concatat_base(next, stru8_map[i + 1]);
			return 0;
		} else if (errno != ENOENT) {
			return warn_errno(ERRMAS_ACCESS_FILE(next->str));
		}
	}

	strbuf_concatat_base(next, stru8_map[0]);
	return 0;
}

static int next_backup_prefix(const struct savesave *sav, char **pref)
{
	int ret;
	struct strbuf src = strbuf_from2(sav->backup_prefix, 0, STRU8_MAX - 1);
	struct strbuf dest = strbuf_from2(sav->backup_prefix, 0,
					  STRU8_MAX - 1);

	ret = backup_fetch_sort(&src, &dest, sav->stack, NULL);
	if (ret == -1)
		goto err_sort_backup;

	if (ret == SRT_NOROOM) {
		strbuf_concatat_base(&dest, "0");
		ret = flexremove(dest.str);
		if (ret)
			goto err_drop_backup;

		ret = backup_fetch_sort(&src, &dest, sav->stack, NULL);
		if (ret == -1)
			goto err_sort_backup;
	}

	ret = find_next_room(&dest, sav->stack);
	if (ret)
		goto err_find_next;

	if (0) {
	err_drop_backup:
		error(_("unable to drop deprecated backup of configuration `%s'"),
		      sav->name);
	} else if (0) {
	err_sort_backup:
		strbuf_concatat_base(&src, "*");
		error(_("unable to sort backup `%s' of configuration `%s'"),
		      src.str, sav->name);
	} else if (0) {
	err_find_next:
		error(_("unable to determine next backup file name of configuration `%s'"),
		      sav->name);
	}

	strbuf_destroy(&src);

	if (ret) {
		strbuf_destroy(&dest);
		return -1;
	}

	*pref = dest.str;
	return 0;
}

static void tmpdir_of_backup(const char *backup, char **name)
{
	struct strbuf sb = strbuf_from2(backup, 0,
					strlen(CONFIG_TMPFILE_EXTENTION));

	strbuf_concat(&sb, CONFIG_TMPFILE_EXTENTION);
	*name = sb.str;
}

int backup(struct savesave *c)
{
	int ret;
	char *dest;
	char *tmp;

	ret = next_backup_prefix(c, &dest);
	if (ret)
		return ret;

	tmpdir_of_backup(c->backup_prefix, &tmp);

	// ret = start_backup(dest, tmp, c->save_prefix, c->use_compress);

	free(dest);
	free(tmp);
	return ret;
}
