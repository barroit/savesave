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
