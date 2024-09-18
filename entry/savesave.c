// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "termas.h"
#include "dotsav.h"
#include "list.h"

static const char *dotsav_path;

void (*setup_longrunning_command)(void);

int cmd_main(int argc, const char **argv)
{
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
	struct arguopt commands[] = MAINCOMMAND_LIST_INIT(&runcmd);

	if (!argc)
		error(_("no command specified"));
	argc = argupar_parse(&ap, commands, NULL,
			     AP_COMMAND_MODE | AP_EXPECT_ARGS);

	if (setup_longrunning_command) {
		size_t i;
		const char *longrun[] = LONGRUNNING_LIST_INIT;

		for_each_idx(i, sizeof_array(longrun)) {
			if (strcmp(*argv, longrun[i]) == 0) {
				setup_longrunning_command();
				break;
			}
		}
	}

	argc--;
	argv++;
	return runcmd(argc, argv);
}
