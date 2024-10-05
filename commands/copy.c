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

static uint thrd_numb = -1;
static int force_exec;

static int makedir(struct iterfile *src, void *data)
{
	struct strbuf *dest = &((struct strbuf *)data)[0];
	strbuf_concatat_base(dest, src->dymname);

	int err = MKDIR(dest->str);
	if (unlikely(err))
		return warn_errno(ERRMAS_CREAT_DIR(dest->str));

	return 0;
}

static int cpyfile(struct iterfile *src, void *data)
{
	struct strbuf *dest = &((struct strbuf *)data)[0];
	strbuf_concatat_base(dest, src->dymname);

	// return cpsched_schedule(src->absname, dest->str,
	// 			S_ISLNK(src->st.st_mode));
	if (S_ISLNK(src->st.st_mode))
		return copy_symlink(src->absname, dest->str);
	else
		return copy_regfile(src->absname, dest->str);
}

static int copy(const char *src, const char *dest)
{
	int err = access(dest, F_OK);
	if (!err) {
		if (!force_exec)
			die(_("dest `%s' already exists"), dest);

		err = flexremove(dest);
		if (err)
			die(_("cannot remove dest `%s'"), dest);
	}

	char *dir = xstrdup(dest);

	err = mkfdirp(dir);
	free(dir);
	if (err)
		return error_errno(ERRMAS_CREAT_DIR(dest));

	int ret[2];
	struct strbuf data[1] = {
		[0] = strbuf_from2(dest, 0, PATH_MAX),
	};

	ret[0] = fileiter(src, makedir, data, FITER_LIST_DIR | FITER_DIR_ONLY);
	if (ret[0])
		goto err_cpy_file;

	cpsched_deploy(thrd_numb);
	ret[0] = fileiter(src, cpyfile, data, 0);
	cpsched_join(&ret[1]);

	if (ret[0] || ret[1])
		goto err_cpy_file;

	if (0) {
	err_cpy_file:
		error(ERRMAS_COPY_FILE(src, dest));
		ret[0] = -1;
	}

	strbuf_destroy(&data[0]);
	return ret[0];
}

static int copy_sav(const char *name)
{
	struct savesave *savarr;
	struct savesave *sav = NULL;
	retrieve_dotsav(&savarr);

	for_each_sav(savarr) {
		if (strcmp(savarr->name, name))
			continue;

		sav = savarr;
		break;
	}

	if (!sav)
		die(_("no matching sav `%s' found in `%s'"), name,
		    userspec.dotsav_path);

	/* remove tailing dot */
	size_t len = strlen(sav->backup_prefix) - 1;
	char *dest = xmalloc(len + 1);

	memcpy(dest, sav->backup_prefix, len);
	dest[len] = 0;

	int ret = copy(sav->save_prefix, dest);

	NOLEAK(dest);
	return ret;
}

static int copy_multisrc(int argc, const char **argv)
{
	char **name = xcalloc(sizeof(*argv), argc);
	size_t i;
	size_t j;

	for_each_idx(i, argc - 1) {
		char *buf = xstrdup(argv[i]);
		char *base = basename(buf);

		name[i] = xstrdup(base);
		free(buf);

		for_each_idx(j, i) {
			if (strcmp(name[j], name[i]))
				continue;

			die(_("name `%s' has collisions at `%s' and `%s'"),
			    name[i], argv[j], argv[i]);
		}
	}

	struct strbuf sb = strbuf_from(argv[i], 0);
	strbuf_normalize_path(&sb);

	for_each_idx(i, argc - 1) {
		strbuf_concat_basename(&sb, name[i]);
		printf("from `%s' to `%s'\n", argv[i], sb.str);
		/* dispatch sub-process to handle this */
		// copy(argv[i], sb.str);
		strbuf_reset_length(&sb);
	}

	NOLEAK(sb);
	NOLEAK(name);
	return 0;
}

int cmd_copy(int argc, const char **argv)
CMDDESCRIP("Copy a file")
{
	struct arguopt option[] = {
		APOPT_UINT('n', "thread", &thrd_numb,
			   N_("number of threads to handle copy operations")),
		APOPT_COUNTUP('f', "force", &force_exec,
			      N_("overwrite existing one")),
		APOPT_END(),
	};
	const char *usage[] = {
		"savesave copy <source...> <dest>",
		"savesave copy <sav>",
		NULL,
	};

	argupar_t ap;
	argupar_init(&ap, argc, argv);

	argc = argupar_parse(&ap, option, usage, AP_NEED_ARGUMENT);

	if (thrd_numb == 0)
		die(_("thread number can't be zero"));

	uint cores = getcpucores();
	if (thrd_numb == -1)
		thrd_numb = cores;
	else if (thrd_numb > cores)
		warn(_("too many threads specified"));

	switch (argc) {
	case 1:
		return copy_sav(argv[0]);
	case 2:
		return copy(argv[0], argv[1]);
	default:
		return copy_multisrc(argc, argv);
	}
}
