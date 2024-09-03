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
#include "termsg.h"
#include "barroit/limits.h"
#include "robio.h"
#include "alloc.h"

static char stru8_map[UINT8_MAX + 1][STRU8_MAX];

CONSTRUCTOR init_u8tstr_table(void)
{
	size_t i;
	int n;

	for_each_idx(i, UINT8_MAX + 1) {
		n = snprintf(stru8_map[i], sizeof(*stru8_map), "%u", (u8)i);
		BUG_ON(n < 0);
	}
		
}

static int move_backup(const char *src, const char *dest)
{
	int err;

	err = rename(src, dest);
	if (err)
		return warn_errno("failed to rename ‘%s’ to ‘%s’", src, dest);
	return 0;
}

static int sort_backup(char *src, char *srcend, u8 stack, int *has_room)
{
	int err;

	u8 i;
	int room = -1;

	char dest[PATH_MAX];
	size_t len = srcend - src;
	char *destend = dest + len;

	memcpy(dest, src, len);
	for_each_idx(i, stack) {
		memcpy(srcend, stru8_map[i], sizeof(*stru8_map));

		err = access(src, F_OK);
		if (!err) {
			if (room == -1)
				continue;

			memcpy(destend, stru8_map[room], sizeof(*stru8_map));
			err = move_backup(src, dest);
			if (err)
				return 1;
			room++;
		} else if (errno != ENOENT) {
			return warn_errno(ERR_ACCESS_FILE, src);
		}

		if (room == -1)
			room = i;
	}

	if (has_room)
		*has_room = room != -1;
	return 0;
}

static int drop_deprecated_backup(char *str, char *strend, u8 stack)
{
	int err;

	strcpy(strend, "0");
	err = remove(str);
	if (err)
		return warn_errno("failed to remove ‘%s’", str);
	return 0;
}

static int find_next_room(char *str, char *strend, u8 stack)
{
	int err;
	u8 i;
	for_each_idx_back(i, stack - 1) {
		memcpy(strend, stru8_map[i], sizeof(*stru8_map));

		err = access(str, F_OK);
		if (!err) {
			BUG_ON(i == stack - 1);

			memcpy(strend, stru8_map[i + 1], sizeof(*stru8_map));
			return 0;
		} else if (errno != ENOENT) {
			return warn_errno(ERR_ACCESS_FILE, str);
		}
	}

	BUG_ON(1);
}

static int request_next_room(const struct savesave *c, char *path)
{
	int err;
	char *pathend = path + c->backup_len;
	int has_room;

	memcpy(path, c->backup, c->backup_len);
	err = sort_backup(path, pathend, c->stack, &has_room);
	if (err)
		goto err_sort_backup;

	if (!has_room) {
		err = drop_deprecated_backup(path, pathend, c->stack);
		if (err)
			return error("unable to drop deprecated backup of configuration ‘%s’",
				     c->name);
		err = sort_backup(path, pathend, c->stack, NULL);
		if (err)
			goto err_sort_backup;
	}

	err = find_next_room(path, pathend, c->stack);
	if (err)
		return error("unable to find next backup file name of configuration ‘%s’",
			     c->name);

	return 0;

err_sort_backup:
	*pathend = '*';
	return error("unable to sort backup ‘%s’ of configuration ‘%s’",
		     path, c->name);
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
		file_iter_init(&ctx, c->save, backup_file_compress, c);
	else
		file_iter_init(&ctx, c->save, backup_file_copy, c);

	ret = file_iter_exec(&ctx);
	file_iter_destroy(&ctx);

	return ret;
}

static char *tmpdir_of_backup(const char *backup, size_t len)
{
	size_t ext = sizeof(CONFIG_TEMPORARY_EXTENTION);
	char *ret = xmalloc(len + ext);

	memcpy(ret, backup, len);
	memcpy(&ret[len], CONFIG_TEMPORARY_EXTENTION, ext);

	return ret;
}

int backup_routine(struct savesave *c)
{
	int ret;

	char dest[PATH_MAX];
	ret = request_next_room(c, dest);
	if (ret)
		return ret;

	char *temp = tmpdir_of_backup(c->backup, c->backup_len);

	DEBUG_RUN() {
		printf("next backup name\n\t%s\n", dest);
		fflush(stdout);
	}

	if (c->use_snapshot) {
		/* create snapshot here */
	}

	ret = do_backup(c);

	free(temp);
	return ret;
}
