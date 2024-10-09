/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * arch/generic/syscall.h
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#ifdef __syscall
# undef __syscall
# define __syscall syscall_noerrno
#endif

#define syscall_noerrno(...)			\
({						\
	int errnum = errno;			\
	int ret = syscall(__VA_ARGS__);		\
	if (ret == -1) {			\
		ret = -errno;			\
		errno = errnum;			\
	}					\
	ret;					\
})

#endif /* ARCH_SYSCALL_H */
