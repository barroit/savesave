// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/cmdline.hpp"
#include "strlist.h"

int cmdline2argv(const char *cmdline, char ***argv)
{

	int argc = (int)sl.nl;
	*argv = strlist_dump(&sl);
	strlist_destroy(&sl);

	return argc;
}
