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

#define __syscall0 syscall_noerrno
#define __syscall1 syscall_noerrno
#define __syscall2 syscall_noerrno
#define __syscall3 syscall_noerrno
#define __syscall4 syscall_noerrno
#define __syscall5 syscall_noerrno
#define __syscall6 syscall_noerrno

#endif /* ARCH_SYSCALL_H */
