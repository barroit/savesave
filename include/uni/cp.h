/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef UNI_CP_H
#define UNI_CP_H

#ifdef __cplusplus
extern "C" {
#endif

int copyfile(int src, int dest, off_t len);

#ifdef __cplusplus
}
#endif

#endif /* UNI_CP_H */
