// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * Strategy inspired by btrfs/zstd.c
 */

#include "compress.h"
#include "constructor.h"
#include "termas.h"

USESTDIO
CONSTRUCTOR(check_libzstd_compat)
{
	int ret;

	ZSTD_CCtx *ctx = ZSTD_createCCtx();
	BUG_ON(!ctx);

	ret = ZSTD_CCtx_setParameter(ctx, ZSTD_c_nbWorkers, 400);
	if (ZSTD_isError(ret))
		die(_("savesave requires a multithreading supported version of libzstd"));

	ZSTD_freeCCtx(ctx);
}

int make_zip(const char *dest, const char *src, int is_dir)
{
	return 0;
}
