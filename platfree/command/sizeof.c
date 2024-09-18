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

int cmd_sizeof(int argc, const char **argv)
{
	struct arguopt options[] = {
		APOPT_END(),
	};

	argupar_t ap;
	argupar_init(&ap, argc, argv);

	if (!argc)
		error(_("command 'sizeof' requires pathspec"));
	argc = argupar_parse(&ap, options, NULL, AP_EXPECT_ARGS);

	int i;
	const char *unit_map[] = { "B", "KiB", "MiB", "GiB", "TiB" };

	for_each_idx(i, argc) {
		int err;
		const char *path = argv[i];

		struct stat st;
		err = lstat(path, &st);
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

		unsigned umi = 0;
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
			error_errno(_("failed to retrieve information for file `%s'"),
			     path);
		} else if (0) {
		err_lacks_size:
			error(_("file `%s' lacks a size information"), path);
		}
	}

	exit(0);
}
