/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * arch/riscv/syscall.h
 */

#ifndef ARCH_SYSCALL_H
#define ARCH_SYSCALL_H

#define __syscall0(number)					\
({								\
	register long a0 asm("a0");				\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7)					\
		: "memory");					\
	(long)a0;						\
})

#define __syscall1(number, arg1)				\
({								\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0)					\
		: "memory");					\
	(long)a0;						\
})

#define __syscall2(number, arg1, arg2)				\
({								\
	register VARAFY(arg2, a1) asm("a1") = ARGIFY(arg2);	\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0), "r" (a1)				\
		: "memory");					\
	(long)a0;						\
})

#define __syscall3(number, arg1, arg2, arg3)			\
({								\
	register VARAFY(arg3, a2) asm("a2") = ARGIFY(arg3);	\
	register VARAFY(arg2, a1) asm("a1") = ARGIFY(arg2);	\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0), "r" (a1), "r" (a2)			\
		: "memory");					\
	(long)a0;						\
})

#define __syscall4(number, arg1, arg2, arg3, arg4)		\
({								\
	register VARAFY(arg4, a3) asm("a3") = ARGIFY(arg4);	\
	register VARAFY(arg3, a2) asm("a2") = ARGIFY(arg3);	\
	register VARAFY(arg2, a1) asm("a1") = ARGIFY(arg2);	\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0), "r" (a1), "r" (a2),			\
		  "r" (a3)					\
		: "memory");					\
	(long)a0;						\
})

#define __syscall5(number, arg1, arg2, arg3, arg4, arg5)	\
({								\
	register VARAFY(arg5, a4) asm("a4") = ARGIFY(arg5);	\
	register VARAFY(arg4, a3) asm("a3") = ARGIFY(arg4);	\
	register VARAFY(arg3, a2) asm("a2") = ARGIFY(arg3);	\
	register VARAFY(arg2, a1) asm("a1") = ARGIFY(arg2);	\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0), "r" (a1), "r" (a2),			\
		  "r" (a3), "r" (a4)				\
		: "memory");					\
	(long)a0;						\
})

#define __syscall6(number, arg1, arg2, arg3, arg4, arg5, arg6)	\
({								\
	register VARAFY(arg6, a5) asm("a5") = ARGIFY(arg6);	\
	register VARAFY(arg5, a4) asm("a4") = ARGIFY(arg5);	\
	register VARAFY(arg4, a3) asm("a3") = ARGIFY(arg4);	\
	register VARAFY(arg3, a2) asm("a2") = ARGIFY(arg3);	\
	register VARAFY(arg2, a1) asm("a1") = ARGIFY(arg2);	\
	register VARAFY(arg1, a0) asm("a0") = ARGIFY(arg1);	\
	register VARAFY(number, a7) asm("a7") = number;		\
	asm volatile(						\
		"svc 0"						\
		: "=r" (a0)					\
		: "r" (a7),					\
		  "r" (a0), "r" (a1), "r" (a2),			\
		  "r" (a3), "r" (a4), "r" (a5)			\
		: "memory");					\
	(long)a0;						\
})

#endif /* ARCH_SYSCALL_H */
