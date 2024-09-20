/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef MAINCMD_H
#define MAINCMD_H

int cmd_main(int argc, const char **argv);
extern int is_longrunning;

struct savesave;
struct savesave *get_dotsav(void);

#endif /* MAINCMD_H */
