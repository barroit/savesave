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
#include "debug.h"
#include "proc.h"

static const char *dotsav_path;
static struct savesave *savarr;

int is_longrunning;

static void prepare_dotsav(void)
{
	if (!dotsav_path)
		dotsav_path = get_dotsav_defpath();
	if (!dotsav_path)
		die(_("no dotsav (.savesave) was provided"));

	char *savstr = read_dotsav(dotsav_path);
	if (!savstr)
		die(_("unable to retrieve content for dotsav `%s'"),
		    dotsav_path);

	size_t savnl = dotsav_parse(savstr, &savarr);
	free(savstr);

	if (!savnl)
		die(_("no configuration found in dotsav `%s'"), dotsav_path);

	DEBUG_RUN()
		dotsav_print(savarr);
}

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
			     AP_STOPAT_NONOPT | AP_NO_ENDOFOPT);

	argupar_subcommand_t runcmd;
	struct arguopt commands[] = MAINCOMMAND_LIST_INIT(&runcmd);

	if (!argc)
		die(_("savesave requires a command"));

	argc = argupar_parse(&ap, commands, NULL, AP_COMMAND_MODE);

	struct arguopt *cmd = commands;
	for_each_option(cmd) {
		if (runcmd != cmd->subcmd)
			continue;

		if (cmd->flag & CMD_UNIQUEPROC)
			check_unique_process();

		if (cmd->flag & CMD_USEDOTSAV)
			prepare_dotsav();

		if (cmd->flag & CMD_LONGRUNNING)
			is_longrunning = 1;

		break;
	}

	argc--;
	argv++;
	return runcmd(argc, argv);
}

struct savesave *get_dotsav(void)
{
	BUG_ON(!savarr);
	return savarr;
}
