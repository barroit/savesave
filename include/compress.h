/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef ZIP_H
#define ZIP_H

#ifdef __cplusplus
extern "C" {
#endif

int make_zip(const char *dest, const char *src, int is_dir);

#ifdef __cplusplus
}
#endif

#endif /* ZIP_H */
