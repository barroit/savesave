// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "termas.h"
#include "path.h"

#define SAVESAVE_USAGE \
"savesave [--dotsav=<path>] [--output=<path> | --no-output] <command> [<args>]"

const char *cm_dotsav_path;
const char *cm_output_path = (void *)-39;

int cm_has_output = 1;

static void redirect_output(void)
{
	const char *name = output_path();

	int fd = flexcreat(name);
	if (fd == -1) {
		warn_errno(ERRMAS_OPEN_FILE(name));
		return;
	}

	if (dup2(fd, STDOUT_FILENO) == -1) {
		warn_errno(_("failed to redirect stdout to %s"), name);
		goto err_out;
	}

	if (dup2(fd, STDERR_FILENO) == -1) {
		warn_errno(_("failed to redirect stderr to %s"), name);
		goto err_out;
	}

err_out:
	close(fd);
}

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

	if (cm_output_path == (void *)-39)
		cm_output_path = NULL;
	else if (cm_output_path == NULL)
		cm_has_output = 0;

	if (!cm_has_output || cm_output_path)
		redirect_output();

	return runcmd(argc, argv);
}
