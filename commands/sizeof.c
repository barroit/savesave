// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * sizeof - get file size (not the size it takes on disk, just the
 *	    byte it has)
 */

#include "argupar.h"
#include "termas.h"
#include "iter.h"
#include "dotsav.h"
#include "strlist.h"
#include "fileiter.h"

struct dirinfo {
	ullong size;
	uint files;
	uint dirs;
};

static int accumsize(struct iterfile *file, void *data)
{
	struct dirinfo *info = data;

	if (S_ISDIR(file->st.st_mode)) {
		info->dirs += 1;
	} else {
		info->size += file->st.st_size ? : 64;
		info->files += 1;
	}

	return 0;
}

int cmd_sizeof(int argc, const char **argv)
CMDDESCRIP("Calculate file size of given path")
{
	int from_dotsav = 0;
	int count_dir = 1;
	int count_file = 1;

	struct arguopt option[] = {
		APOPT_SWITCH(0, "no-dir", &count_dir,
			     N_("do not count directories")),
		APOPT_SWITCH(0, "no-file", &count_file,
			     N_("do not count files")),
		APOPT_COUNTUP('s', "from-dotsav", &from_dotsav,
			      N_("read path from dotsav")),
		APOPT_END(),
	};
	const char *usage[] = {
		"sizeof [<options>] <pathspec>...",
		"sizeof [<options>] --from-dotsav [--] [<pathspec>...]",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_STOPAT_NONOPT);

	if (from_dotsav)
		dotsav_prepare();
	else if (!argc)
		die(_("sizeof requires pathspec"));

	size_t i;
	const char *unit_map[] = { "B", "KiB", "MiB", "GiB", "TiB" };
	struct strlist sl;

	strlist_init(&sl, STRLIST_STORE_REF);
	if (dotsav_size)
		strlist_join_member(&sl, dotsav_array,
				    dotsav_size, save_prefix);
	strlist_join_argv(&sl, argv);

	for_each_idx(i, sl.size) {
		int err;
		const char *path = strlist_at(&sl, i);
		struct stat st;

		err = stat(path, &st);
		if (err) {
			error_errno(ERRMAS_STAT_FILE(path));
			continue;
		}

		struct dirinfo info = {
			.size  = st.st_size,
			.files = 1,
		};

		if (S_ISDIR(st.st_mode)) {
			err = fileiter(path, accumsize, &info,
				       FITER_LIST_DIR | FITER_USE_STAT);
			if (err) {
				error(_("cannot calculate size for directory `%s'"),
				      path);
				continue;
			}
		} else if (!info.size) {
			error(_("file `%s' lacks a size information"), path);
			continue;
		}

		uint uidx = 0;
		ullong scale = 1;

		while (info.size / (scale * SZ_1K) != 0) {
			scale *= SZ_1K;
			uidx++;
		}

		uint integer = info.size / scale;
		uint fraction = ((info.size % scale) * 1000) / scale;
		const char *unit = unit_map[uidx];

		int indent = 14;
		int width = printf("%u", integer);

		if (fraction)
			width += printf(".%03u", fraction);

		width += printf(" %s", unit);

		if (count_file) {
			width += printf("%*s%u", indent - width, "",
					info.files);
			indent += 7;
		}

		if (count_dir) {
			width += printf("%*s%u", indent - width, "",
					 info.dirs);
			indent += 6;
		}

		printf("%*s%s\n", indent - width, "", path);
	}

	exit(0);
}
