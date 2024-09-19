// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "atenter.h"

extern int cmd_main(int argc, const char **argv);
extern void (*prepare_longrunning)(void);

int main(int argc, const char **argv)
{
	setup_program();
	post_setup_program();

	setup_message_translation();

	prepare_longrunning = NULL;
	return cmd_main(argc, argv);
}
