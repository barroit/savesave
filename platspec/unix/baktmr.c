// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "baktmr.h"
#include "iter.h"
#include "dotsav.h"
#include "alloc.h"
#include "termas.h"
#include "barrier.h"

struct baktmr {
	timer_t *tmr;		/* timer ids */
	char *rtm;		/* running task map, using char
				   due to __atomic builtins */
};

/* overload counter */
struct bakolc {
	uint head;
	uint tail;
	uint mask;
	timestamp_t arr[1 << 3];

	int killed;
	mtx_t lock;
};

static struct baktmr btmr;
static struct bakolc bolc = {
	.mask = sizeof_array(bolc.arr) - 1,
};

static void tmr_callback_busy(uint i)
{
	timestamp_t new = timestamp();
	timestamp_t old;

	mtx_lock(&bolc.lock);
	if (bolc.killed) {
		mtx_unlock(&bolc.lock);
		return;
	}

	uint tail = bolc.tail;
	uint head = bolc.head;

	if (tail == head)
		goto push_ts;

	while (head != tail) {
		old = bolc.arr[head & bolc.mask];
		if (new - old <= BAKTMR_OVERLOAD_WINDOW)
			break;

		head += 1;
	}

	bolc.head = head;
	if (head == tail)
		goto push_ts;

	if (tail + 1 - head > BAKTMR_MAX_OVERLOAD) {
		bolc.killed = 1;
		mtx_unlock(&bolc.lock);

		error(_("too many backup requests were dropped, "
		      "savesave can't keep up anymore"));
		kill(0, SIGTERM);
	}

push_ts:
	bolc.arr[tail & bolc.mask] = new;
	tail += 1;

	bolc.tail = tail;
	mtx_unlock(&bolc.lock);

	warn(_("backup request of sav `%s' was dropped"),
	     dotsav_array[i].name);
}

static void tmr_callback(sigval_t sv)
{
	uint i = sv.sival_int;

	char running = __atomic_test_and_set(&btmr.rtm[i], __ATOMIC_ACQ_REL);
	if (running) {
		tmr_callback_busy(i);
		return;
	}

	FEATSPEC(baktmr_callback)(i);

	__atomic_clear(&btmr.rtm[i], __ATOMIC_RELEASE);
}

void baktmr_arm(void)
{
	mtx_init(&bolc.lock, mtx_plain);

	btmr.tmr = xcalloc(sizeof(*btmr.tmr), dotsav_size);
	btmr.rtm = xcalloc(sizeof(*btmr.rtm), dotsav_size);

	int err;
	size_t i;
	struct itimerspec ts = { 0 };
	struct sigevent sev = {
		.sigev_notify           = SIGEV_THREAD,
		.sigev_notify_function  = tmr_callback,
	};

	for_each_idx(i, dotsav_size) {
		sev.sigev_value.sival_int = i;
retry:
		err = timer_create(CLOCK_REALTIME, &sev, &btmr.tmr[i]);
		if (!err)
			continue;
		else if (errno == EAGAIN)
			goto retry;

		die_errno(_("failed to create backup timer for sav `%s'"),
			  dotsav_array[i].name);
	}

	for_each_idx(i, dotsav_size) {
		ts.it_value.tv_sec = dotsav_array[i].period;
		ts.it_interval.tv_sec = dotsav_array[i].period;

		timer_settime(btmr.tmr[i], 0, &ts, NULL);
	}
}

void baktmr_disarm(void)
{
	size_t i;
	struct itimerspec ts = { 0 };

	for_each_idx(i, dotsav_size)
		timer_settime(btmr.tmr[i], 0, &ts, NULL);
}
