// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "atenter.h"
#include "termas.h"
#include "constructor.h"
#include "list.h"

USESTDIO CONSTRUCTOR(check_os_version)
{
	if (!IsWindows7OrGreater())
		die(_("unsupported windows version (at least win7)"));
}

void do_setup(void)
{
	uint i;
	constructor_t constructors[] = CONSTRUCTOR_LIST_INIT;

	for_each_idx(i, sizeof_array(constructors))
		constructors[i]();
}

void do_delayed_setup(void)
{
	uint i;
	constructor_t constructors[] = CONSTRUCTOR_USESTDIO_LIST_INIT;

	for_each_idx(i, sizeof_array(constructors))
		constructors[i]();
}