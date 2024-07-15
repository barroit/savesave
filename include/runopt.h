// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef RUNOPT_H
#define RUNOPT_H

#ifdef __cplusplus
extern "C" {
#endif

enum optid {
	OPT_VERSION = 'v',
	OPT_HELP    = 'h',
};

int parse_option(int argc, char *const *argv);

#ifdef __cplusplus
}
#endif

#endif /* RUNOPT_H */
