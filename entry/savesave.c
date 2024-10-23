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
#include "proc.h"
#include "path.h"

static struct savesave *savarr;
static size_t savnl;

const char *cm_dotsav_path;
const char *cm_output_path;

static void prepare_dotsav(void)
{
	if (!cm_dotsav_path)
		cm_dotsav_path = dotsav_path();
	if (access(cm_dotsav_path, F_OK | R_OK) != 0)
		die(_("no dotsav (.savesave) was provided"));

	char *savstr = read_dotsav(cm_dotsav_path);

	savnl = dotsav_parse(savstr, &savarr);
	free(savstr);

	if (!savnl)
		die(_("no configuration found in dotsav `%s'"),
		    cm_dotsav_path);
}

int cmd_main(int argc, const char **argv)
{
	subcmd_t runcmd = NULL;
	struct arguopt option[] = {
		APOPT_FILENAME(0, "dotsav", &cm_dotsav_path,
			       N_("use specified dotsav")),
		APOPT_FILENAME(0, "log", &cm_output_path,
			       N_("output to log file (only affect long-running tasks)")),
		APOPT_GROUP("Savesave commands"),
		APOPT_MAINCOMMAND(&runcmd),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave [--dotsav=<path>] <command> [<args>]",
		NULL,
	};

	if (argc < 2)
		prompt_shrt_help(usage, option);

	argupar_parse(&argc, &argv, option, usage,
		      AP_COMMAND_MODE | AP_NEED_ARGUMENT);
	BUG_ON(!runcmd);

	struct arguopt *cmd = option;
	for_each_option(cmd) {
		if (runcmd != cmd->subcmd)
			continue;

		if (cmd->flags & CMD_USEDOTSAV)
			prepare_dotsav();

		break;
	}

	return runcmd(argc, argv);
}

size_t retrieve_dotsav(struct savesave **sav)
{
	if (!savarr)
		prepare_dotsav();

	*sav = savarr;
	return savnl;
}
