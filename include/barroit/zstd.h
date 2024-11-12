/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * A wrapper for zstd.h that conforms to kernel coding style
 */

#ifndef BRT_ZSTD_H
#define BRT_ZSTD_H

#define zstd_init_cctx ZSTD_initStaticCCtx

typedef ZSTD_CCtx zstd_cctx;

#endif /* BRT_ZSTD_H */
