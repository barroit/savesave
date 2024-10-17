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

#ifdef _WIN32
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef SSIZE_T ssize_t;

typedef uint mode_t;

typedef int pid_t;

typedef ulong rlim_t;
#endif

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
	char *name;		/* sav name */

	char *save_prefix;	/* prefix of save path */
	char *backup_prefix;	/* prefix of backup path */

	int is_dir;		/* save file is directory */
	int use_compress;	/* compress save file */

	u32 period;		/* backup period (in seconds) */
	u8 stack;		/* maximum number of backups
				   exist at the same time */

	/*
	 * task_* is used by baksched
	 */
	uint task_idx;		/* index of task array */
	ullong task_pos;	/* position of a task array element */

	struct list_head head;
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
