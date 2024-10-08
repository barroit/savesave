/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * arch/arm64/syscall.h
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#define CLOBBERED_REGISTER "cc"

#define __syscall0(number)					\
({								\
	register long x0 asm("x0");				\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8)					\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall1(number, arg1)				\
({								\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0)					\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall2(number, arg1, arg2)				\
({								\
	register VARAFY(arg2, x1) asm("x1") = arg2;		\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0), "r" (x1)				\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall3(number, arg1, arg2, arg3)			\
({								\
	register VARAFY(arg3, x2) asm("x2") = arg3;		\
	register VARAFY(arg2, x1) asm("x1") = arg2;		\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0), "r" (x1), "r" (x2)			\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall4(number, arg1, arg2, arg3, arg4)		\
({								\
	register VARAFY(arg4, x3) asm("x3") = arg4;		\
	register VARAFY(arg3, x2) asm("x2") = arg3;		\
	register VARAFY(arg2, x1) asm("x1") = arg2;		\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0), "r" (x1), "r" (x2),			\
		  "r" (x3)					\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall5(number, arg1, arg2, arg3, arg4, arg5)	\
({								\
	register VARAFY(arg5, x4) asm("x4") = arg5;		\
	register VARAFY(arg4, x3) asm("x3") = arg4;		\
	register VARAFY(arg3, x2) asm("x2") = arg3;		\
	register VARAFY(arg2, x1) asm("x1") = arg2;		\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0), "r" (x1), "r" (x2),			\
		  "r" (x3), "r" (x4)				\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#define __syscall6(number, arg1, arg2, arg3, arg4, arg5, arg6)	\
({								\
	register VARAFY(arg6, x5) asm("x5") = arg6;		\
	register VARAFY(arg5, x4) asm("x4") = arg5;		\
	register VARAFY(arg4, x3) asm("x3") = arg4;		\
	register VARAFY(arg3, x2) asm("x2") = arg3;		\
	register VARAFY(arg2, x1) asm("x1") = arg2;		\
	register VARAFY(arg1, x0) asm("x0") = arg1;		\
	register VARAFY(number, x8) asm("x8") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (x0)					\
		: "r" (x8),					\
		  "r" (x0), "r" (x1), "r" (x2),			\
		  "r" (x3), "r" (x4), "r" (x5)			\
		: "memory", CLOBBERED_REGISTER);		\
	(long)x0;						\
})

#endif /* ARCH_SYSCALL_H */
