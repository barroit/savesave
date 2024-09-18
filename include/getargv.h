/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef _WIN32
# error "getargv.h is win-spec"
#endif

#ifndef GETARGV_H
#define GETARGV_H

int getargv(const char ***argv);

#endif /* GETARGV_H */
