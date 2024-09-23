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

void prepare_dotsav(void);

struct savesave;
extern struct savesave *savarr;

#endif /* MAINCMD_H */
