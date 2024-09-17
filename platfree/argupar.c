// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma GCC diagnostic ignored "-Wswitch"

#include "argupar.h"
#include "debug.h"
#include "termas.h"
#include "text2num.h"

#define OPTARG_APPLICATOR(type) \
int apply_##type##_optarg(struct arguopt *opt, const char *arg, int unset)

enum arguret {
	AP_ERROR = -1,	/* error */
	AP_SUCCESS = 0,	/* success */
	AP_SHRTHELP,	/* -h */
	AP_LONGHELP,	/* --help */
	AP_DONE,	/* no more options to be parsed */
};

void argupar_init(struct argupar *ctx, int argc, const char **argv)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->argc = argc;
	ctx->argv = argv;
	ctx->outv = argv;
}

static void debug_check_argupar(struct argupar *ctx)
{
	struct arguopt *opt = ctx->option;
	int is_subcmd = ctx->flag & ARGUPAR_SUBCOMMAND;

	while (opt->type != ARGUOPT_END) {
		if (is_subcmd &&
		    opt->type != ARGUOPT_SUBCOMMAND &&
		    opt->type != ARGUOPT_GROUP)
			goto err_incompat_type;

		if ((opt->flag & ARGUOPT_NOARG) &&
		    (opt->flag & ARGUOPT_OPTARG))
			goto err_incompat_argflag;

		if (opt->type == ARGUOPT_STRING || opt->type == ARGUOPT_FILE)
			if ((opt->flag & ARGUOPT_OPTARG) && !opt->defval)
				goto err_missing_defval;

		opt++;
	}

	return;

err_incompat_type:
	bug("parser is set to 'ARGUPAR_SUBCOMMAND' but arguopt `%s` has type `%d`",
	    opt->longname, opt->type);
err_incompat_argflag:
	bug("flag of arguopt `%s` contains an illegal combination of 'ARGUOPT_NOARG' and 'ARGUOPT_OPTARG'",
	    opt->longname);
err_missing_defval:
	bug("arguopt `%s` has flag 'ARGUOPT_OPTARG' but lacks a 'defval'",
	    opt->longname);
}

static OPTARG_APPLICATOR(subcommand)
{
	BUG_ON("not implemented");
}

static OPTARG_APPLICATOR(callback)
{
	BUG_ON("not implemented");
}

static OPTARG_APPLICATOR(countup)
{
	int *p = opt->value;

	if (unset)
		*p = 0;
	else
		*p += 1;

	return 0;
}

static OPTARG_APPLICATOR(unsigned)
{
	if (!unset)
		return str2uint(arg, opt->value);

	*(unsigned *)opt->value = 0;
	return 0;
}

static OPTARG_APPLICATOR(string)
{
	const char **p = opt->value;

	if (unset)
		*p = NULL;
	else if (arg)
		*p = arg;
	else
		arg = (const char *)opt->defval;

	return 0;
}

static OPTARG_APPLICATOR(file)
{
	BUG_ON("not implemented");
}

static int dispatch_optarg(struct arguopt *opt, const char *arg, int unset)
{
	BUG_ON(unset && arg);

	static typeof(dispatch_optarg) *map[ARGUOPT_TYPEMAX] = {
		[ARGUOPT_END]        = CALLBACK_MAP_POISON1,
		[ARGUOPT_GROUP]      = CALLBACK_MAP_POISON2,

		[ARGUOPT_SUBCOMMAND] = apply_subcommand_optarg,
		[ARGUOPT_CALLBACK]   = apply_callback_optarg,

		[ARGUOPT_COUNTUP]    = apply_countup_optarg,
		[ARGUOPT_UNSIGNED]   = apply_unsigned_optarg,

		[ARGUOPT_STRING]     = apply_string_optarg,
		[ARGUOPT_FILE]       = apply_file_optarg,
	};

	int err = map[opt->type](opt, arg, unset);
	if (err)
		return error(_("failed to parse option `%s' with value `%s'"),
			     opt->longname, arg); /* unset never fails */

	return 0;
}

static int do_parse_shrtopt(struct argupar *ctx, const char **next)
{
	struct arguopt *opt = ctx->option;
	const char *str = *next;
	const char *arg;

	for (; opt->type != ARGUOPT_END; opt++) {
		if (opt->shrtname != *str)
			continue;

		*next = str[1] ? &str[1] : NULL;
		if (opt->flag & ARGUOPT_NOARG) {
			arg = NULL;
		} else if (str[1]) {
			arg = &str[1];
			*next = NULL;
		} else if (ctx->argc > 1) {
			ctx->argc--;
			arg = *(++ctx->argv);
		} else if (!(opt->flag & ARGUOPT_OPTARG)) {
			goto err_missing_arg;
		}

		return dispatch_optarg(opt, arg, 0);
	}

	return error("unknown short option `%c'", *str);
err_missing_arg:
	return error(_("short option `%c' requires a value"), *str);
}

static int parse_shrtopt(struct argupar *ctx)
{
	int err;
	const char *next = *ctx->argv;

	while (next != NULL) {
		err = do_parse_shrtopt(ctx, &next);
		if (err)
			return err;
	}

	return 0;
}

