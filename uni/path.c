// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "path.h"
#include "termas.h"

int is_abs_path(const char *path)
{
	return *path == '/';
}

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
