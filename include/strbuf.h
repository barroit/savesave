/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef STRBUF_H
#define STRBUF_H

#define STRBUF_INIT { 0 }

#define STRBUF_CONSTANT 1 << 0

/**
 * strbuf_init - initialize strbuf with flags
 */
void strbuf_init(struct strbuf *sb, flag_t flags);

/**
 * strbuf_from - initialize strbuf with initial string base, as well as a
 * 		 pre-allocate area
 *
 * extalloc - the extra bytes to be allocated, excluding null-terminating byte
 */
struct strbuf strbuf_from2(const char *base, flag_t flags, size_t extalloc);

/**
 * strbuf_from - initialize strbuf with initial string base
 */
static inline struct strbuf strbuf_from(const char *base, flag_t flags)
{
	return strbuf_from2(base, flags, 0);
}

/**
 * strbuf_require_cap - make strbuf has at least n capacity
 */
void strbuf_require_cap(struct strbuf *sb, size_t n);

/**
 * strbuf_destroy - destroy strbuf
 *
 * note: a destroyed strbuf object can be re-initialized using strbuf_init()
 */
void strbuf_destroy(struct strbuf *sb);

/**
 * strbuf_reset_length - reset length of strbuf to its initializing status
 */
static inline void strbuf_reset_length(struct strbuf *sb)
{
	sb->len = sb->baslen;
}

/**
 * strbuf_reset - reset strbuf object to its initializing status
 */
static inline void strbuf_reset(struct strbuf *sb)
{
	strbuf_reset_length(sb);
	sb->str[sb->len] = 0;
}

/**
 * strbuf_reset_from - reset strbuf and its initial string to base
 */
void strbuf_reset_from(struct strbuf *sb, const char *base);

/**
 * strbuf_move - store str reference instead of copy it
 * 
 * note: strbuf must initialized with STRBUF_CONSTANT
 */
size_t strbuf_move(struct strbuf *sb, const char *str);

/**
 * strbuf_concat2 - concat str to strbuf, with pre-allocate area
 *
 * extalloc - the extra bytes to be allocated, excluding null-terminating byte
 */
size_t strbuf_concat2(struct strbuf *sb, const char *str, size_t extalloc);

/**
 * strbuf_concat2 - concat str to strbuf
 */
static inline size_t strbuf_concat(struct strbuf *sb, const char *str)
{
	return strbuf_concat2(sb, str, 0);
}

/**
 * strbuf_concatat - concat str at specific index
 */
size_t strbuf_concatat(struct strbuf *sb, size_t idx, const char *str);

/**
 * strbuf_concatat_base - concat str to the initial string
 */
static inline size_t strbuf_concatat_base(struct strbuf *sb, const char *str)
{
	return strbuf_concatat(sb, sb->baslen, str);
}

/**
 * strbuf_printf - produce output to strbuf according to format
 */
size_t strbuf_printf(struct strbuf *sb, const char *fmt, ...);

/**
 * strbuf_trunc - truncate strbuf
 */
static inline void strbuf_trunc(struct strbuf *sb, size_t n)
{
	sb->len -= n;
	sb->str[sb->len] = 0;
}

/**
 * strbuf_trim - remove space character from both ends of strbuf
 */
void strbuf_trim(struct strbuf *sb);

/**
 * strbuf_cntchr - count occurrences of c in strbuf
 */
size_t strbuf_cntchr(struct strbuf *sb, int c);

/**
 * strbuf_normalize_path - unify path separator into slash
 */
void strbuf_normalize_path(struct strbuf *sb);

/**
 * strbuf_strrsep - get the pointer points to the last path separator
 */
static inline char *strbuf_strrsep(struct strbuf *sb)
{
#ifdef __unix__
	return strrchr(sb->str, '/');
#else
	char *sep = strrchr(sb->str, '/');
	return sep ? : strrchr(sb->str, '\\');
#endif
}

/**
 * strbuf_concat_path - concat '/name' to strbuf
 */
size_t strbuf_concat_basename(struct strbuf *sb, const char *name);

/**
 * strbuf_concat_path - concat 'prefix/name' to strbuf
 */
size_t strbuf_concat_path(struct strbuf *sb,
			  const char *prefix, const char *name);

extern int mkdirp2(char *name, size_t start);

/**
 * strbuf_mkdirp - make directory as well as its parent directories, starting
 * 		   from initial string
 */
static inline int strbuf_mkdirp(struct strbuf *sb)
{
	return mkdirp2(sb->str, sb->baslen);
}

/**
 * strbuf_to_dirname - make strbuf become its parent name
 */
void strbuf_to_dirname(struct strbuf *sb);

#endif /* STRBUF_H */
