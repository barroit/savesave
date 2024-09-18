// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "fileiter.h"
#include "alloc.h"
#include "strbuf.h"
#include "strlist.h"

void fileiter_init(struct fileiter *ctx, const char *root,
		   fileiter_callback_t cb, void *data, flag_t flags)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->root = root;

	ctx->sb = xmalloc(sizeof(*ctx->sb));
	strbuf_init(ctx->sb, 0);

	ctx->sl = xmalloc(sizeof(*ctx->sl));
	strlist_init(ctx->sl, 0);

	ctx->cb = cb;
	ctx->data = data;

	ctx->flag = flags;
}

void fileiter_destroy(struct fileiter *ctx)
{
	strbuf_destroy(ctx->sb);
	strlist_destroy(ctx->sl);

	free(ctx->sb);
	free(ctx->sl);
}

static int dispatch_directory(struct fileiter *ctx)
{
	struct fileiter_file file = {
		.absname = ctx->sb->str,
		.dymname = straftr(file.absname, ctx->root),
		.is_dir  = 1,
		.fd      = -1,
	};

	return ctx->cb(&file, ctx->data);
}

int fileiter_exec(struct fileiter *ctx)
{
	int err;
	const char *dir = ctx->root;
	size_t baslen = strlen(dir), ndrlen;

	while (39) {
		strbuf_reset_from(ctx->sb, dir);

		err = PLATSPECOF(fileiter_do_exec)(ctx);
		if (unlikely(err))
			return -1;

		dir = strlist_pop2(ctx->sl, 0);
		if (unlikely(!dir)) {
			if (!(ctx->flag & FI_LIST_DIR))
				return 0;

			strbuf_reset(ctx->sb);
			ndrlen = baslen;
			goto iter_done;
		}

		if (!(ctx->flag & FI_LIST_DIR))
			continue;
		strbuf_reset(ctx->sb);

		const char *prev = ctx->sb->str, *next = dir;
		ndrlen = strrchr(next, '/') - next;

		/*
		 * If previous name is the prefix of next name, we skip it
		 */
		if (strlen(prev) == ndrlen)
			continue;
		size_t pdrlen = strrchr(prev, '/') - prev;

		/*
		 * Previous name has the same parent as next name
		 */
		if (pdrlen == ndrlen) {
			err = dispatch_directory(ctx);
			if (err)
				return -1;
			continue;
		}

		/*
		 * Previous branch is done
		 */
		while (39) {
iter_done:
			err = dispatch_directory(ctx);
			if (unlikely(err))
				return -1;

			strbuf_to_dirname(ctx->sb);
			if (ctx->sb->len == ndrlen) {
				if (likely(dir))
					break;
				else
					return dispatch_directory(ctx);
			}
		}
	}
}
