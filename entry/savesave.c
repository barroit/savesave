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

#define SAVESAVE_USAGE \
"savesave [--dotsav=<path>] [--output=<path> | --no-output] <command> [<args>]"

static struct savesave *savarr;
static size_t savnl;

const char *cm_dotsav_path;
const char *cm_output_path = (void *)-39;

int cm_has_output = 1;

static void prepare_dotsav(void)
{
	const char *name = dotsav_path();

	if (access(name, F_OK | R_OK) != 0)
		die(_("no dotsav (.savesave) was provided"));

	char *savstr = read_dotsav(name);

	savnl = dotsav_parse(savstr, &savarr);
	free(savstr);

	if (!savnl)
		die(_("no configuration found in dotsav `%s'"), name);
}

int cmd_main(int argc, const char **argv)
{
	subcmd_t runcmd = NULL;
	struct arguopt option[] = {
		APOPT_FILENAME(0, "dotsav", &cm_dotsav_path,
			       N_("dotsav file")),
		__APOPT_STRING(0, "output", &cm_output_path, "path",
			       N_("output file"), ARGUOPT_HASNEG),
		APOPT_GROUP("Savesave commands"),
		APOPT_MAINCOMMAND(&runcmd),
		APOPT_END(),
	};
	const char *usage[] = {
		SAVESAVE_USAGE,
		NULL,
	};

	if (argc < 2)
		prompt_shrt_help(usage, option);

	argupar_parse(&argc, &argv, option, usage,
		      AP_COMMAND_MODE | AP_NEED_ARGUMENT);

	if (cm_output_path == (void *)-39)
		cm_output_path = NULL;
	else if (cm_output_path == NULL)
		cm_has_output = 0;

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
