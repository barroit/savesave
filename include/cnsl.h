/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef _WIN32
# error "cnsl.h is win-spec"
#endif

#ifndef CNSL_H
#define CNSL_H

void cnsl_attach(void);

void cnsl_show(void);

void cnsl_hide(void);

#endif /* CNSL_H */
