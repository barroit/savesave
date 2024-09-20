// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "atenter.h"
#include "maincmd.h"

int main(int argc, const char **argv)
{
	setup_program();
	post_setup_program();

	setup_message_translation();

	return cmd_main(argc, argv);
}
