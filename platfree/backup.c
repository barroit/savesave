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

struct backup_callback_ctx {
	struct savesave *conf;
	struct strbuf name;
};
	
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

		if (room == -1)
			room = i;
	}

	return room != -1;

err_rename_file:
	return warn_errno(_("failed to rename file `%s' to `%s'"),
			  src->str, dest->str);
err_access_file:
	return warn_errno(_(ERRMAS_ACCESS_FILE), src->str);
}

static int drop_deprecated_backup(struct strbuf *path)
{
	int err;

	strbuf_concatat_base(path, "0");
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
		strbuf_concatat_base(next, stru8_map[i]);

		err = access(next->str, F_OK);
		if (!err) {
			strbuf_concatat_base(next, stru8_map[i + 1]);
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
	struct strbuf src = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);
	struct strbuf dest = strbuf_from2(c->backup_prefix, 0, STRU8_MAX);

	ret = sort_backup(&src, &dest, c->stack);
	if (ret == -1)
		goto err_sort_backup;

	if (!ret) {
		ret = drop_deprecated_backup(&dest);
		if (ret)
			goto err_drop_backup;

		ret = sort_backup(&src, &dest, c->stack);
		if (ret == -1)
			goto err_sort_backup;
	}

	ret = find_next_room(&dest, c->stack);
	if (ret)
		goto err_find_next;

	ERROR_HANDLE(err_drop_backup, {
		error(_("unable to drop deprecated backup of configuration `%s'"),
		      c->name);
	});

	ERROR_HANDLE(err_sort_backup, {
		strbuf_concatat_base(&src, "*");
		error(_("unable to sort backup `%s' of configuration `%s'"),
		      src.str, c->name);
	});

	ERROR_HANDLE(err_find_next, {
		error(_("unable to determine next backup file name of configuration `%s'"),
		      c->name);
	});

	strbuf_destroy(&src);

	if (ret) {
		strbuf_destroy(&dest);
		return NULL;
	}

	return dest.str;
}

static FILEITER_CALLBACK(backup_file_compress)
{
	const struct savesave *c = data;

	return 0;
}

// static void format_backup_name(const char *dirname, )
// {
// }

static FILEITER_CALLBACK(backup_file_copy)
{
	struct backup_callback_ctx *c = data;
	int err;

	// strbuf_concatat_base(&c->name, c->conf);

	// int destfd = creat(destname, 0664);
	// if (destfd == -1)
	// 	return error_errno(_(ERRMAS_CREAT_FILE), destname);
	puts(src->basname);
	// my_mkdir();
	// copyfile(src->relname, src->fd, src->st, c->backup);
	return 0;
}

static int do_backup(struct savesave *c)
{
	int ret;
	struct fileiter iter;
	struct backup_callback_ctx data = {
		.conf = c,
		.name = strbuf_from2(c->backup_prefix, 0, PATH_MAX),
	};

	if (c->use_compress)
		fileiter_init(&iter, c->save_prefix,
			      backup_file_compress, &data);
	else
		fileiter_init(&iter, c->save_prefix, backup_file_copy, &data);

	ret = fileiter_exec(&iter);

	fileiter_destroy(&iter);
	strbuf_destroy(&data.name);
	return ret;
}

static char *tmpdir_of_backup(const char *backup)
{
	struct strbuf sb = strbuf_from2(backup, 0,
					sizeof(CONFIG_TMPFILE_EXTENTION));

	strbuf_concat(&sb, CONFIG_TMPFILE_EXTENTION);
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
