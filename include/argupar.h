/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 *
 * Program argument parser, strategy inspired by git/parse-options
 */

#ifndef ARGUPAR_H
#define ARGUPAR_H

enum arguopt_type {
	ARGUOPT_SWITCH,		/* switch */
	ARGUOPT_COUNTUP,	/* count up */
	ARGUOPT_UINT,		/* unsigned integer */

	ARGUOPT_STRING,		/* string */

	ARGUOPT_CMDMODE,	/* command mode */

	ARGUOPT_TYPEMAX,

	ARGUOPT_END,		/* end of arguopt */
	ARGUOPT_GROUP,		/* start a new group (visual) */

	ARGUOPT_SUBCOMMAND,	/* subcmd */

	ARGUOPT_MEMBINFO,	/* member information (print usage) */
};

#define ARGUOPT_NOARG   (1 << 0) /* no argument */
#define ARGUOPT_OPTARG  (1 << 1) /* argument is optional */
#define ARGUOPT_HASNEG  (1 << 2) /* negated version available (no-*) */

struct arguopt {
	enum arguopt_type type;	/* option type */

	int shrtname;		/* short name */
	const char *longname;	/* long name */

	void *value;		/* a pointer that points to the value */

	intptr_t defval;	/* default value of the option, the way the
				   value be interpreted can vary, depending
				   on the arguopt type */

	const char *argh;	/* a short name of option argument, should be
				   wrapped by N_() for translation */

	const char *usage;	/* option usages */

	flag_t flags;		/* option flags */

	int (*callback)(const struct arguopt *, const char *);
				/* function to use for ARGUOPT_CALLBACK */

	int (*subcmd)(int, const char **);
				/* function to use for ARGUOPT_SUBCOMMAND */
};

typedef typeof(((struct arguopt *)0)->subcmd) subcmd_t;

#define AP_STOPAT_NONOPT (1 << 0) /* stop at non-option */
#define AP_COMMAND_MODE  (1 << 1) /* stop *after* command */
#define AP_NEED_ARGUMENT (1 << 2) /* need argument (not option!) */
#define AP_NO_ARGUMENT   (1 << 3) /* takes no argument */

#define for_each_option(opt) for (; opt->type != ARGUOPT_END; opt++)

extern int argupar_opt_mas_pad;

void __noreturn prompt_shrt_help(const char **usage, struct arguopt *option);

void argupar_parse(int *argc, const char ***argv,
		   struct arguopt *option, const char **usage, flag_t flags);

#define APOPT_END()		\
{				\
	.type = ARGUOPT_END,	\
}

#define APOPT_GROUP(l)		\
{				\
	.type = ARGUOPT_GROUP,	\
	.longname = (l),	\
}

#define APOPT_COUNTUP(s, l, v, h) __APOPT_COUNTUP(s, l, v, h, 0)
#define __APOPT_COUNTUP(s, l, v, h, f)		\
{						\
	.type     = ARGUOPT_COUNTUP,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.usage    = (h),			\
	.flags     = ARGUOPT_NOARG | (f),	\
}

#define APOPT_SWITCH(s, l, v, h) __APOPT_SWITCH(s, l, v, h, 0)
#define __APOPT_SWITCH(s, l, v, h, f)		\
{						\
	.type     = ARGUOPT_SWITCH,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.usage    = (h),			\
	.flags     = ARGUOPT_NOARG | (f),	\
}

#define APOPT_UINT(s, l, v, h) __APOPT_UINT(s, l, v, h, 0)
#define __APOPT_UINT(s, l, v, h, f)		\
{						\
	.type     = ARGUOPT_UINT,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.argh     = "n",			\
	.usage    = (h),			\
	.flags     = (f),			\
}

#define APOPT_FILENAME(s, l, v, h) __APOPT_STRING(s, l, v, "path", h, 0)
#define APOPT_STRING(s, l, v, a, h) __APOPT_STRING(s, l, v, a, h, 0)
#define __APOPT_STRING(s, l, v, a, h, f) ___APOPT_STRING(s, l, v, 0, a, h, f)
#define ___APOPT_STRING(s, l, v, d, a, h, f)	\
{						\
	.type     = ARGUOPT_STRING,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.defval   = (intptr_t)(d),		\
	.argh     = (a),			\
	.usage    = (h),			\
	.flags     = (f),			\
}

#define APOPT_SUBCMD(l, v, h, c, f)		\
{						\
	.type     = ARGUOPT_SUBCOMMAND,		\
	.longname = (l),			\
	.value    = (v),			\
	.usage    = (h),			\
	.subcmd   = (c),			\
	.flags     = (f),			\
}

#define APOPT_MEMBINFO(l, h)			\
{						\
	.type     = ARGUOPT_MEMBINFO,		\
	.longname = (l),			\
	.usage    = (h),			\
}

#define APOPT_CMDMODE(s, l, v, d, h) __APOPT_CMDMODE(s, l, v, d, h, 0)
#define __APOPT_CMDMODE(s, l, v, d, h, f)	\
{						\
	.type     = ARGUOPT_CMDMODE,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.defval   = (d),			\
	.usage    = (h),			\
	.flags     = ARGUOPT_NOARG | (f),	\
}

#include "command.h"

#endif /* ARGUPAR_H */
