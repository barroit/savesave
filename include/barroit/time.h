/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_TIME_H
#define BRT_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t timestamp_t;

timestamp_t gettimestamp(void);

#ifdef __cplusplus
}
#endif

#endif /* BRT_TIME_H */
