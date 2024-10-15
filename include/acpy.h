/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef __linux
# error "acpy.h is linux-specific"
#endif

#ifndef ACPY_H
#define ACPY_H

extern int acpy_disabled;

void acpy_deploy(void);

int acpy_copy(const char *src, const char *dest);

#endif /* ACPY_H */
