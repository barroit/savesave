// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "list.h"
#include "path.h"
#include "termas.h"
#include "strbuf.h"
#include "noleak.h"

struct pathinfo {
	const char *name;
	const char *(*func)(void);
};

static int cmd_query_path(int argc, const char **argv)
{
	struct arguopt option[] = {
		APOPT_GROUP(N_("List of query names")),
		APOPT_MEMBINFO("log",      N_("log file")),
		APOPT_MEMBINFO("home",     N_("home directory")),
		APOPT_MEMBINFO("dotsav",   N_("dotsav configuration")),
		APOPT_MEMBINFO("proc-id",  N_("process id file")),
		APOPT_MEMBINFO("data-dir", N_("program data directory")),
		APOPT_MEMBINFO("tmp-dir",  N_("temporary data directory")),
		APOPT_MEMBINFO("exe-dir",  N_("executable directory")),
		APOPT_MEMBINFO("locale",   N_("locale file directory")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave query path <name>",
		NULL,
	};

	argupar_cmd_prefix = "savesave query";
	argupar_parse(&argc, &argv, option, usage, AP_NEED_ARGUMENT);

	struct pathinfo info[] = {
		{ "log",      log_path },
		{ "home",     home_dir },
		{ "dotsav",   dotsav_path },
		{ "proc-id",  pid_path },
		{ "data-dir", data_dir },
		{ "tmp-dir",  tmp_dir },
		{ "exe-dir",  exec_dir },
		{ "locale",   locale_dir },
	};

	int i;
	struct strbuf sb = STRBUF_INIT;

	strbuf_require_cap(&sb, PATH_MAX);
	for_each_idx(i, argc) {
		size_t j;
		const char *name = argv[i];

		for_each_idx(j, sizeof_array(info)) {
			struct pathinfo *q = &info[j];

			if (strcmp(name, q->name))
				continue;

			const char *path = q->func();

			strbuf_concatat_base(&sb, path);
			strbuf_normalize_path(&sb);
			printf("%-10s %s\n", name, sb.str);

			goto next;
		}

		warn("unknown query name `%s'", name);
next:
	}

	NOLEAK(sb);
	return 0;
}

static int cmd_query_limit(int argc, const char **argv)
{
	puts("limit");
	return 0;
}

int cmd_query(int argc, const char **argv)
CMDDESCRIP("Query default information of Savesave")
{
	argupar_subcommand_t subcmd = NULL;

	struct arguopt option[] = {
		APOPT_SUBCOMMAND("path", &subcmd, NULL,
				 cmd_query_path, ARGUOPT_NOUSAGE),
		APOPT_SUBCOMMAND("limit", &subcmd, NULL,
				 cmd_query_limit, ARGUOPT_NOUSAGE),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave query path [-h] <name>",
		"savesave query limit [-h] <name>",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage,
		      AP_COMMAND_MODE | AP_NEED_ARGUMENT);
	return subcmd(argc, argv);
}
