// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "argupar.h"
#include "termas.h"
#include "strlist.h"
#include "strbuf.h"
#include "list.h"
#include "iter.h"
#include "alloc.h"

#define OPTMAS_INDENT_OPT  "    "
#define OPTMAS_INDENT_CMD  "   "

#define OPTMAS_PADOPT 26
#define OPTMAS_PADCMD 16

#define OPTARG_APPLICATOR(type)					\
	int apply_##type##_optarg(struct arguopt *opt,		\
				  const char *arg, int unset)

enum arguret {
	AP_ERROR   = -1,	/* equals to error() */
	AP_SUCCESS = 0,		/* success */
	AP_DONE    = 39,	/* no more options to be parsed */
};

struct cmdmode {
	int val;		/* value */
	int *addr;		/* address of variable that stores value */

	struct arguopt *opt;	/* arguopt reference */
	flag_t flags;		/* flags of arguopt */

	struct list_head list;
};

struct argupar {
	int argc;
	const char **argv;

	struct arguopt *option;
	const char **usage;

	flag_t flags;

	int outc;
	const char **outv;

	struct list_head cml;	/* cmdmode list */
};

static void assert_flags(struct argupar *ctx)
{
	BUG_ON(!ctx->usage);

	if (ctx->flags & AP_COMMAND_MODE)
		BUG_ON(ctx->flags &
		       (AP_STOPAT_NONOPT | AP_NO_ARGUMENT));

	BUG_ON(ctx->flags & AP_NO_ARGUMENT && (ctx->flags & AP_NEED_ARGUMENT));

	struct arguopt *opt = ctx->option;
	for_each_option(opt) {
		if (opt->type == ARGUOPT_GROUP)
			continue;

		if (ctx->flags & AP_COMMAND_MODE)
			continue;

		BUG_ON((opt->flags & ARGUOPT_NOARG) &&
		       (opt->flags & ARGUOPT_OPTARG));

		BUG_ON(opt->type == ARGUOPT_STRING &&
		       (opt->flags & ARGUOPT_OPTARG) && !opt->defval);
	}
}

static const char *cmdpath(const char *name)
{
	static struct strbuf sb = STRBUF_INIT;
	static uint cnt;

	if (!name)
		goto out;

	if (cnt++) {
		strbuf_concat(&sb, " ");
		strbuf_concat(&sb, name);
	} else {
		strbuf_concat(&sb, "savesave");
	}

out:
	return sb.str;
}

static enum arguret parse_subcommand(struct arguopt *opt, const char *cmd)
{
	for_each_option(opt) {
		if (opt->type != ARGUOPT_SUBCOMMAND)
			continue;

		if (strcmp(opt->longname, cmd))
			continue;

		*(subcmd_t *)opt->value = opt->subcmd;
		return AP_DONE;
	}

	return error(_("unknown command `%s', see '%s -h'"),
		     cmd, cmdpath(NULL));
}

#define OPT_SHRT (1 << 0)
#define OPT_LONG (1 << 1)
#define OPT_USET (1 << 2)

static char *pretty_optname(struct arguopt *opt, flag_t flags)
{
	struct strbuf sb = STRBUF_INIT;

	/*
	 * Short options never have a negated version.
	 */
	if (flags & OPT_SHRT) {
		strbuf_printf(&sb, "-%c", opt->shrtname);
		goto out;
	}

	strbuf_printf(&sb, "--%s%s",
		      flags & OPT_USET ? "no-" : "", opt->longname);

out:
	return sb.str;
}

