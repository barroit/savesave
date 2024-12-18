// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "argupar.h"
#include "termas.h"
#include "path.h"
#include "proc.h"

#define SAVESAVE_USAGE \
"savesave [--dotsav=<path>] [--output=<path> | --no-output] <command> [<args>]"

const char *cm_dotsav_path;
const char *cm_output_path = CM_OUTPUT_UNSET;

int cm_has_output = 1;
int cm_no_detach;

int cmd_main(int argc, const char **argv)
{
	subcmd_t runcmd;
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

	if (cm_output_path == CM_OUTPUT_UNSET)
		cm_output_path = NULL;
	else if (cm_output_path == NULL)
		cm_has_output = 0;

	if (cm_io_need_update) {
		const char *name = output_path();
		cm_no_detach = termas_rd_output(name);
	}

	return runcmd(argc, argv);
}
