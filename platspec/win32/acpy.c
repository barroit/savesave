// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "acpy.h"

int acpy_disabled;

void acpy_deploy(uint qs, size_t bs)
{
	acpy_disabled = 1;
}

int acpy_copy(const char *src, const char *dest)
{
	return 0;
}
