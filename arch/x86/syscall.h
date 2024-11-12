/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * arch/x86/syscall.h
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#define CLOBBERED_REGISTER "cc", "rcx", "r11"

#define __syscall0(nr)						\
({								\
	long res;						\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr)					\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall1(nr, arg1)					\
({								\
	long res;						\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr)					\
		  "r" (rdi),					\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall2(nr, arg1, arg2)				\
({								\
	long res;						\
	register VARAFY(arg2, rsi) asm("rsi") = ARGIFY(arg2);	\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr),					\
		  "r" (rdi), "r" (rsi)				\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall3(nr, arg1, arg2, arg3)			\
({								\
	long res;						\
	register VARAFY(arg3, rdx) asm("rdx") = ARGIFY(arg3);	\
	register VARAFY(arg2, rsi) asm("rsi") = ARGIFY(arg2);	\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr),					\
		  "r" (rdi), "r" (rsi), "r" (rdx)		\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall4(nr, arg1, arg2, arg3, arg4)			\
({								\
	long res;						\
	register VARAFY(arg4, r10) asm("r10") = ARGIFY(arg4);	\
	register VARAFY(arg3, rdx) asm("rdx") = ARGIFY(arg3);	\
	register VARAFY(arg2, rsi) asm("rsi") = ARGIFY(arg2);	\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr),					\
		  "r" (rdi), "r" (rsi), "r" (rdx),		\
		  "r" (r10)					\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall5(nr, arg1, arg2, arg3, arg4, arg5)		\
({								\
	long res;						\
	register VARAFY(arg5, r8)  asm("r8")  = ARGIFY(arg5);	\
	register VARAFY(arg4, r10) asm("r10") = ARGIFY(arg4);	\
	register VARAFY(arg3, rdx) asm("rdx") = ARGIFY(arg3);	\
	register VARAFY(arg2, rsi) asm("rsi") = ARGIFY(arg2);	\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr),					\
		  "r" (rdi), "r" (rsi), "r" (rdx),		\
		  "r" (r10), "r" (r8)				\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#define __syscall6(nr, arg1, arg2, arg3, arg4, arg5, arg6)	\
({								\
	long res;						\
	register VARAFY(arg6, r9)  asm("r9")  = ARGIFY(arg6);	\
	register VARAFY(arg5, r8)  asm("r8")  = ARGIFY(arg5);	\
	register VARAFY(arg4, r10) asm("r10") = ARGIFY(arg4);	\
	register VARAFY(arg3, rdx) asm("rdx") = ARGIFY(arg3);	\
	register VARAFY(arg2, rsi) asm("rsi") = ARGIFY(arg2);	\
	register VARAFY(arg1, rdi) asm("rdi") = ARGIFY(arg1);	\
	asm volatile(						\
		"syscall"					\
		: "=a" (res)					\
		: "0" (nr),					\
		  "r" (rdi), "r" (rsi), "r" (rdx),		\
		  "r" (r10), "r" (r8), "r" (r9)			\
		: "memory", CLOBBERED_REGISTER);		\
	res;							\
})

#endif /* ARCH_SYSCALL_H */
