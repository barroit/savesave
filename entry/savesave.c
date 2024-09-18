// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "termas.h"
#include "dotsav.h"
#include "debug.h"
#include "backup.h"

static const char *dotsav_path;

int main(int argc, const char **argv)
{
	setup_message_translation();

	argupar_t ap;
	struct arguopt options[] = {
		APOPT_FILENAME(0, "dotsav", &dotsav_path,
			       _("Configuration file for savesave")),
		APOPT_END(),
	};

	argc--;
	argv++;

	argupar_init(&ap, argc, argv);
	argc = argupar_parse(&ap, options, NULL,
			     AP_STOPAT_NONOPT |
			     AP_EXPECT_ARGS |
			     AP_NO_ENDOFOPT);

	argupar_subcommand_t runcmd;
	struct arguopt commands[] = APOPT_MAIN_COMMAND_INIT(&runcmd);

	if (!argc)
		error(_("no command specified"));
	argc = argupar_parse(&ap, commands, NULL,
			     AP_COMMAND_MODE | AP_EXPECT_ARGS);

	return runcmd(argc, argv);
}
