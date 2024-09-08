// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Strategy inspired by btrfs/zstd.c
 */

#include "compress.h"
#include "termas.h"
#include "debug.h"

CONSTRUCTOR(check_libzstd_mt)
{
	int res;

	ZSTD_CCtx *ctx = ZSTD_createCCtx();
	BUG_ON(!ctx);

	res = ZSTD_CCtx_setParameter(ctx, ZSTD_c_nbWorkers, 400);
	if (ZSTD_isError(res))
		die("savesave requires a multithreading supported version of libzstd");

	ZSTD_freeCCtx(ctx);
}

int make_zip(const char *dest, const char *src, int is_dir)
{
	return 0;
}
