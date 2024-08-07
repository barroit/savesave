// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "alloc.h"
#include "list.h"

struct argarr {
	char **buf;
	size_t nr;
	size_t cap;
};

int cmdline2argv(const char *cmdline, char ***argv)
{
	std::string line = std::string(APPNAME) + " " + std::string(cmdline);
	std::istringstream stream(std::move(line));
	std::string arg;

	struct argarr args = { 0 };
	while (stream >> std::quoted(arg)) {
		char *buf = xstrdup(arg.c_str());

		CAP_GROW(&args.buf, args.nr + 1, &args.cap);
		args.buf[args.nr++] = buf;
	}

	CAP_GROW(&args.buf, args.nr + 1, &args.cap);
	args.buf[args.nr] = NULL;

	*argv = args.buf;
	return args.nr;
}
