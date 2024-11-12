/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef ACPY_H
#define ACPY_H

#define ACPY_FULLCOMP (1 << 0)	/* process results completely */

extern int acpy_disabled;

void acpy_deploy(uint qs, size_t bs);

int acpy_copy(const char *src, const char *dest);

#endif /* ACPY_H */
