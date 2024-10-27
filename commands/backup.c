// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "dotsav.h"
#include "maincmd.h"
#include "termas.h"
#include "proc.h"
#include "atexit.h"
#include "calc.h"

static int cmd_backup_start(int argc, const char **argv)
{
	int frequent;
	struct arguopt option[] = {
		APOPT_SWITCH('m', "frequent", &frequent,
			     N_("backup appears frequently")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup start [-m | --frequent]",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_NO_ARGUMENT);

	check_unique_process(NULL, 1);
	detach_process();

	setup_lr_logging();
	atexit_enque(teardown_lr_logging);

	push_process_id();
	atexit_enque(pop_process_id);

	atexit_apply();

	/* setup timer... */

	while (39)
		pause();
	exit(0);
}

static int cmd_backup_stop(int argc, const char **argv)
{
	int test;
	struct arguopt option[] = {
		APOPT_SWITCH('n', "dry-run", &test, N_("just print pid")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup start [-m | --frequent]",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_NO_ARGUMENT);

	char *name;
	pid_t pid = check_unique_process(&name, 0);
	int nf = pid == max_int_valueof(pid);
	if (nf || test)
		goto finish;

	kill_process(pid, SIGINT);
	unlink(name);

finish:
	if (nf)
		puts("no running process of savesave was found");
	else
		printf("killed the process of savesave `%d'\n", pid);
	exit(0);
}

USEDOTSAV
int cmd_backup(int argc, const char **argv)
CMDDESCRIP("Start backup task in background")
{
	subcmd_t cmd = cmd_backup_start;

	struct arguopt option[] = {
		APOPT_GROUP("List of subcommands"),
		APOPT_SUBCMD("start", &cmd,
			     _("start a backup routine"), cmd_backup_start, 0),
		APOPT_SUBCMD("stop", &cmd,
			     _("stop a backup routine"), cmd_backup_stop, 0),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup [start [-m | --frequent]]",
		"savesave backup stop",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_COMMAND_MODE);
	if (!cmd) {
		struct strlist sl;
		strlist_init(&sl, STRLIST_STORE_REF);

		strlist_push(&sl, "exec");
		strlist_join_argv(&sl, argv);

		argc = sl.size;
		argv = (const char **)strlist_dump2(&sl, 0);
		cmd = cmd_backup_exec;

		NOLEAK(argv);
		strlist_destroy(&sl);
	}

	return cmd(argc, argv);
}
