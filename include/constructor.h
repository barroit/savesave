/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Generated by scripts/genh-constructor.sh
 */

#ifndef GENH_CONSTRUCTOR_H
#define GENH_CONSTRUCTOR_H

#define USESTDIO

#ifdef _WIN32

/*
 * __attribute__((constructor)) just doesn't work on windows
 */
# define CONSTRUCTOR(name) void name(void)

CONSTRUCTOR(init_u8tstr_table);
CONSTRUCTOR(setup_crt_report_hook);
USESTDIO CONSTRUCTOR(check_libzstd_compat);
USESTDIO CONSTRUCTOR(check_os_version);

#define CONSTRUCTOR_LIST_INIT { \
	init_u8tstr_table, \
	setup_crt_report_hook, \
}

#define CONSTRUCTOR_USESTDIO_LIST_INIT { \
	check_libzstd_compat, \
	check_os_version, \
}

typedef typeof(&check_libzstd_compat) constructor_t;

#else

#define CONSTRUCTOR(name) static void __attribute__((constructor)) name(void)

#endif /* _WIN32 */

#endif /* GENH_CONSTRUCTOR_H */
