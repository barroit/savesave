// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"
#include "strlist.h"
#include "alloc.h"
#include "debug.h"
#include "strbuf.h"

static char *get_user_home(const char *user)
{
	struct passwd *pw = getpwnam(user);
	if (!pw) {
		warn_errno("can’t get home for ‘%s’ who invoked sudo", user);
		return NULL;
	}

	return pw->pw_dir;
}

const char *get_home_dir(void)
{
	const char *home;
	const char *user = getenv("SUDO_USER");
	if (user)
		home = get_user_home(user);
	else
		home = getenv("HOME");
	if (!home)
		die("your $HOME corrupted");
	return home;
}

static int file_sizeof(const char *f, off_t *size)
{
	int err;
	struct stat st;

	err = stat(f, &st);
	if (err)
		return error_errno("unable to get metadata for file ‘%s’", f);

	*size += st.st_size;
	return 0;
}

static int dirent_sizeof(struct strbuf *sb, off_t *size,
			 struct dirent *ent, struct strlist *sl)
{
	if (strcmp(ent->d_name, "..") == 0 ||  strcmp(ent->d_name, ".") == 0)
		return 0;
	if (strcmp(sb->str, "/") == 0)
		strbuf_zerolen(sb);

	strbuf_concat(sb, "/");
	strbuf_concat(sb, ent->d_name);

	switch (ent->d_type) {
	case DT_REG:
		return file_sizeof(sb->str, size);
	case DT_DIR:
		strlist_push2(sl, sb->str, 32);
		break;
	case DT_LNK:
		/*
		 * we don’t handle symbolic links; a constant is given that is
		 * large enough for most symbolic link files
		 */
		*size += 64;
		break;
	case DT_UNKNOWN:
		return error("can’t determine file type of ‘%s’", sb->str);
	default:
		warn("‘%s’ has unsupported file type ‘%d’; file size retrieval for this file is skipped",
		     sb->str, ent->d_type);
	}

	return 0;
}

static int do_calc_dir_size(struct strbuf *sb, off_t *size, struct strlist *sl)
{
	DIR *d = opendir(sb->str);
	if (!d)
		return error_errno("failed to open dir ‘%s’", sb->str);

	int err;
	struct dirent *ent;
	size_t len = sb->len;

	errno = 0;
	while ((ent = readdir(d)) != NULL) {
		err = dirent_sizeof(sb, size, ent, sl);
		if (err) {
			closedir(d);
			return 1;
		}

		strbuf_truncate(sb, sb->len - len);
	}
	BUG_ON(errno != 0);
	closedir(d);

	return 0;
}

int calc_dir_size(const char *dir, off_t *size)
{
	int ret;
	struct strlist sl;
	struct strbuf sb = STRBUF_INIT;
	const char *path = dir;

	strlist_init(&sl, STRLIST_DUPSTR);
	do {
		strbuf_concat(&sb, path);

		ret = do_calc_dir_size(&sb, size, &sl);
		if (ret)
			break;

		strbuf_zerolen(&sb);
	} while ((path = strlist_pop2(&sl, 0)) != NULL);

	strlist_destroy(&sl);
	strbuf_destroy(&sb);
	return ret;
}
