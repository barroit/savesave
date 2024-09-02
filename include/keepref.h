/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef KEEPREF_H
#define KEEPREF_H

#ifdef __cplusplus
extern "C" {
#endif

void __keepref(void *ptr, size_t size);

#ifdef CONFIG_SUPPRESS_LEAK_REPORT
# define KEEPREF(var) __keepref(&(var), sizeof(var))
#else
# define KEEPREF(var) do {} while (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* KEEPREF_H */