static OPTARG_APPLICATOR(switch)
{
	int *p = opt->value;

	if (unset)
		*p = 0;
	else
		*p = 1;

	return 0;
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

static OPTARG_APPLICATOR(uint)
{
	if (!unset)
		return str2uint(arg, opt->value);

	*(uint *)opt->value = 0;
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
		*p = (const char *)opt->defval;

	return 0;
}

static OPTARG_APPLICATOR(cmdmode)
{
	*(int *)opt->value = opt->defval;
	return 0;
}

static int check_cmdmode_collision(struct arguopt *opt,
				   flag_t flags, struct list_head *cml)
{
	struct cmdmode *cm;
	list_for_each_entry(cm, cml, list) {
		if (*cm->addr == cm->val || cm->opt == opt)
			continue;

		if (cm->opt)
			break;

		cm->opt = opt;
		cm->flags = flags;
	}

	if (list_entry_is_head(cm, cml, list))
		return 0;

	char *n1 = pretty_optname(cm->opt, cm->flags);
	char *n2 = pretty_optname(opt, flags);

	error(_("options '%s' and '%s' cannot be used together"), n1, n2);

	free(n1);
	free(n2);
	return -1;
}

static int dispatch_optarg(struct arguopt *opt, const char *arg,
			   flag_t flags, struct list_head *cml)
{
	BUG_ON(flags & OPT_USET && arg);

	static typeof(apply_switch_optarg) *map[ARGUOPT_TYPEMAX] = {
		[ARGUOPT_SWITCH]     = apply_switch_optarg,
		[ARGUOPT_COUNTUP]    = apply_countup_optarg,
		[ARGUOPT_UINT]       = apply_uint_optarg,

		[ARGUOPT_STRING]     = apply_string_optarg,

		[ARGUOPT_CMDMODE]    = apply_cmdmode_optarg,
	};

	int err = map[opt->type](opt, arg, flags & OPT_USET);
	if (err)
		return error(_("failed to parse option `%s' with value `%s'"),
			     opt->longname, arg); /* unset never fails */

	if (opt->type != ARGUOPT_CMDMODE)
		return 0;

	return check_cmdmode_collision(opt, flags, cml);
}

static int do_parse_shrtopt(struct argupar *ctx, const char **next)
{
	struct arguopt *opt = ctx->option;
	const char *str = *next;
	const char *arg = ACCESS_POISON;

	for_each_option(opt) {
		if (opt->shrtname != *str)
			continue;

		*next = str[1] ? &str[1] : NULL;
		if (opt->flags & ARGUOPT_NOARG) {
			arg = NULL;
		} else if (str[1]) {
			arg = &str[1];
			*next = NULL;
		} else if (ctx->argc > 1) {
			ctx->argc--;
			arg = *(++ctx->argv);
		} else if (!(opt->flags & ARGUOPT_OPTARG)) {
			goto err_missing_arg;
		}

		return dispatch_optarg(opt, arg, OPT_SHRT, &ctx->cml);
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
	const char *arg = ACCESS_POISON;
	const char *argstr = *ctx->argv;
	const char *argsep = strchrnul(argstr, '=');
	const char *argval;

	struct arguopt *opt = ctx->option;
	int arg_unset = strskip(argstr, "no-", &argstr) == 0;
	struct arguopt_dupopt abbrev = { 0 }, ambiguous = { 0 };

	for_each_option(opt) {
		const char *optname = opt->longname;

		if (!optname ||
		    opt->type == ARGUOPT_SUBCOMMAND ||
		    opt->type == ARGUOPT_MEMBINFO)
			continue;

		int opt_unset = strskip(optname, "no-", &optname) == 0;

		if (arg_unset != opt_unset && !(opt->flags & ARGUOPT_HASNEG))
			continue;

		if (strskip(argstr, optname, &argval) == 0) {
prepare_optarg:
			if (argval[0] == '=') {
				if (opt->flags & ARGUOPT_NOARG || arg_unset)
					goto err_extra_value;
				arg = &argval[1];
			} else if (*argval) {
				continue;
			} else if ((opt->flags & ARGUOPT_NOARG) || arg_unset) {
				arg = NULL;
			} else if (ctx->argc > 1) {
				ctx->argc--;
				arg = *(++ctx->argv);
			} else if (!(opt->flags & ARGUOPT_OPTARG)) {
				goto err_missing_arg;
			}

			flag_t flags = (arg_unset ? OPT_USET : 0) | OPT_LONG;
			return dispatch_optarg(opt, arg, flags, &ctx->cml);
		}

		if (strncmp(optname, argstr, argsep - argstr) == 0)
			register_abbrev(opt, arg_unset != opt_unset,
					&abbrev, &ambiguous);

		if ((opt->flags & ARGUOPT_HASNEG) && straftr("no-", argstr))
			register_abbrev(opt, !opt_unset, &abbrev, &ambiguous);
	}

	if (ambiguous.opt)
		goto err_ambiguous_opt;

	if (abbrev.opt) {
		argval = argsep;
		opt = abbrev.opt;
		arg_unset = abbrev.unset;
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

static void print_command_usage(const char *const *usage, struct strlist *sl)
{
	const char *pref = "usage: ";

	while (*usage != NULL) {
		const char *str = *usage;
		const char *mas = strnxtws(str);

		mas = strskipws(mas);
		int namlen = mas - str;
		int preflen = strlen("usage: ");

		strlist_split_word(sl, mas, 80 - (preflen + namlen));

		printf("%*s%.*s", preflen, pref, namlen, str);
		puts(strlist_at(sl, 0));

		if (sl->size == 1)
			goto next;

		uint i = 1;
		int pad = preflen + namlen;

		for_each_idx_from(i, sl->size)
			printf("%*s%s\n", pad, "", strlist_at(sl, i));

next:
		strlist_cleanup(sl);
		usage++;
		pref = "or: ";
	}
}

int argupar_opt_mas_pad;

static size_t print_option_usage(struct arguopt *option, struct strlist *sl)
{
	struct arguopt *opt = option;
	size_t cnt = 0;

	for_each_option(opt) {
		cnt++;
		if (opt->type == ARGUOPT_GROUP) {
			size_t i = cnt - 1;

			if (i && option[i - 1].type != ARGUOPT_GROUP)
				putchar('\n');

			if (opt->longname)
				puts(opt->longname);

			continue;
		}

		const char *fmt;
		int nr;
		int is_subcmd = opt->type == ARGUOPT_SUBCOMMAND;
		int pad;

		if (is_subcmd) {
			nr = strlen(OPTMAS_INDENT_CMD);
			fputs(OPTMAS_INDENT_CMD, stdout);

			nr += strlen(opt->longname);
			fputs(opt->longname, stdout);

			goto print_option_usage;
		} else {
			nr = strlen(OPTMAS_INDENT_OPT);
			fputs(OPTMAS_INDENT_OPT, stdout);
		}

		if (opt->shrtname)
			nr += printf("-%c, ", opt->shrtname);

		if (opt->type == ARGUOPT_MEMBINFO)
			fmt = "%s";
		else
			fmt = (opt->flags & ARGUOPT_HASNEG) ? "--[no-]%s" :
							     "--%s";
		nr += printf(fmt, opt->longname);

		if (!opt->argh)
			goto print_option_usage;

		int nobrac = strpbrk(opt->argh, "()<>[]|") != NULL;
		if (opt->flags & ARGUOPT_OPTARG)
			fmt = nobrac ? "[=%s]" : "[=<%s>]";
		else
			fmt = nobrac ? " %s" : " <%s>";

		nr += printf(fmt, opt->argh);

print_option_usage:
		if (argupar_opt_mas_pad)
			pad = argupar_opt_mas_pad;
		else
			pad = is_subcmd ? OPTMAS_PADCMD : OPTMAS_PADOPT;

		if (nr >= pad) {
			putchar('\n');
			nr = 0;
		}

		strlist_split_word(sl, _(opt->usage), 80 - pad);
		printf("%*s%s\n", pad - nr, "", strlist_at(sl, 0));

		if (sl->size == 1)
			goto next;

		uint i = 1;
		for_each_idx_from(i, sl->size)
			printf("%*s%s\n", pad, "", strlist_at(sl, i));

next:
		strlist_cleanup(sl);
	}

	return cnt;
}

void __cold prompt_shrt_help(const char **usage, struct arguopt *option)
{
	struct strlist sl = STRLIST_INIT;

	print_command_usage(usage, &sl);
	putchar('\n');

	size_t cnt = print_option_usage(option, &sl);
	if (cnt)
		putchar('\n');

	exit(128);
}

static enum arguret do_parse(struct argupar *ctx)
{
	const char *str = *ctx->argv;

	/* non option (argument) */
	if (str[0] != '-') {
		if (ctx->flags & AP_STOPAT_NONOPT)
			return AP_DONE;
		else if (ctx->flags & AP_COMMAND_MODE)
			return parse_subcommand(ctx->option, str);

		ctx->outv[ctx->outc++] = str;
		return AP_SUCCESS;

	/* short option */
	} else if (str[1] != '-') {
		str += 1;

		if (str[0] == 'h' && str[1] == 0)
			prompt_shrt_help(ctx->usage, ctx->option);

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
			prompt_shrt_help(ctx->usage, ctx->option);

		*ctx->argv = str;
		return parse_longopt(ctx);
	}
}

static void create_cmdmode_list(struct argupar *ctx, struct arguopt *option)
{
	for_each_option(option) {
		if (option->type != ARGUOPT_CMDMODE)
			continue;

		struct cmdmode *cm = xcalloc(sizeof(*cm), 1);
		list_add(&cm->list, &ctx->cml);

		cm->val = *(int *)option->value;
		cm->addr = option->value;
	}
}

static void cleanup_cmdmode_list(struct argupar *ctx)
{
	struct cmdmode *cm, *tmp;
	list_for_each_entry_safe(cm, tmp, &ctx->cml, list) {
		list_del(&cm->list);
		free(cm);
	}
}

void argupar_parse(int *argc, const char ***argv,
		   struct arguopt *option, const char **usage, flag_t flags)
{
	int narg = *argc - 1;
	struct argupar ctx = {
		.argc = narg,
		.argv = *argv + 1,
		.outv = ctx.argv,

		.option = option,
		.usage = usage,
		.flags = flags,

		.cml = LIST_HEAD_INIT(ctx.cml),
	};

	assert_flags(&ctx);
	cmdpath((*argv)[0]);

	if (!ctx.argc) {
		if (!(flags & AP_NEED_ARGUMENT))
			goto parse_done;
		goto help_no_arg;
	}

	create_cmdmode_list(&ctx, option);
	while (ctx.argc) {
		enum arguret ret = do_parse(&ctx);
		switch (ret) {
		case AP_SUCCESS:
			break;
		case AP_ERROR:
			exit(128);
		case AP_DONE:
			goto parse_done;
		}

		ctx.argc--;
		ctx.argv++;
	}

parse_done:
	if (!list_is_empty(&ctx.cml))
		cleanup_cmdmode_list(&ctx);

	if (ctx.argc)
		memmove(&ctx.outv[ctx.outc], ctx.argv,
			st_umult(sizeof(*ctx.argv), ctx.argc));

	int n = ctx.outc + ctx.argc;
	if (flags & AP_NEED_ARGUMENT && !n) {
		/*
		 * We do have some options, but no argument found
		 */
		if (narg)
			error(_("command '%s' requires an argument\n"),
			      cmdpath(NULL));
help_no_arg:
		prompt_shrt_help(ctx.usage, ctx.option);
	}

	if (flags & AP_NO_ARGUMENT && n)
		die(_("command '%s' takes no argument"), cmdpath(NULL));

	ctx.outv[n] = NULL;
	*argv = ctx.outv;
	*argc = n;
}
