/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef TEXT2NUM_H
#define TEXT2NUM_H

#ifdef __cplusplus
extern "C" {
#endif

int str2ulong(const char *str, size_t len,
	      unsigned long max, unsigned long *val);

static inline int str2u32(const char *str, u32 *val)
{
	return str2ulong(str, strlen(str), UINT32_MAX, (unsigned long *)val);
}

static inline int str2u8(const char *str, u8 *val)
{
	return str2ulong(str, strlen(str), UINT8_MAX, (unsigned long *)val);
}

#if UINT_MAX == 4294967295U
# define str2uint str2u32
#else
static inline int str2uint(const char *str, unsigned *val)
{
	return str2ulong(str, strlen(str), UINT_MAX, (unsigned long *)val);
}
#endif

int str2period(const char *str, u32 *val);

int str2bool(const char *str, int *val);

#ifdef __cplusplus
}
#endif

#endif /* TEXT2NUM_H */
