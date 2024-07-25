// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "getconf.h"
#include "termsg.h"
#include "alloc.h"

static char *get_sudo_user_dir(void)
{
	const char *user = getenv("SUDO_USER");
	if (!user)
		return NULL;

	struct passwd *pw = getpwnam(user);
	if (!pw) {
		warn_errno("can’t get home for ‘%s’ who invoked sudo", user);
		return NULL;
	}

	return pw->pw_dir;
}

const char *get_home_dir(void)
{
	const char *home = getenv("HOME");
	if (!home)
		home = get_sudo_user_dir();
	return home;
}
