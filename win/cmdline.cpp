// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/alloc.hpp"
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
		size_t nr = arg.size() + 1;
		char *buf = xnew<char>(nr);
		memcpy(buf, arg.c_str(), nr);

		CAP_GROW(&args.buf, args.nr + 1, &args.cap);
		args.buf[args.nr++] = buf;
	}

	CAP_GROW(&args.buf, args.nr + 1, &args.cap);
	args.buf[args.nr] = NULL;

	*argv = args.buf;
	return args.nr;
}

void rmargv(int argc, char **argv)
{
	int i;
	for_each_idx(i, argc)
		delete[] argv[i];

	free(argv);
}
