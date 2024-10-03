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
extern void (*prepare_longrunning)(void);

struct savesave;
size_t retrieve_dotsav(struct savesave **sav);

struct userspec_argument {
	const char *dotsav_path;
	const char *lr_log_path;
};

extern struct userspec_argument userspec;

#endif /* MAINCMD_H */
