// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "cpsched.h"
#include "list.h"
#include "alloc.h"
#include "fcpy.h"

#define MAX_PENDING 5

struct cptask {
	struct list_head list;

	char *src;
	char *dest;

	int is_lnk;
};

struct cpthrd {
	struct list_head queue;
	uint pending;

	mtx_t mtx;
	cnd_t created;
	cnd_t released;

	size_t nl;
	thrd_t *thrd;

	int state;
};

static struct cpthrd pool;

static void release_task(struct cptask *task)
{
	free(task->src);
	free(task->dest);
	free(task);
}

static int cpsched_task(void *data)
{
	int err;
	int prev_done = 0;

	while (39) {
		mtx_lock(&pool.mtx);
		if (pool.state != POOL_RUNNING)
			goto terminate_thread;

		if (prev_done) {
			cnd_signal(&pool.released);
			pool.pending -= 1;
			prev_done = 0;
		}

retry:
		cnd_wait(&pool.created, &pool.mtx);
		if (unlikely(pool.state != POOL_RUNNING))
			goto terminate_thread;

		if (unlikely(list_is_empty(&pool.queue)))
			goto retry;

		struct cptask *task = list_last_entry(&pool.queue,
						      typeof(*task), list);
		list_del(&task->list);

		mtx_unlock(&pool.mtx);

		if (task->is_lnk)
			err = copy_symlink(task->src, task->dest);
		else
			err = copy_regfile(task->src, task->dest);

		release_task(task);
		prev_done = 1;

		if (likely(!err))
			continue;

		__atomic_store_n(&pool.state, POOL_ERROR, __ATOMIC_RELEASE);
		cnd_broadcast(&pool.created);
		thrd_exit(POOL_ERROR);
	}

	BUG_ON(1);

terminate_thread:
	mtx_unlock(&pool.mtx);
	thrd_exit(pool.state);
}

void cpsched_deploy(size_t nl)
{
	BUG_ON(pool.thrd);
	list_head_init(&pool.queue);

	pool.nl = nl;
	pool.thrd = xcalloc(sizeof(*pool.thrd), pool.nl);

	xmtx_init(&pool.mtx, mtx_plain);
	xcnd_init(&pool.created);
	xcnd_init(&pool.released);

	thrd_affinity_all(nl);

	size_t i;
	for_each_idx(i, pool.nl)
		xthrd_create(&pool.thrd[i], cpsched_task, NULL);
}

int cpsched_schedule(const char *src, const char *dest, int is_lnk)
{
	int state;
	struct cptask *task = xmalloc(sizeof(*task));

	task->src = xstrdup(src);
	task->dest = xstrdup(dest);
	task->is_lnk = is_lnk;

	mtx_lock(&pool.mtx);

	while (pool.pending > MAX_PENDING)
		cnd_wait(&pool.released, &pool.mtx);

	state = pool.state;
	if (unlikely(state != POOL_RUNNING)) {
		mtx_unlock(&pool.mtx);
		release_task(task);
		goto return_state;
	}

	list_add_tail(&task->list, &pool.queue);
	pool.pending += 1;

	cnd_signal(&pool.created);
	mtx_unlock(&pool.mtx);
	return 0;

return_state:
	switch (state) {
	case POOL_ERROR:
		return SCHED_ERROR;
	case POOL_CLOSED:
		return SCHED_TERMINATED;
	}

	BUG_ON(1);
}

void cpsched_join(int *res)
{
	size_t i;
	int state = __atomic_exchange_n(&pool.state, POOL_CLOSED,
					__ATOMIC_ACQ_REL);

	cnd_broadcast(&pool.created);
	for_each_idx(i, pool.nl)
		thrd_join(pool.thrd[i], NULL);

	cnd_destroy(&pool.created);
	cnd_destroy(&pool.released);
	mtx_destroy(&pool.mtx);

	*res = state;
}
