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
#include "dotsav.h"
#include "noleak.h"
#include "list.h"
#include "iter.h"
#include "fcpy.h"
#include "acpy.h"
#include "path.h"
#include "cls.h"

enum cpymode {
	CPY_DEFAULT,	/* appropriate syscall */
	CPY_ASYNCED,	/* asynchronous io */
};

struct cpytsk {
	const char *src;
	char *dest;

	mode_t mode;	/* st_mode */

	struct list_head list;
};

uint aio_qs = 1U << 5;		/* queue size */
size_t aio_bs = 1U << 15;	/* block size */

static int force_exec;
static enum cpymode copy_mode;

static LIST_HEAD(tskl);

static void cleanup_cpytsk(void)
{
	if (list_is_empty(&tskl))
		return;

	struct cpytsk *ct, *tmp;
	list_for_each_entry_safe(ct, tmp, &tskl, list) {
		if (S_ISDIR(ct->mode))
			rmdirr(ct->dest);
		else
			unlink(ct->dest);

		list_del(&ct->list);
		free(ct->dest);
		free(ct);
	}
}

static struct savesave *find_sav(const char *name)
{
	dotsav_prepare();
	struct savesave *sav = dotsav_array;

	for_each_sav(sav)
		if (strcmp(sav->name, name) == 0)
			break;

	if (sav_is_last(sav)) {
		const char *path = dotsav_path();
		die(_("no matching sav `%s' found in `%s'"), name, path);
	}

	return sav;
}

/*
 * Returns the number of sorted backups. If an error is encountered, -1
 * is returned. A return value of -EBUSY indicates that there's no room
 * to make any backups.
 *
 * NB: this function uses dest as buffer to store the formatted path,
 *     which is returned by __next.
 */
static int backup_messy_sort(struct strbuf *src,
			     struct strbuf *dest,
			     u8 stack, const char **__next)
{
	u8 i;
	u8 first;
	int next = -1;

	for_each_idx(i, stack) {
		strbuf_boprintf(src, "%u", i);

		int err = access(src->str, F_OK);
		if (!err) {
			if (next == -1)
				continue;

			strbuf_boprintf(dest, "%u", next);
			err = rename(src->str, dest->str);
			if (!err) {
				next += 1;
				continue;
			}

			return warn_errno(ERRMAS_RENAME_FILE(src->str,
							     dest->str));
		} else if (errno != ENOENT) {
			return warn_errno(ERRMAS_ACCESS_FILE(src->str));
		} else if (next == -1) {
			first = i;
			next = i;
		}
	}

	if (next == -1)
		return -EBUSY;

	if (!__next)
		goto out;

	strbuf_boprintf(dest, "%u", next);
	*__next = dest->str;

out:
	return next - first;
}

static void sav2argv(const char *name, int *argc, const char ***argv)
{
	struct savesave *sav = find_sav(name);
	struct strbuf src;
	struct strbuf dest;

	strbuf_init2(&src, sav->backup_prefix, 0);
	strbuf_init2(&dest, sav->backup_prefix, 0);

	const char *next;
	int ret;

retry:
	ret = backup_messy_sort(&src, &dest, sav->stack, &next);

	if (ret == -1) {
		die(_("unable to sort backup %s for sav `%s'"),
		    dest.str, sav->name);
	} else if (ret == -EBUSY) {
		strbuf_boprintf(&dest, "%u", 0);
		ret = flexremove(dest.str);
		if (ret)
			die(_("unable to drop deprecated backup for `%s'"),
			    sav->name);
		goto retry;
	}

	strbuf_destroy(&src);
	*argc = 2;
	*argv = xcalloc(sizeof(*argv), 3);

	(*argv)[0] = sav->save_prefix;
	(*argv)[1] = next;
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
	argc -= 1;

	size_t i;
	struct strbuf sb;
	struct stat st;

	strbuf_init2(&sb, argv[argc], 0);
	for_each_idx(i, argc) {
		assert_file_not_same(sb.str, argv[i]);

		int err = stat(argv[i], &st);
		if (err) {
			warn_errno(ERRMAS_STAT_FILE(argv[i]));
			continue;
		}

		struct cpytsk *tsk = xmalloc(sizeof(*tsk));
		tsk->src = argv[i];
		tsk->mode = st.st_mode;

		char *tmp = xstrdup(tsk->src);
		const char *name = basename(tmp);

		if (argc > 1)
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
	char *cp;

	err = access(name, F_OK);
	if (err)
		goto mkdir;

	if (!force_exec)
		die(_("dest `%s' already exists"), name);

	err = flexremove(name);
	if (err)
		die(_("cannot remove dest `%s'"), name);

mkdir:
	cp = xstrdup(name);
	err = mkfdirp3(cp);
	if (err)
		die_errno(ERRMAS_CREAT_DIR(cp));
	free(cp);
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
	strbuf_boconcat(dest, src->dymname);

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

		int err;
		if (!S_ISDIR(ct->mode))
			err = copy_regfile(ct->src, ct->dest);
		else
			err = fileiter(ct->src,
				       def_copy_routine, &sb, FITER_LIST_DIR);
		if (err) {
			error(ERRMAS_COPY_FILE(ct->src, ct->dest));
			continue;
		}

		list_del(&ct->list);
		free(ct->dest);
		free(ct);
	}

	strbuf_destroy(&sb);
}

static void aio_copy(void)
{
	if (__builtin_popcount(aio_qs) != 1)
		die(_("qs is not a power of two"));

	acpy_deploy(aio_qs, aio_bs);
	if (acpy_disabled)
		return def_copy();

	struct cpytsk *ct, *tmp;
	list_for_each_entry_safe(ct, tmp, &tskl, list) {
		prepare_dest_dir(ct->dest);

		int err = acpy_copy(ct->src, ct->dest);
		if (err)
			die(ERRMAS_COPY_FILE(ct->src, ct->dest));

		list_del(&ct->list);
		free(ct->dest);
		free(ct);
	}

}

int cmd_copy(int argc, const char **argv)
CMDDESCRIP("Copy a file")
{
	struct arguopt option[] = {
		APOPT_COUNTUP('f', "force", &force_exec,
			      N_("overwrite existing one")),

		APOPT_GROUP("List of file copying methods"),
		APOPT_CMDMODE(0, "asynced", &copy_mode, CPY_ASYNCED,
			      N_("copy files using asynchronous io")),

		APOPT_GROUP("Options for asynchronous io"),
		APOPT_UINT(0, "qs", &aio_qs,
			   N_("size of aio queue, must be the power of two")),
		APOPT_UINT(0, "bs", &aio_bs,
			   N_("max block size of a file in request")),

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
		[CPY_ASYNCED]  = aio_copy,
	};

	cls_push(cleanup_cpytsk);
	funcmap[copy_mode]();
	exit(0);
}
