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
	OPT_DONE    = -1,
	OPT_FLAG    = 0,
	OPT_UNKNOWN = '?',
	OPT_CONFIG  = 'f',
	OPT_HELP    = 'h',
	OPT_VERSION = 'v',
};

extern char *optarg;

enum optid parse_option(int argc, char *const *argv);

#ifdef __cplusplus
}
#endif

#endif /* RUNOPT_H */
