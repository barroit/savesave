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

	ARGUOPT_SUBCOMMAND,	/* subcommand */
	ARGUOPT_CALLBACK,	/* callback */

	ARGUOPT_COUNTUP,	/* count up */
	ARGUOPT_UNSIGNED,	/* unsigned integer */

	ARGUOPT_STRING,		/* string */
	ARGUOPT_FILE,		/* file name */

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
	int (*subcommand)(int, const char **);			/* (4) */

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

#define ARGUPAR_STOPAT_NONOPT (1 << 0) /* stop at non-option */
#define ARGUPAR_SUBCOMMAND    (1 << 1) /* parse in subcommand mode */

struct argupar {
	int argc;
	const char **argv;

	struct arguopt *option;
	const char *const *usage;

	flag_t flag;

	int outc;
	const char **outv;
};

void argupar_init(struct argupar *ctx, int argc, const char **argv);

int argupar_parse(struct argupar *ctx, struct arguopt *option,
		  const char *const *usage, flag_t flag);

#ifdef __cplusplus
}
#endif

#endif /* ARGUPAR_H */
