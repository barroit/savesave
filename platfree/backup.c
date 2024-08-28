// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "savconf.h"
#include "debug.h"
#include "strbuf.h"
#include "list.h"
#include "termsg.h"
#include "barroit/limits.h"

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
			return warn_errno("failed to access ‘%s’", src);
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
			return warn_errno("failed to access ‘%s’", str);
		}
	}

	BUG_ON(1);
}

int backup_routine(const struct savesave *c)
{
	int err;

	if (c->use_snapshot) {
		/* create snapshot here */
	}

	char path[PATH_MAX];
	char *pathend = path + c->backup_len;
	memcpy(path, c->backup, c->backup_len);

	int has_room;
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

	DEBUG_RUN() {
		printf("next backup name\n\t%s\n", path);
		fflush(stdout);
	}

	if (c->use_compress) {
		//
	}

	return 0;

err_sort_backup:
	*pathend = '*';
	return error("unable to sort backup ‘%s’ of configuration ‘%s’",
		     path, c->name);
}
