// SPDX-License-Identifier: GPL-3.0
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "compat/getopt.h"
#include "termsg.h"
#include "list.h"
#include "alloc.h"

#ifdef __linux
# define HELP_MESSAGE HELP_MESSAGE_LINUX
#else
# define HELP_MESSAGE HELP_MESSAGE_WINDOWS
#endif

#define OPT(n, ha, f, v) \
	{ .name = (n), .has_arg = (ha), .flag = (f), .val = (v) }

static struct option options[] = {
	OPT("version", no_argument, NULL, OPT_VERSION),
	OPT("help",    no_argument, NULL, OPT_HELP),
	OPT("config",  required_argument, NULL, OPT_CONFIG),
	OPT("stdout",  required_argument, NULL, OPT_STDOUT),
	OPT("stderr",  required_argument, NULL, OPT_STDERR),
	OPT(0, 0, 0, 0),
};

struct posarg {
	char *buf;
	size_t len;
	size_t cap;
};

static const char *aliases = "f:";

static void get_optarg(const char **key)
{
	if (*optarg == 0)
		die("empty string is not allowed");

	*key = optarg;
}

static void push_arg(const char *arg, struct posarg *args)
{
	size_t len;

	if (!*arg) {
		len = strlen("‘none’, ");

		CAP_GROW(&args->buf, args->len + len, &args->cap);
		memcpy(args->buf + args->len, "‘none’, ", len);
	} else {
		len = strlen(arg) + strlen("‘’, ") + 1;

		CAP_GROW(&args->buf, args->len + len, &args->cap);
		snprintf(args->buf + args->len, len, "‘%s’, ", arg);
		len -= 1; /* no nul-term */
	}

	args->len += len;
}

static void report_positional_argument(int idx, int argc, char *const *argv)
{
	struct posarg args = { 0 };
	int npos = (argc - 1) - idx;

	CAP_GROW(&args.buf, npos * 10, &args.cap);
	for_each_idx_from(idx, argc)
		push_arg(argv[idx], &args);

	args.buf[args.len - 2] = 0;
	error("positional argument%s %s %s not allowd",
	      npos > 1 ? "s" : "", args.buf, npos > 1 ? "are" : "is");

	free(args.buf);
}

int parse_option(int argc, char *const *argv, struct cmdarg *args)
{
	int optidx = 0;
	enum optid opt;

	while (39) {
		opt = getopt_long(argc, argv, aliases, options, &optidx);
		switch (opt) {
		case OPT_CONFIG:
			get_optarg(&args->confpath);
			break;
		case OPT_STDOUT:
			get_optarg(&args->fout);
			break;
		case OPT_STDERR:
			get_optarg(&args->ferr);
			break;
		case OPT_UNKNOWN:
			/* error message already printed out */
			return 1;
		case OPT_VERSION:
			puts(APPNAME "-" SAVESAVE_VERSION);
			return 1;
		case OPT_HELP:
			puts(HELP_MESSAGE);
			return 1;
		case -1:
			goto finish;
		case OPT_FLAG:
			/* make gcc happy */
		}
	}

finish:
	if (optidx < argc) {
		report_positional_argument(optidx, argc, argv);
		return 1;
	}

	return 0;
}
