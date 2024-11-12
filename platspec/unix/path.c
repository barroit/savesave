// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "path.h"
#include "termas.h"
#include "alloc.h"
#include "strbuf.h"

int path_is_abs(const char *path)
{
	return *path == '/';
}

static char *get_homedir_of(const char *user)
{
	struct passwd *pw = getpwnam(user);

	if (pw)
		return pw->pw_dir;

	warn_errno(_("can't get home directory for sudo user `%s'"), user);
	return NULL;
}

const char *home_dir(void)
{
	const char *home;
	const char *user = getenv("SUDO_USER");

	if (user)
		home = get_homedir_of(user);
	else
		home = getenv("HOME");

	if (!home)
		die(_("your $HOME corrupted"));

	return home;
}

const char *tmp_dir(void)
{
	if (getuid() == geteuid() && getgid() == getegid()) {
		const char *path = getenv("TMPDIR");
		if (path)
			return path;
	}

#ifdef P_tmpdir
	return P_tmpdir;
#else
	return "/tmp";
#endif
}

int readlink_nt(const char *name, char **target)
{
	struct stat st;
	int err = lstat(name, &st);

	if (err)
		return warn_errno(ERRMAS_STAT_FILE(name));

	off_t size = st.st_size ? : fix_grow(64);

	ssize_t nr;
	char *buf = xmalloc(size);

retry:
	nr = readlink(name, buf, size);

	if (nr == size) {
		free(buf);
		size = fix_grow(size);
		buf = xmalloc(size);
		goto retry;
	}

	if (nr == -1) {
		free(buf);
		return warn_errno(ERRMAS_READ_LINK(name));
	}

	buf[nr] = 0;
	*target = buf;
	return 0;
}
