// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "win/alloc.hpp"
#include "termsg.h"
#include "alloc.h"

template <typename T>
T* xnew()
{
	try {
		return new T;
	} catch (const std::bad_alloc& _) {
		die("out of memory (tried to allocate %" PRIuMAX " bytes)",
		    (uintmax_t)sizeof(T));
	}
}

template <typename T>
T* xnew(size_t n)
{
	size_t nb = sizeof(T) * n;
	memory_limit_check(nb);

	try {
		return new T[n];
	} catch (const std::bad_alloc& _) {
		die("out of memory (tried to allocate %" PRIuMAX " bytes)",
		    (uintmax_t)nb);
	}
}

template char *xnew<char>(size_t);
