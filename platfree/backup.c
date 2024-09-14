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
#include "mkdir.h"
#include "alloc.h"

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

/**
 * sort_backup - sort backup files
 *
 * return: -1 on error, 0 on success, 1 if there is no room
 */
static int sort_backup(struct strbuf *src,
		       struct strbuf *dest, u8 stack, int *n)
{
	int err;
	u8 i;
	int room = -1, base;

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
		}

		if (room == -1) {
			room = i;
			base = i;
		}
	}

	if (room != -1 && n)
		*n = room - base;

	/*
	 * You may think this is shit, it is!
	 * And hooray for semantics!
	 */
	return room == -1 ? 1 : 0;

err_rename_file:
	return warn_errno(_("failed to rename file `%s' to `%s'"),
			  src->str, dest->str);
err_access_file:
	return warn_errno(_("failed to access file `%s'"), src->str);
}

static int drop_deprecated_backup(struct strbuf *path)
{
	int ret;

	strbuf_concatat_base(path, "0");
	const char *name = path->str;

	ret = unlink(name);
	if (!ret)
		return 0;
	else if (errno == EISDIR)
		ret = rmdirr(path->str);
	else
		goto err_remove_file;

	return ret;

err_remove_file:
	return warn_errno(_("failed to remove file `%s'"), name);
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
			return warn_errno(_("failed to access file `%s'"),
					  next->str);
		}
	}

	strbuf_concatat_base(next, stru8_map[0]);
	return 0;
}

static char *get_next_backup_name(const struct savesave *c)
{
	int ret;
	struct strbuf src = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);
	struct strbuf dest = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);

	ret = sort_backup(&src, &dest, c->stack, NULL);
	if (ret == -1)
		goto err_sort_backup;

	if (ret == 1) {
		ret = drop_deprecated_backup(&dest);
		if (ret)
			goto err_drop_backup;

		ret = sort_backup(&src, &dest, c->stack, NULL);
		if (ret == -1)
			goto err_sort_backup;
	}

	ret = find_next_room(&dest, c->stack);
	if (ret)
		goto err_find_next;

	if (0) {
	err_drop_backup:
		error(_("unable to drop deprecated backup of configuration `%s'"),
		      c->name);
	} else if (0) {
	err_sort_backup:
		strbuf_concatat_base(&src, "*");
		error(_("unable to sort backup `%s' of configuration `%s'"),
		      src.str, c->name);
	} else if (0) {
	err_find_next:
		error(_("unable to determine next backup file name of configuration `%s'"),
		      c->name);
	}

	strbuf_destroy(&src);

	if (ret) {
		strbuf_destroy(&dest);
		return NULL;
	}

	return dest.str;
}

static char *tmpdir_of_backup(const char *backup)
{
	struct strbuf sb = strbuf_from2(backup, 0,
					sizeof(CONFIG_TMPFILE_EXTENTION));

	strbuf_concat(&sb, CONFIG_TMPFILE_EXTENTION);
	return sb.str;
}

static int backup_file_compress(struct fileiter_file *src, void *data)
{
	return *(int *)ACCESS_POISON;
}

static int backup_file_copy(struct fileiter_file *src, void *data)
{
	struct strbuf *dest = &((struct strbuf *)data)[0];
	struct strbuf *__buf = &((struct strbuf *)data)[1];

	strbuf_concatat_base(dest, src->dymname);

	if (src->is_lnk)
		return PLATSPECOF(backup_copy_symlink)(src, dest, __buf);
	else
		return PLATSPECOF(backup_copy_regfile)(src, dest);
}

static int do_backup(const char *dest, const char *temp,
		     const char *save, int use_compress)
{
	int ret;
	fileiter_callback_t cb;

	if (use_compress) {
		cb = backup_file_compress;
	} else {
		cb = backup_file_copy;

		char *d = xstrdup(dest);
		ret = mkdirp(d);
		free(d);
		if (ret)
			goto err_make_dir;
	}

	struct fileiter iter;
	struct strbuf data[2] = {
		[0] = strbuf_from2(dest, 0, PATH_MAX),
		[1] = strbuf_from2("", 0, PATH_MAX),
	};

	fileiter_init(&iter, save, cb, data, FI_USE_STAT | FI_USE_FD);
	ret = fileiter_exec(&iter);
	if (ret)
		error(_("failed to backup save `%s' to `%s'"), save, dest);

	fileiter_destroy(&iter);
	strbuf_destroy(&data[0]);
	strbuf_destroy(&data[1]);
	return ret;

err_make_dir:
	return error_errno(_("failed to make directory `%s'"), dest);
}

int backup(struct savesave *c)
{
	int ret;

	char *dest = get_next_backup_name(c);
	if (!dest)
		return 1;

	char *temp = tmpdir_of_backup(c->backup_prefix);

	DEBUG_RUN() {
		printf(_("next backup name\n\t%s\n\n"), dest);
		fflush(stdout);
	}

	if (c->use_snapshot) {
		/* create snapshot here */
	}

	ret = do_backup(dest, temp, c->save_prefix, c->use_compress);

	free(dest);
	free(temp);
	return ret;
}
