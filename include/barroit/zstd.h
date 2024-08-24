/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * A wrapper for zstd.h that conforms to kernel coding style
 */

#ifndef BRT_ZSTD_H
#define BRT_ZSTD_H

#ifdef __cplusplus
extern "C" {
#endif

#define zstd_init_cctx ZSTD_initStaticCCtx

typedef ZSTD_CCtx zstd_cctx;

#ifdef __cplusplus
}
#endif

#endif /* BRT_ZSTD_H */
