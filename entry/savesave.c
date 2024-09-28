// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "maincmd.h"
#include "argupar.h"
#include "termas.h"
#include "dotsav.h"
#include "list.h"
#include "debug.h"
#include "proc.h"
#include "path.h"

static const char *dotsav_path = NULL;

static struct savesave *savarr;
static size_t savnl;

int is_longrunning;
void (*prepare_longrunning)(void);

static void prepare_dotsav(void)
{
	if (!dotsav_path)
		dotsav_path = get_dotsav_filename();
	if (access(dotsav_path, F_OK | R_OK) != 0)
		die(_("no dotsav (.savesave) was provided"));

	char *savstr = read_dotsav(dotsav_path);
	if (!savstr)
		die(_("unable to retrieve content for dotsav `%s'"),
		    dotsav_path);

	savnl = dotsav_parse(savstr, &savarr);
	free(savstr);

	if (!savnl)
		die(_("no configuration found in dotsav `%s'"), dotsav_path);
}

size_t get_dotsav(struct savesave **ret)
{
	if (!savarr)
		prepare_dotsav();

	*ret = savarr;
	return savnl;
}

int cmd_main(int argc, const char **argv)
{
	argupar_subcommand_t runcmd = NULL;
	struct arguopt option[] = {
		APOPT_FILENAME(0, "dotsav", &dotsav_path,
			       N_("use specified dotsav")),
		APOPT_GROUP("Savesave commands"),
		APOPT_MAINCOMMAND(&runcmd),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave [--dotsav=<path>] <command> [<args>]",
		NULL,
	};
	const char *argfb[] = { "-h", NULL };
	argupar_t ap;

	argc--;
	argv++;
	if (argc == 0) {
		argv = argfb;
		argc = 1;
	}

	argupar_init(&ap, argc, argv);
	argc = argupar_parse(&ap, option, usage, AP_COMMAND_MODE);

	if (!runcmd)
		die(_("savesave requires a command"));

	struct arguopt *cmd = option;
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

	if (is_longrunning && prepare_longrunning)
		prepare_longrunning();

	argc--;
	argv++;
	return runcmd(argc, argv);
}
