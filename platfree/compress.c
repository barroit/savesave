// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Strategy inspired by btrfs/zstd.c
 */

#include "compress.h"
#include "constructor.h"
#include "termas.h"
#include "debug.h"

USESTDIO CONSTRUCTOR(check_libzstd_compat)
{
	int ret;

	ZSTD_CCtx *ctx = ZSTD_createCCtx();
	BUG_ON(!ctx);

	ret = ZSTD_CCtx_setParameter(ctx, ZSTD_c_nbWorkers, 400);
	if (ZSTD_isError(ret))
		goto err_no_mt;

	ZSTD_freeCCtx(ctx);
	return;

err_no_mt:
	die(_("savesave requires a multithreading supported version of libzstd"));
}

int make_zip(const char *dest, const char *src, int is_dir)
{
	return 0;
}
