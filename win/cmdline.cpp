// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "windows.h"
#include "win/cmdline.hpp"
#include "shellapi.h"
#include <iomanip>
#include <sstream>
#include <string.h>
#include "win/alloc.hpp"
#include "alloc.h"

struct arg_array {
	char **buf;
	size_t nr;
	size_t cap;
};

static int cmdline2argv(const char *cmdline, char ***argv, int *argc)
{
	std::istringstream stream{ cmdline };
	std::string arg;

	char *buf;
	size_t nr;
	struct arg_array arr = { 0 };

	while (stream >> std::quoted(arg)) {
		nr = arg.size() + 1;
		buf = xnew<char>(nr);
		memcpy(buf, arg.c_str(), nr);

		CAP_GROW(&arr.buf, arr.nr + 1, &arr.cap);
	}
	return 0;
}

int parse_cmdline(const char *cmdline)
{
	char **argv;
	int argc, err;

	err = cmdline2argv(cmdline, &argv, &argc);
	if (err) {
		goto err_get_argv;
	}

	return 0;

err_get_argv:
	return 1;
}
