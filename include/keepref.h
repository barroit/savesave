/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef KEEPREF_H
#define KEEPREF_H

void __keepref(void *ptr, size_t size);

#ifdef CONFIG_SUPPRESS_ANNOTATED_LEAKS
# define KEEPREF(var) __keepref(&(var), sizeof(var))
#else
# define KEEPREF(var) do {} while (0)
#endif

#endif /* KEEPREF_H */
