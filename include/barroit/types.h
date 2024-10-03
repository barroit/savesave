/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef BRT_TYPES_H
#define BRT_TYPES_H

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef long long llong;
typedef unsigned long long ullong;

typedef uintmax_t flag_t;

struct list_head {
	struct list_head *prev;
	struct list_head *next;
};

struct strbuf {
	char *str;
	size_t len;
	size_t cap;
	size_t baslen;

	int is_const;
};

struct strlist {
	struct strbuf *list;
	size_t uninit;
	size_t nl;
	size_t cap;

	int use_ref;
};

struct savesave {
	char *name;

	char *save_prefix;
	char *backup_prefix;

	int is_dir_save;
	int use_compress;

	u32 period;
	u8 stack;

	uint task_idx;
	ullong task_pos;
};

enum {
	SCHED_ERROR      = -1,
	SCHED_SUCCESS    = 0,
	SCHED_BUSY       = 1,
	SCHED_TERMINATED = 39,
};

enum {
	POOL_RUNNING = 0,
	POOL_CLOSED  = 39,
	POOL_ERROR   = 128,
};

#endif /* BRT_TYPES_H */
