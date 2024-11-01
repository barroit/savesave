// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "iter.h"
#include "path.h"
#include "termas.h"
#include "strbuf.h"
#include "noleak.h"

#define is_dot_ph(name) ((name)[0] == '.' && (name)[1] == 0)

struct pathinfo {
	const char *name;
	const char *(*func)(void);
};

static int cmd_query_path(int argc, const char **argv)
{
	struct arguopt option[] = {
		APOPT_GROUP(N_("List of query names")),
		APOPT_MEMBINFO(".",      N_("list all paths")),
		APOPT_MEMBINFO("output", N_("log file")),
		APOPT_MEMBINFO("home",   N_("home directory")),
		APOPT_MEMBINFO("dotsav", N_("dotsav configuration")),
		APOPT_MEMBINFO("pid",    N_("process id file")),
		APOPT_MEMBINFO("data",   N_("program data directory")),
		APOPT_MEMBINFO("tmp",    N_("temporary data directory")),
		APOPT_MEMBINFO("exec",   N_("executable file")),
		APOPT_MEMBINFO("ws",     N_("executable working directory")),
		APOPT_MEMBINFO("locale", N_("locale file directory")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave query path <name>",
		NULL,
	};

	argupar_opt_mas_pad = 16;
	argupar_parse(&argc, &argv, option, usage, AP_NEED_ARGUMENT);

	struct pathinfo info[] = {
		{ "output", __output_path },
		{ "home",   home_dir },
		{ "dotsav", __dotsav_path },
		{ "pid",    pid_path },
		{ "data",   data_dir },
		{ "tmp",    tmp_dir },
		{ "exec",   exec_path },
		{ "ws",     exec_dir },
		{ "locale", locale_dir },
	};

	int i;
	int has_dot_ph = 0;
	for_each_idx(i, argc)
		if (is_dot_ph(argv[i]))
			has_dot_ph = 1;

	struct strbuf sb = STRBUF_INIT;
	for_each_idx(i, argc) {
		uint j;
		const char *name = argv[i];

		for_each_idx(j, sizeof_array(info)) {
			struct pathinfo *q = &info[j];

			if (strcmp(name, q->name))
				continue;
			else if (has_dot_ph)
				goto next;

			const char *path = q->func();

			strbuf_boconcat(&sb, path);
			strbuf_normalize_path(&sb);

			printf("%-10s %s\n", name, sb.str);
			goto next;
		}

		if (!is_dot_ph(name))
			warn("unknown query name `%s'", name);
next:;
	}

	if (has_dot_ph) {
		for_each_idx(i, sizeof_array(info)) {
			struct pathinfo *q = &info[i];
			const char *path = q->func();

			strbuf_boconcat(&sb, path);
			strbuf_normalize_path(&sb);
			printf("%-10s %s\n", q->name, sb.str);
		}
	}

	exit(0);
}

static int cmd_query_limit(int argc, const char **argv)
{
	puts("limit");
	exit(0);
}

int cmd_query(int argc, const char **argv)
CMDDESCRIP("Query default information of Savesave")
{
	subcmd_t subcmd = NULL;

	struct arguopt option[] = {
		APOPT_GROUP("List of subcommands"),
		APOPT_SUBCMD("path", &subcmd,
			     _("list default paths"), cmd_query_path, 0),
		APOPT_SUBCMD("limit", &subcmd,
			     _("list default limits"), cmd_query_limit, 0),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave query <command> <name>",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage,
		      AP_COMMAND_MODE | AP_NEED_ARGUMENT);
	return subcmd(argc, argv);
}
