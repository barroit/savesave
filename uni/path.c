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

static char *executable_dirname;

CONSTRUCTOR(populate_executable_dir)
{
	char exe[PATH_MAX];
	ssize_t nr = readlink("/proc/self/exe", exe, sizeof(exe) - 1);

	BUG_ON(nr == -1);
	exe[nr] = 0;

	char *dir = dirname(exe);

	executable_dirname = xstrdup(dir);
}

int is_absolute_path(const char *path)
{
	return *path == '/';
}

static char *get_user_home_dir(const char *user)
{
	struct passwd *pw = getpwnam(user);

	if (pw)
		return pw->pw_dir;

	warn_errno(_("can't get home directory for sudo user `%s'"), user);
	return NULL;
}

const char *get_home_dir(void)
{
	const char *home;
	const char *user = getenv("SUDO_USER");

	if (user)
		home = get_user_home_dir(user);
	else
		home = getenv("HOME");

	if (!home)
		die("your $HOME corrupted");

	return home;
}

const char *get_executable_dirname(void)
{
	return executable_dirname;
}
