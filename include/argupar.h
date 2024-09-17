/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 *
 * Program argument parser, strategy inspired by git/parse-options
 */

#ifndef ARGUPAR_H
#define ARGUPAR_H

#ifdef __cplusplus
extern "C" {
#endif

enum arguopt_type {
	ARGUOPT_END,		/* end of arguopt */
	ARGUOPT_GROUP,		/* start a new group (visual) */

	ARGUOPT_SUBCOMMAND,	/* subcmd */
	ARGUOPT_CALLBACK,	/* callback */

	ARGUOPT_COUNTUP,	/* count up */
	ARGUOPT_UNSIGNED,	/* unsigned integer */

	ARGUOPT_STRING,		/* string */

	ARGUOPT_TYPEMAX,
};

#define ARGUOPT_NOARG	(1 << 0)	/* no argument */
#define ARGUOPT_OPTARG	(1 << 1)	/* argument is optional */
#define ARGUOPT_HASNEG	(1 << 2)	/* negated version available (no-*) */

struct arguopt {
	enum arguopt_type type;	/* option type */

	int shrtname;		/* short name */
	const char *longname;	/* long name */

	void *value;		/* a pointer that points to the value */

	intptr_t defval;	/* default value of the option (1) */

	const char *argh;	/* a short name of option argument (2) */
	const char *usage;	/* option usages */

	flag_t flag;		/* option flag */

	int (*callback)(const struct arguopt *, const char *);	/* (3) */
	int (*subcmd)(int, const char **);			/* (4) */

	/*
	 * (1)	the way the value be interpreted can vary depending
	 *	on the arguopt type
	 *
	 * (2)	should be wrapped by N_() for translation
	 *
	 * (3) function to use for ARGUOPT_CALLBACK
	 *
	 * (4) function to use for ARGUOPT_SUBCOMMAND
	 */
};

typedef typeof(((struct arguopt *)0)->subcmd) argupar_subcommand_t;

#define ARGUPAR_STOPAT_NONOPT (1 << 0) /* stop at non-option */
#define ARGUPAR_COMMAND_MODE  (1 << 1) /* parse in command mode (oneshot) */

struct argupar {
	int argc;
	const char **argv;

	struct arguopt *option;
	const char *const *usage;

	flag_t flag;

	int outc;
	const char **outv;
};

#define for_each_option(opt) for (; opt->type != ARGUOPT_END; opt++)

typedef struct argupar argupar_t;

void argupar_init(struct argupar *ctx, int argc, const char **argv);

int argupar_parse(struct argupar *ctx, struct arguopt *option,
		  const char *const *usage, flag_t flag);

#define APOPT_END()		\
{				\
	.type = ARGUOPT_END,	\
}

#define APOPT_COUNTUP(s, l, v, h) __APOPT_COUNTUP(s, l, v, h, 0)
#define __APOPT_COUNTUP(s, l, v, h, f)		\
{						\
	.type     = ARGUOPT_COUNTUP,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.usage    = (h),			\
	.flag     = ARGUOPT_NOARG | (f),	\
}

#define APOPT_UNSIGNED(s, l, v, h) __APOPT_UNSIGNED(s, l, v, h, 0)
#define __APOPT_UNSIGNED(s, l, v, h, f)		\
{						\
	.type     = ARGUOPT_UNSIGNED,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.argh     = N_("n"),			\
	.usage    = (h),			\
	.flag     = (f),			\
}

#define APOPT_FILENAME(s, l, v, h) __APOPT_STRING(s, l, v, "path", h, 0)
#define APOPT_STRING(s, l, v, a, h) __APOPT_STRING(s, l, v, a, h, 0)
#define __APOPT_STRING(s, l, v, a, h, f)	\
{						\
	.type     = ARGUOPT_STRING,		\
	.shrtname = (s),			\
	.longname = (l),			\
	.value    = (v),			\
	.argh     = (a),			\
	.usage    = (h),			\
	.flag     = (f),			\
}

#define APOPT_SUBCOMMAND(l, v, c)		\
{						\
	.type     = ARGUOPT_SUBCOMMAND,		\
	.longname = (l),			\
	.value    = (v),			\
	.subcmd   = (c),			\
}

#include "command.h"

#ifdef __cplusplus
}
#endif

#endif /* ARGUPAR_H */
