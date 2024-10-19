/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef MCPY_H
#define MCPY_H

extern int mcpy_disabled;

void mcpy_deploy(uint qs, size_t bs, uint wt);

int mcpy_copy(const char *src, const char *dest);

#endif /* MCPY_H */
