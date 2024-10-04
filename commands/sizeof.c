// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * sizeof - get file size (not the size it takes on disk, just the
 *	    byte it has)
 */

#include "argupar.h"
#include "termas.h"
#include "list.h"
#include "maincmd.h"
#include "dotsav.h"
#include "strlist.h"
#include "fileiter.h"

struct dirinfo {
	ullong size;
	uint files;
	uint dirs;
};

int iterfunc(struct iterfile *file, void *data)
{
	struct dirinfo *info = data;

	if (!file->is_dir) {
		info->size += file->st->st_size ? : 64;
		info->files += 1;
	} else {
		info->dirs += 1;
	}

	return 0;
}

int sizeof_directory(const char *name, struct dirinfo *info)
{
	int ret;
	struct fileiter ctx;

	fileiter_init(&ctx, iterfunc, info,
		      FITER_LIST_DIR | FITER_USE_STAT | FITER_LIST_UNSUP);

	ret = fileiter_exec(&ctx, name);
	fileiter_destroy(&ctx);

	return ret;
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

	argupar_t ap;
	argupar_init(&ap, argc, argv);

	argc = argupar_parse(&ap, option, usage, AP_STOPAT_NONOPT);

	struct savesave *savarr;
	size_t savnl = 0;

	if (from_dotsav)
		savnl = retrieve_dotsav(&savarr);
	else if (!argc)
		die(_("sizeof requires pathspec"));

	size_t i;
	const char *unit_map[] = { "B", "KiB", "MiB", "GiB", "TiB" };
	struct strlist sl;

	strlist_init(&sl, STRLIST_USEREF);
	if (savnl)
		strlist_join_member(&sl, savarr, savnl, save_prefix);
	strlist_join_argv(&sl, argv);

	for_each_idx(i, sl.nl) {
		int err;
		const char *path = sl.list[i].str;

		struct stat st;
		err = stat(path, &st);
		if (err)
			goto err_stat_file;

		struct dirinfo info = {
			.size  = st.st_size,
			.files = 1,
		};

		if (S_ISDIR(st.st_mode)) {
			err = sizeof_directory(path, &info);
			if (err)
				goto err_calc_dir;
		} else if (!info.size) {
			goto err_lacks_size;
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
		continue;

		if (0) {
		err_stat_file:
			error_errno(ERRMAS_STAT_FILE(path));
		} else if (0) {
		err_calc_dir:
			error(_("cannot calculate size for directory `%s'"),
			      path);
		} else if (0) {
		err_lacks_size:
			error(_("file `%s' lacks a size information"), path);
		}
	}

	strlist_destroy(&sl);
	return 0;
}