struct arguopt_dupopt {
	struct arguopt *opt;
	int unset;
};

static void register_abbrev(struct arguopt *opt, int unset,
			    struct arguopt_dupopt *abbrev,
			    struct arguopt_dupopt *ambiguous)
{
	if (abbrev->opt) {
		ambiguous->opt = abbrev->opt;
		ambiguous->unset = abbrev->unset;
	}

	abbrev->opt = opt;
	abbrev->unset = unset;
}

static int parse_longopt(struct argupar *ctx)
{
	const char *arg;
	const char *argstr = *ctx->argv;
	const char *argsep = strchrnul(argstr, '=');
	const char *argval;

	struct arguopt *opt = ctx->option;
	int arg_unset = strskip(argstr, "no-", &argstr) == 0;
	struct arguopt_dupopt abbrev = { 0 }, ambiguous = { 0 };

	for (; opt->type != ARGUOPT_END; opt++) {
		const char *optname = opt->longname;
		if (!optname || opt->type == ARGUOPT_SUBCOMMAND)
			continue;

		int opt_unset = strskip(optname, "no-", &optname) == 0;

		if (arg_unset != opt_unset && !(opt->flag & ARGUOPT_HASNEG))
			continue;

		if (strskip(argstr, optname, &argval) == 0) {
prepare_optarg:
			if (argval[0] == '=') {
				if (opt->flag & ARGUOPT_NOARG || arg_unset)
					goto err_extra_value;
				arg = &argval[1];
			} else if (*argval) {
				continue;
			} else if ((opt->flag & ARGUOPT_NOARG) || arg_unset) {
				arg = NULL;
			} else if (ctx->argc > 1) {
				ctx->argc--;
				arg = *(++ctx->argv);
			} else if (!(opt->flag & ARGUOPT_OPTARG)) {
				goto err_missing_arg;
			}

			return dispatch_optarg(opt, arg, arg_unset);
		}

		if (strncmp(optname, argstr, argsep - argstr) == 0)
			register_abbrev(opt, arg_unset != opt_unset,
					&abbrev, &ambiguous);

		if ((opt->flag & ARGUOPT_HASNEG) && straftr("no-", argstr))
			register_abbrev(opt, !opt_unset, &abbrev, &ambiguous);
	}

	if (ambiguous.opt)
		goto err_ambiguous_opt;

	if (abbrev.opt) {
		argval = argsep;
		opt = abbrev.opt;
		goto prepare_optarg; /* ah...... what a fucking mess */
	}

	return error(_("unknown long option `%.*s'"),
		     (int)(argsep - argstr), argstr);
err_missing_arg:
	return error(_("long option `%s%s' requires a value"),
		     arg_unset ? "no-" : "", opt->longname);
err_extra_value:
	return error(_("long option `%s%s' takes no value"),
		     arg_unset ? "no-" : "", opt->longname);
err_ambiguous_opt:
	return error(_("long option `%.*s' is ambiguous (could be --%s%s or --%s%s)"),
		     (int)(argsep - argstr), argstr,
		     ambiguous.unset ? "no-" : "", ambiguous.opt->longname,
		     abbrev.unset ? "no-" : "", abbrev.opt->longname);
}

static enum arguret do_parse(struct argupar *ctx)
{
	const char *str = *ctx->argv;

	/* non option (argument) */
	if (str[0] != '-') {
		if (ctx->flag & ARGUPAR_STOPAT_NONOPT) {
			ctx->outv[ctx->outc++] = str;
			return AP_DONE;
		}
		return error(_("unknown argument `%s'"), *ctx->argv);

	/* short option */
	} else if (str[1] != '-') {
		str += 1;

		if (str[0] == 'h' && str[1] == 0)
			return AP_SHRTHELP;

		*ctx->argv = str;
		return parse_shrtopt(ctx);

	/* long option */
	} else {
		str += 2;

		if (str[0] == 0) {
			ctx->argc--;
			ctx->argv++;
			return AP_DONE;
		}

		if (strcmp(str, "help") == 0)
			return AP_LONGHELP;

		*ctx->argv = str;
		return parse_longopt(ctx);
	}
}

int argupar_parse(struct argupar *ctx, struct arguopt *option,
		  const char *const *usage, flag_t flag)
{
	ctx->option = option;
	ctx->usage = usage;
	ctx->flag = flag;

	DEBUG_RUN()
		debug_check_argupar(ctx);

	enum arguret ret;

	while (ctx->argc) {
		ret = do_parse(ctx);
		switch (ret) {
		case AP_SUCCESS:
			break;
		case AP_ERROR:
			exit(128);
		case AP_DONE:
			goto parse_done;
		case AP_SHRTHELP:
			//
		case AP_LONGHELP:
			//
		}

		ctx->argc--;
		ctx->argv++;
	}

parse_done:
	if (ctx->argc)
		memmove(&ctx->outv[ctx->outc], ctx->argv,
			sizeof(*ctx->argv) * ctx->argc);

	int nl = ctx->outc + ctx->argc;
	ctx->outv[nl] = NULL;
	return nl;
}