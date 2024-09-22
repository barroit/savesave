/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef _WIN32
# error "atenter.h is win-spec"
#endif

#ifndef ATENTER_H
#define ATENTER_H

void do_setup(void);

void do_delayed_setup(void);

#endif /* ATENTER_H */
