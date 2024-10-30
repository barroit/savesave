/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BARRIER_H
#define BARRIER_H

#define READ_ONCE(x) \
	__atomic_load_n(x, __ATOMIC_RELAXED)

#define WRITE_ONCE(x, val) \
	__atomic_store_n(x, val, __ATOMIC_RELAXED)

#define smp_load_acquire(x) \
	__atomic_load_n(x, __ATOMIC_ACQUIRE)

#define smp_store_release(x, val) \
	__atomic_store_n(x, val, __ATOMIC_RELEASE)

#define smp_inc_return(x) \
	__atomic_add_fetch(x, 1, __ATOMIC_ACQ_REL)

#define smp_dec_return(x) \
	__atomic_sub_fetch(x, 1, __ATOMIC_ACQ_REL)

#define smp_add_return(x, val) \
	__atomic_add_fetch(x, val, __ATOMIC_ACQ_REL)

#define smp_sub_return(x, val) \
	__atomic_sub_fetch(x, val, __ATOMIC_ACQ_REL)

#define smp_mb() \
	__atomic_thread_fence(__ATOMIC_ACQ_REL)

#endif /* BARRIER_H */
