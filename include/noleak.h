/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef NOLEAK_H
#define NOLEAK_H

void __noleak(void *ptr, size_t size);

#ifdef CONFIG_SUPPRESS_ANNOTATED_LEAKS
# define NOLEAK(var) __noleak(&(var), sizeof(var))
#else
# define NOLEAK(var) do {} while (0)
#endif

#endif /* NOLEAK_H */
