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

static const char *executable_dirname;

CONSTRUCTOR(setup_executable_dirname)
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
	return executable_dirname;
}
