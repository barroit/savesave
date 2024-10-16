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
	char *src;
	char *dest;
	struct list_head list;
};

struct cpthrd {
	struct list_head queue;
	uint pending;

	mtx_t lock;
	cnd_t created;

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

	while (39) {
		mtx_lock(&pool.lock);
		if (pool.state != POOL_RUNNING)
			goto terminate_thread;

retry:
		cnd_wait(&pool.created, &pool.lock);
		if (unlikely(pool.state != POOL_RUNNING))
			goto terminate_thread;

		if (unlikely(list_is_empty(&pool.queue)))
			goto retry;

		struct cptask *task = list_last_entry(&pool.queue,
						      typeof(*task), list);
		list_del(&task->list);
		pool.pending -= 1;

		mtx_unlock(&pool.lock);

			err = copy_regfile(task->src, task->dest);
			err = copy_regfile(task->src, task->dest);

		err = copy_regfile(task->src, task->dest);

		release_task(task);

		if (likely(!err))
			continue;

		__atomic_store_n(&pool.state, POOL_ERROR, __ATOMIC_RELEASE);
		cnd_broadcast(&pool.created);
		thrd_exit(POOL_ERROR);
	}

	BUG_ON(1);

terminate_thread:
	mtx_unlock(&pool.lock);
	thrd_exit(pool.state);
}

void cpsched_deploy(size_t nl)
{
	BUG_ON(pool.thrd);
	list_head_init(&pool.queue);

	pool.nl = nl;
	pool.thrd = xcalloc(sizeof(*pool.thrd), pool.nl);

	xmtx_init(&pool.lock, mtx_plain);
	xcnd_init(&pool.created);

	thrd_affinity_all(nl);

	size_t i;
	for_each_idx(i, pool.nl)
		xthrd_create(&pool.thrd[i], cpsched_task, NULL);
}

int cpsched_schedule(const char *src, const char *dest)
{
	int state = __atomic_load_n(&pool.state, __ATOMIC_RELAXED);
	if (unlikely(state != POOL_RUNNING))
		goto return_state;

	uint pending = __atomic_load_n(&pool.pending, __ATOMIC_RELAXED);
	if (pending > MAX_PENDING)
		return SCHED_BUSY;

	struct cptask *task = xmalloc(sizeof(*task));

	task->src = xstrdup(src);
	task->dest = xstrdup(dest);

	mtx_lock(&pool.lock);

	state = pool.state;
	if (unlikely(state != POOL_RUNNING)) {
		mtx_unlock(&pool.lock);
		release_task(task);
		goto return_state;
	}

	list_add_tail(&task->list, &pool.queue);
	pool.pending += 1;

	cnd_signal(&pool.created);
	mtx_unlock(&pool.lock);
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
	mtx_destroy(&pool.lock);

	*res = state;
}
