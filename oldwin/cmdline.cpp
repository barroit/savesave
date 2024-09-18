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
	std::string line = std::string(SAVESAVE_NAME) + " " +
			   std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string opt;
	struct strlist sl = STRLIST_INIT;

	while (stream >> std::quoted(opt))
		strlist_push(&sl, opt.c_str());

	int argc = (int)sl.nl;
	*argv = strlist_dump2arr(&sl);
	strlist_destroy(&sl);

	return argc;
}
