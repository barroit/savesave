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
	int apply_##type##_optarg(struct arguopt *opt, const char *arg)

enum arguret {
	AP_ERROR = -1,	/* error */
	AP_SUCCESS = 0,	/* success */
	AP_NONOPT,	/* non-option (argument) */
	AP_UNKOPT,	/* unknown option */
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
	*(int *)opt->value += 1;
	return 0;
}

static OPTARG_APPLICATOR(unsigned)
{
	return str2uint(arg, opt->value);
}

static OPTARG_APPLICATOR(string)
{
	*(const char **)opt->value = arg ? arg : (const char *)opt->defval;
	return 0;
}

static OPTARG_APPLICATOR(file)
{
	BUG_ON("not implemented");
}

static int apply_optarg(struct arguopt *opt, const char *arg)
{
	static typeof(apply_optarg) *map[ARGUOPT_TYPEMAX] = {
		[ARGUOPT_END]        = CALLBACK_MAP_POISON1,
		[ARGUOPT_GROUP]      = CALLBACK_MAP_POISON2,

		[ARGUOPT_SUBCOMMAND] = apply_subcommand_optarg,
		[ARGUOPT_CALLBACK]   = apply_callback_optarg,

		[ARGUOPT_COUNTUP]    = apply_countup_optarg,
		[ARGUOPT_UNSIGNED]   = apply_unsigned_optarg,

		[ARGUOPT_STRING]     = apply_string_optarg,
		[ARGUOPT_FILE]       = apply_file_optarg,
	};

	return map[opt->type](opt, arg);
}

static int do_parse_shrtopt(struct argupar *ctx, const char **next)
{
	const char *str = *next;
	const char *arg = NULL;
	struct arguopt *opt = ctx->option;

	while (opt->type != ARGUOPT_END) {
		if (opt->shrtname != *str) {
			opt++;
			continue;
		}

		if (opt->flag & ARGUOPT_NOARG) {
			*next = str[1] ? &str[1] : NULL;
		} else if (str[1]) {
			arg = &str[1];
		} else if (ctx->argc) {
			ctx->argc--;
			arg = *(++ctx->argv);
		} else if (!(opt->flag & ARGUOPT_OPTARG)) {
			goto err_missing_arg;
		}

		return apply_optarg(opt, arg);
	}

	return AP_UNKOPT;

err_missing_arg:
	return error(_("option `%c' requires a value"), *str);

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
	int uset;
};

static void register_abbrev(struct arguopt *opt, int uset,
			    struct arguopt_dupopt *abbrev,
			    struct arguopt_dupopt *ambiguous)
{
	if (abbrev->opt && abbrev->uset != uset) {
		ambiguous->opt = abbrev->opt;
		ambiguous->uset = abbrev->uset;
	}

	abbrev->opt = opt;
	abbrev->uset = uset;
}

static int parse_longopt(struct argupar *ctx)
{
	struct arguopt *opt = ctx->option;
	const char *argstr = *ctx->argv;
	const char *arg = NULL;
	int is_arguset = strskip(argstr, "no-", &argstr) == 0;
	struct arguopt_dupopt abbrev, ambiguous;

	while (opt->type != ARGUOPT_END) {
		if (!opt->longname || opt->type == ARGUOPT_SUBCOMMAND)
			continue;

		const char *optname = opt->longname;
		const char *argval;
		int is_optuset = strskip(optname, "no-", &optname) == 0;
		int is_noarg = opt->flag & ARGUOPT_NOARG;
		int is_useton = opt->flag & ARGUOPT_HASNEG;

		if (is_arguset != is_optuset && !is_useton)
			continue;

		if (strskip(argstr, optname, &argval) == 0) {
prepare_optarg:
			if (argval[0] == '=') {
				if (is_noarg)
					goto err_extra_value;
				arg = &argval[1];
			} else if (*argval) {
				continue;
			} else if (is_noarg) {
				return apply_optarg(opt, NULL);
			} else if (ctx->argc) {
				ctx->argc--;
				arg = *(++ctx->argv);
			} else if (!(opt->flag & ARGUOPT_OPTARG)) {
				goto err_missing_arg;
			}

			if (is_arguset && arg)
				goto err_extra_value;

			return apply_optarg(opt, arg);
		}

		if (strncmp(optname, argstr, argval - argstr) == 0)
			register_abbrev(opt, is_arguset != is_optuset,
					&abbrev, &ambiguous);

		if (is_useton && straftr("no-", argstr))
			register_abbrev(opt, is_optuset,
					&abbrev, &ambiguous);
	}

	if (ambiguous.opt)
		goto err_ambiguous_opt;

	if (abbrev.opt)
		goto prepare_optarg;

	return 0;

err_missing_arg:
	return error(_("option `%s%s' requires a value"),
		     is_arguset ? "no-" : "", opt->longname);
err_extra_value:
	return error(_("option `%s%s' takes no value"),
		     is_arguset ? "no-" : "", opt->longname);
err_ambiguous_opt:
	return error(_("arguopt `%s' is ambiguous (could be --%s%s or --%s%s)"),
		     opt->longname,
		     ambiguous.uset ? "no-" : "", ambiguous.opt->longname,
		     abbrev.uset ? "no-" : "", abbrev.opt->longname);
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

		return AP_NONOPT;

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

	while (39) {
		ret = do_parse(ctx);
		switch (ret) {
		case AP_SUCCESS:
			break;
		case AP_NONOPT:
			error(_("unknown argument `%s'"), *ctx->argv);
			exit(128);
		case AP_SHRTHELP:
			//
		case AP_LONGHELP:
			//
		case AP_DONE:
			goto parse_done;
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
