// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "atenter.h"
#include "termas.h"
#include "constructor.h"
#include "iter.h"

USESTDIO
CONSTRUCTOR(check_os_version)
{
	if (!IsWindows7OrGreater())
		die(_("unsupported windows version (at least win7)"));
}

void setup_program(void)
{
	uint i;
	constructor_t constructors[] = CONSTRUCTOR_LIST_INIT;

	for_each_idx(i, sizeof_array(constructors))
		constructors[i]();
}

void post_setup_program(void)
{
	uint i;
	constructor_t constructors[] = CONSTRUCTOR_LIST_INIT_USESTDIO;

	for_each_idx(i, sizeof_array(constructors))
		constructors[i]();
}
