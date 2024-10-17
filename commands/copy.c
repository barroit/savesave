// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "argupar.h"
#include "termas.h"
#include "fileiter.h"
#include "mkdir.h"
#include "strbuf.h"
#include "alloc.h"
#include "maincmd.h"
#include "dotsav.h"
#include "noleak.h"
#include "list.h"
#include "cpsched.h"
#include "fcpy.h"
#include "path.h"

enum cpymode {
	CPY_DEFAULT,	/* appropriate syscall */
	CPY_THREADED,	/* multithreaded scheduler */
	CPY_ASYNCED,	/* asynchronous io */
};

struct cpytsk {
	const char *src;
	char *dest;
	struct list_head list;
};

static uint thrd_numb = -1;
static int force_exec;
static enum cpymode copy_mode;

static LIST_HEAD(tskl);

static void sav2argv(const char *name, int *argc, const char ***argv)
{
	struct savesave *sav;
	retrieve_dotsav(&sav);

	for_each_sav(sav)
		if (strcmp(sav->name, name) == 0)
			break;

	if (sav_is_last(sav))
		die(_("no matching sav `%s' found in `%s'"),
		    name, userspec.dotsav_path);

	/*
	 * -1 for removing the trailing dot
	 */
	size_t len = strlen(sav->backup_prefix) - 1;
	char *dest = xmalloc(len + 1);

	memcpy(dest, sav->backup_prefix, len);
	dest[len] = 0;

	*argc = 2;
	*argv = xcalloc(sizeof(*argv), 3);

	(*argv)[0] = sav->save_prefix;
	(*argv)[1] = dest;
	(*argv)[2] = NULL;

	NOLEAK(*argv);
}

static void assert_file_not_same(const char *src, const char *dest)
{
	int err;
	const char *rest;

	if (strcmp(src, dest) == 0)
		goto err;

	err = strskip(src, dest, &rest);
	if (!err && issep(*rest) && rest[1] == 0)
		goto err;

	err = strskip(dest, src, &rest);
	if (!err && issep(*rest) && rest[1] == 0)
		goto err;

	return;

err:
	die(_("`%s' and `%s' are the same file"), src, dest);
}

static void argv2task(int argc, const char **argv)
{
	size_t i;
	struct strbuf sb = strbuf_from2(argv[argc - 1], 0, PATH_MAX);

	argc -= 1;
	for_each_idx(i, argc) {
		assert_file_not_same(sb.str, argv[i]);

		struct cpytsk *tsk = xmalloc(sizeof(*tsk));
		tsk->src = argv[i];

		char *tmp = xstrdup(tsk->src);
		const char *name = basename(tmp);

		strbuf_concat_basename(&sb, name);
		tsk->dest = xstrdup(sb.str);

		struct cpytsk *ct;
		list_for_each_entry(ct, &tskl, list) {
			if (strcmp(ct->dest, tsk->dest) != 0)
				continue;

			die(_("name has collisions at `%s' and `%s'"),
			    tsk->src, ct->src);
		}

		list_add(&tsk->list, &tskl);

		free(tmp);
		strbuf_reset(&sb);
	}

	strbuf_destroy(&sb);
}

static void prepare_dest_dir(const char *name)
{
	int err;
	char *dir;

	err = access(name, F_OK);
	if (err)
		goto mkdir;

	if (!force_exec)
		die(_("dest `%s' already exists"), name);

	err = flexremove(name);
	if (err)
		die(_("cannot remove dest `%s'"), name);

mkdir:
	dir = xstrdup(name);
	err = mkfdirp(dir);
	free(dir);

	if (err)
		die_errno(ERRMAS_CREAT_DIR(name));
}

static inline int __mkdir(const char *name)
{
	int ret = MKDIR(name);

	if (unlikely(ret))
		warn_errno(ERRMAS_CREAT_DIR(name));

	return ret;
}

static int def_copy_routine(struct iterfile *src, void *data)
{
	struct strbuf *dest = data;
	strbuf_concatat_base(dest, src->dymname);

	if (likely(S_ISREG(src->st.st_mode)))
		return copy_regfile(src->absname, dest->str);
	else if (S_ISDIR(src->st.st_mode))
		return __mkdir(dest->str);
	else
		return copy_symlink(src->absname, dest->str);
}

