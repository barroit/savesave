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
	const char *argb[3] = { "-h" };

	argc--;
	argv++;
	if (argc == 0) {
		argc = 1;
		argv = argb;
	}

	struct arguopt options[] = {
		APOPT_FILENAME(0, "dotsav", &dotsav_path,
			       "Configuration file for savesave"),
		APOPT_END(),
	};

parse_command:
	argupar_init(&ap, argc, argv);
	argc = argupar_parse(&ap, options, NULL, ARGUPAR_STOPAT_NONOPT);

	if (!argc) {
		error(_("no command specified"));
		argc = 1;
		argv = argb;
		goto parse_command;
	}

	argupar_subcommand_t handle;
	struct arguopt commands[] = APOPT_MAIN_COMMAND_INIT(&handle);

	argc = argupar_parse(&ap, commands, NULL, ARGUPAR_COMMAND_MODE);
	handle(argc, argv);
	exit(0);

	if (!dotsav_path)
		dotsav_path = get_dotsav_defpath();
	if (!dotsav_path)
		die(_("no dotsav (.savesave) was provided"));

	struct savesave *savv;
	size_t savc = parse_dotsav(dotsav_path, &savv);
	if (!savc)
		die(_("dotsav `%s' must contain at least one configuration"),
		    dotsav_path);
	DEBUG_RUN()
		print_dotsav(savv, savc);

	backup(savv);
	exit(0);
}
