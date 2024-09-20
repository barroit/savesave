// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "termas.h"
#include "debug.h"
#include "alloc.h"
#include "strbuf.h"

int is_absolute_path(const char *path)
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

const char *get_home_dirname(void)
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

const char *get_executable_dirname(void)
{
	static char *path;

	if (!path) {
		int err;
		struct stat st;

		err = lstat("/proc/self/exe", &st);
		BUG_ON(err);

		size_t cap = st.st_size + 1;
		path = xmalloc(cap);

		ssize_t nr = readlink("/proc/self/exe", path, cap);
		BUG_ON(nr == -1 || nr == cap);

		path[nr] = 0;
	}

	return path;
}

const char *get_tmp_dirname(void)
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
