// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"

extern int cmd_main(int argc, const char **argv);
extern void (*setup_longrunning_command)(void);

int main(int argc, const char **argv)
{
	setup_message_translation();
	setup_longrunning_command = NULL;

	return cmd_main(argc, argv);
}