static void def_copy(void)
{
	struct cpytsk *ct, *tmp;
	struct strbuf sb = STRBUF_INIT;

	list_for_each_entry_safe(ct, tmp, &tskl, list) {
		prepare_dest_dir(ct->dest);
		strbuf_reset_from(&sb, ct->dest);

		int err = fileiter(ct->src, def_copy_routine,
				   &sb, FITER_LIST_DIR);
		if (err)
			die(ERRMAS_COPY_FILE(ct->src, ct->dest));

		list_del(&ct->list);
		free(ct->dest);
		free(ct);
	}

	strbuf_destroy(&sb);
}

static int __mt_cpy_reg(struct iterfile *src, void *data)
{
	struct strbuf *dest = &((struct strbuf *)data)[0];
	strbuf_concatat_base(dest, src->dymname);

	int ret = cpsched_schedule(src->absname, dest->str);
	if (ret != SCHED_BUSY)
		return ret;

	return copy_regfile(src->absname, dest->str);
}

int mt_copy_routine(const char *src, struct strbuf *sb)
{
	int iter_err;
	int thrd_err = 0;

	iter_err = fileiter(src, __fiter_cpy_nonreg, sb, __FITER_CPY_NONREG);
	if (iter_err)
		goto out;

	iter_err = fileiter(src, __mt_cpy_reg, sb, FITER_NO_SYMLINK);
	cpsched_join(&thrd_err);

out:
	if (iter_err || thrd_err)
		return error(ERRMAS_COPY_FILE(src, sb->str));

	return 0;
}

static void mt_copy(void)
{
	struct cpytsk *ct, *tmp;
	struct strbuf sb = STRBUF_INIT;
	uint cores = getcpucores();

	if (thrd_numb == 0)
		die(_("thread number can't be zero"));

	if (thrd_numb == -1)
		thrd_numb = cores;
	else if (thrd_numb > cores * 2)
		warn(_("too many threads specified"));

	cpsched_deploy(thrd_numb);
	list_for_each_entry_safe(ct, tmp, &tskl, list) {
		prepare_dest_dir(ct->dest);
		strbuf_reset_from(&sb, ct->dest);

		int err = mt_copy_routine(ct->src, &sb);
		if (err)
			die(ERRMAS_COPY_FILE(ct->src, ct->dest));

		list_del(&ct->list);
		free(ct->dest);
		free(ct);
	}

	strbuf_destroy(&sb);
}

static void aio_copy(void)
{
}

int cmd_copy(int argc, const char **argv)
CMDDESCRIP("Copy a file")
{
	struct arguopt option[] = {
		APOPT_COUNTUP('f', "force", &force_exec,
			      N_("overwrite existing one")),

		APOPT_GROUP("List of file copying methods"),
		APOPT_CMDMODE(0, "threaded", &copy_mode, CPY_THREADED,
			      N_("copy files using multiple threads")),
		APOPT_CMDMODE(0, "asynced", &copy_mode, CPY_ASYNCED,
			      N_("copy files using asynchronous io")),

		APOPT_GROUP("Options for multithreading"),
		APOPT_UINT('n', "thread", &thrd_numb,
			   N_("number of threads to handle copy operations")),

		APOPT_END(),
	};
	const char *usage[] = {
		"savesave copy [<method>] [options] <source...> <dest>",
		"savesave copy [<method>] [options] <sav>",
		NULL,
	};

	argupar_parse(&argc, &argv, option, usage, AP_NEED_ARGUMENT);

	if (argc == 1)
		sav2argv(argv[0], &argc, &argv);
	argv2task(argc, argv);

	typeof(def_copy) *funcmap[] = {
		[CPY_DEFAULT]  = def_copy,
		[CPY_THREADED] = mt_copy,
		[CPY_ASYNCED]  = aio_copy,
	};

	funcmap[copy_mode]();
	return 0;
}
