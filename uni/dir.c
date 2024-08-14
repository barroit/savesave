// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "termsg.h"
#include "strlist.h"
#include "list.h"
#include "alloc.h"

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

static int dirent_sizeof(const char *dir, off_t *size,
			 struct dirent *ent, struct strlist *sl)
{
	if (strcmp(ent->d_name, "..") == 0 ||  strcmp(ent->d_name, ".") == 0)
		return 0;
	if (strcmp(dir, "/") == 0)
		dir = "";

	static char buf[PATH_MAX];
	int nr = snprintf(buf, sizeof(buf), "%s/%s", dir, ent->d_name);
	BUG_ON(nr < 0);

	switch (ent->d_type) {
	case DT_LNK:
		/*
		 * we don’t handle symbolic links; a constant is given that is
		 * large enough for most symbolic link files
		 */
		*size += 64;
		break;
	case DT_REG:
		return file_sizeof(buf, size);
	case DT_DIR:
		strlist_push(sl, buf);
		break;
	case DT_UNKNOWN:
		return error("can’t determine file type of ‘%s’", buf);
	default:
		warn("‘%s’ has unsupported file type ‘%d’; file size retrieval for this file is skipped",
		     buf, ent->d_type);
	}

	return 0;
}

static int do_calc_dir_size(const char *dir, off_t *size, struct strlist *sl)
{
	int err;

	DIR *d = opendir(dir);
	if (!d)
		return error_errno("failed to open dir ‘%s’", dir);

	struct dirent *ent;
	errno = 0;
	while ((ent = readdir(d)) != NULL) {
		err = dirent_sizeof(dir, size, ent, sl);
		if (err) {
			closedir(d);
			return 1;
		}
	}
	BUG_ON(errno != 0);
	closedir(d);

	return 0;
}

int calc_dir_size(const char *dir, off_t *size)
{
	int err;
	struct strlist sl = STRLIST_INIT_DUPE;

	char *path = xstrdup(dir);
	do {
		err = do_calc_dir_size(path, size, &sl);
		free(path);
		if (err)
			goto err_get_size;
	} while ((path = strlist_pop(&sl)) != NULL);

	strlist_destroy_buf(&sl);
	return 0;

err_get_size:
	strlist_destroy(&sl);
	return 1;
}

int make_user_dir(const char *path)
{
	return mkdir(path, 0775);
}
