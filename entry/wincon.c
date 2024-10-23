// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "command.h"
#include "i18n.h"
#include "atenter.h"
#include "proc.h"
#include "termas.h"

int main(int argc, const char **argv)
{
	setup_program();
	post_setup_program();

	setup_message_translation();

	cmd_main(argc, argv);

	HANDLE thread = GetCurrentThread();
	WaitForSingleObject(thread, INFINITE);
}
