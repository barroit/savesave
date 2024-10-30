// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "dotsav.h"
#include "termas.h"
#include "proc.h"
#include "cls.h"
#include "baktmr.h"
#include "path.h"
#include "strlist.h"
#include "noleak.h"

static int cmd_backup_exec(int argc, const char **argv)
{
	dotsav_prepare();

	struct arguopt option[] = {
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup exec",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_NO_ARGUMENT);

	proc_assert_unique();
	proc_detach();

	cls_push(pid_erase);
	pid_dump();

	cls_push(baktmr_disarm);
	baktmr_arm();

	return 0;
}

static int cmd_backup_kill(int argc, const char **argv)
{
	int dryrun = 0;
	struct arguopt option[] = {
		APOPT_SWITCH('n', "dry-run", &dryrun,
			     N_("just print messages")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup kill [-n | --dry-run]",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_NO_ARGUMENT);

	pid_t pid = pid_retrieve();
	if (pid == max_value(pid)) {
		puts(_("no running process found for savesave"));
		exit(128);
	}

	if (dryrun)
		goto out;

	int err = pid_kill(pid, SIGTERM);
	if (!err)
		goto out;

	BUG_ON(err != EPERM);
	die_errno(_("failed to kill process `%d'"), pid);

out:
	printf(_("killed process `%d'\n"), pid);
	exit(0);
}

int cmd_backup(int argc, const char **argv)
CMDDESCRIP("Start backup task in background")
{
	subcmd_t cmd = NULL;

	struct arguopt option[] = {
		APOPT_GROUP("List of subcommands"),
		APOPT_SUBCMD("exec", &cmd,
			     _("start a backup routine"), cmd_backup_exec, 0),
		APOPT_SUBCMD("kill", &cmd,
			     _("stop a backup routine"), cmd_backup_kill, 0),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave backup [exec]",
		"savesave backup kill [-n | --dry-run]",
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
