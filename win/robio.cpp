// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "barroit/io.h"
#include "termsg.h"

int mk_file_dir(const char *name)
{
	std::filesystem::path abs = name;
	std::filesystem::path dir = abs.parent_path();
	std::string tmp;
	const char *str;
	try {
		dir = abs.parent_path();
		tmp = dir.string();
		str = tmp.c_str();

		if (std::filesystem::is_directory(dir)) {
			return 0;
		} else if (std::filesystem::exists(dir)) {
			warn("file ‘%s’ is not a directory", str);
			errno = EEXIST;
			return 1;
		}
	} catch (...) {
		die("exception occurred while resolving directory for ‘%s’",
		    str);
	}

	if (strcmp(str, ".") && strcmp(str + 1, ":\\"))
		return mkdir(str);
	else
		return 0;
}

int is_abs_path(const char *path)
{
	std::filesystem::path p = path;
	return p.is_absolute();
}
