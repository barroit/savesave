/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * Generated by scripts/gen-constructor.sh
 */

#ifndef BRTGEN_CONSTRUCTOR_H
#define BRTGEN_CONSTRUCTOR_H

#define USESTDIO

#if defined(__unix__)
# define __CONSTRUCTOR CONSTRUCTOR
# define CONSTRUCTOR(name) static void __attribute__((constructor)) name(void)
#elif defined(_WIN32)
/*
 * __attribute__((constructor)) doesn't work on windows, we need to call these
 * functions manually
 */
# define CONSTRUCTOR(name) void name(void)

CONSTRUCTOR(setup_atexit);
CONSTRUCTOR(check_libzstd_compat);
CONSTRUCTOR(setup_crt_report_hook);
CONSTRUCTOR(check_os_version);

#define CONSTRUCTOR_LIST_INIT { \
	setup_atexit, \
	setup_crt_report_hook, \
}

#define CONSTRUCTOR_LIST_INIT_USESTDIO { \
	check_libzstd_compat, \
	check_os_version, \
}

typedef typeof(&setup_atexit) constructor_t;
#endif

#endif /* BRTGEN_CONSTRUCTOR_H */
