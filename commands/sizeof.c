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
#include "mkdir.h"
#include "maincmd.h"
#include "dotsav.h"
#include "strlist.h"

int cmd_sizeof(int argc, const char **argv)
CMDDESCRIP("Calculate file size of given path")
{
	int from_dotsav = 0;

	struct arguopt option[] = {
		APOPT_COUNTUP('s', "from-dotsav", &from_dotsav,
			      N_("read path from dotsav")),
		APOPT_END(),
	};
	const char *usage[] = {
		"sizeof <pathspec>...",
		"sizeof --from-dotsav [--] [<pathspec>...]",
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

		off_t size = st.st_size;
		if (S_ISDIR(st.st_mode)) {
			err = calc_dir_size(path, &size);
			if (err)
				continue;
		} else if (!size) {
			goto err_lacks_size;
		}

		if (size < SZ_1K) {
			printf("%s %ld B\n", path, size);
			continue;
		}

		uint umi = 0;
		off_t scale = 1;

		while (size / (scale * SZ_1K) != 0) {
			scale *= SZ_1K;
			umi++;
		}

		printf("%s %ld.%03ld %s\n", path, size / scale,
		       ((size % scale) * 1000) / scale, unit_map[umi]);
		continue;

		if (0) {
		err_stat_file:
			error_errno(ERRMAS_STAT_FILE(path));
		} else if (0) {
		err_lacks_size:
			error(_("file `%s' lacks a size information"), path);
		}
	}

	strlist_destroy(&sl);
	return 0;
}
