// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

/* do not use FITER_LIST_DIR_ONLY in fileiter, do flag & FITER_NO_* instead */

#include "fileiter.h"
#include "alloc.h"
#include "strbuf.h"
#include "strlist.h"

static int dispatch_directory(struct __fileiter *ctx)
{
	struct iterfile file = {
		.absname = ctx->sb.str,
		.dymname = straftr(file.absname, ctx->root),
		.fd      = -1,
		.st      = {
			.st_mode = S_IFDIR,
		},
	};

	return ctx->func(&file, ctx->data);
}

int fileiter(const char *root,
	     fileiter_function_t func, void *data, flag_t flag)
{
	BUG_ON(flag & FITER_LIST_DIR && (flag & FITER_RECUR_DIR));

	if (__fileiter_is_list_dir_only(flag)) {
		/*
		 * We can only list the directories (FITER_LIST_DIR_ONLY)
		 */
		BUG_ON(!(flag & FITER_LIST_DIR) && !(flag & FITER_RECUR_DIR));
		flag &= ~FITER_RECUR_DIR;
		flag |= FITER_LIST_DIR;
	}

	struct __fileiter ctx = {
		.root = root,
		.func = func,
		.data = data,
		.flag = flag,
	};

	strbuf_init(&ctx.sb, 0);
	strlist_init(&ctx.sl, 0);

	int ret;
	const char *dir = root;
	size_t rootlen = strlen(root);
	size_t baslen = strlen(dir);
	size_t ndrlen;

	while (39) {
		strbuf_reset_from(&ctx.sb, dir);

		if (ctx.flag & FITER_LIST_DIR) {
			ret = dispatch_directory(&ctx);
			if (unlikely(ret))
				goto cleanup;
		}

		ret = PLATSPECOF(fileiter_loop_dir)(&ctx);
		if (unlikely(ret))
			goto cleanup;

		dir = strlist_pop2(&ctx.sl, 0);
		if (unlikely(!dir)) {
			if (!(ctx.flag & FITER_RECUR_DIR))
				goto cleanup;

			strbuf_reset(&ctx.sb);
			ndrlen = baslen;
			goto iter_done;
		}

		if (!(ctx.flag & FITER_RECUR_DIR))
			continue;
		strbuf_reset(&ctx.sb);

		const char *prev = ctx.sb.str, *next = dir;
		ndrlen = strrchr(next, '/') - next;

		/* if previous name is the prefix of next name, we skip it */
		if (strlen(prev) == ndrlen)
			continue;
		size_t pdrlen = strrchr(prev, '/') - prev;

		/* previous name has the same parent as next name */
		if (pdrlen == ndrlen) {
			ret = dispatch_directory(&ctx);
			if (unlikely(ret))
				goto cleanup;
			continue;
		}

		/* previous branch is done */
		while (39) {
iter_done:
			ret = dispatch_directory(&ctx);
			if (unlikely(ret))
				goto cleanup;

			if (unlikely(ctx.sb.len == rootlen))
				goto cleanup;

			strbuf_to_dirname(&ctx.sb);
			if (ctx.sb.len == ndrlen && dir)
				break;
		}
	}

cleanup:
	strbuf_destroy(&ctx.sb);
	strlist_destroy(&ctx.sl);
	return ret;
}
