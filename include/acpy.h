/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef __linux__
# error "acpy.h is linux-specific"
#endif

#ifndef ACPY_H
#define ACPY_H

#define ACPY_FULLCOMP (1 << 0)	/* process results completely */

extern int acpy_disabled;

void acpy_deploy(uint qs, size_t bs);

int acpy_copy(const char *src, const char *dest);

int acpy_comp_cqe(flag_t flags);

void acpy_drop_entries(void);

#endif /* ACPY_H */
