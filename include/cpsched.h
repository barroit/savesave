/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef CPSCHED_H
#define CPSCHED_H

/**
 * cpsched_deploy - Deploy copy scheduler
 *
 * note: This function can only be called once
 *	 during the life cycle of a program
 */
void cpsched_deploy(size_t nl);

/**
 * cpsched_schedule - Schedule a task
 *
 * return: SCHED_SUCCESS, SCHED_BUSY, SCHED_ERROR
 */
int cpsched_schedule(const char *src, const char *dest, int is_lnk);

/**
 * cpsched_join - Block current thread until all schedule
 *		  handling threads finish execution
 */
void cpsched_join(int *res);

#endif /* CPSCHED_H */
