// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "robio.h"
#include "alloc.h"
#include "strbuf.h"
#include "strlist.h"

void file_iter_init(struct file_iter *ctx, const char *root,
		    file_iterator_cb_t cb, void *data)
{
	memset(ctx, 0, sizeof(*ctx));

	ctx->root = root;

	ctx->sb = xmalloc(sizeof(*ctx->sb));
	strbuf_init(ctx->sb, 0);

	ctx->sl = xmalloc(sizeof(*ctx->sl));
	strlist_init(ctx->sl, STRLIST_DUPSTR);

	ctx->cb = cb;
	ctx->data = data;
}

void file_iter_destroy(struct file_iter *ctx)
{
	strbuf_destroy(ctx->sb);
	strlist_destroy(ctx->sl);

	free(ctx->sb);
	free(ctx->sl);
}

int file_iter_do_exec(struct file_iter *ctx);

int file_iter_exec(struct file_iter *ctx)
{
	int ret;
	const char *dir = ctx->root;

	do {
		strbuf_concat(ctx->sb, dir);

		ret = file_iter_do_exec(ctx);
		if (ret)
			return 1;

		strbuf_zerolen(ctx->sb);
	} while ((dir = strlist_pop2(ctx->sl, 0)));

	return 0;
}
