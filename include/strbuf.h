/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef STRBUF_H
#define STRBUF_H

#define STRBUF_INIT { 0 }

/*
 * Same as strbuf_init, except it also setup base.
 */
void strbuf_init2(struct strbuf *sb, const char *base, flag_t flags);

/*
 * Initialize strbuf with flags.
 */
static inline void strbuf_init(struct strbuf *sb, flag_t flags)
{
	strbuf_init2(sb, NULL, flags);
}

/*
 * Destroy strbuf.
 */
void strbuf_destroy(struct strbuf *sb);

/*
 * Reset length to its initializing status.
 */
static inline void strbuf_reset_length(struct strbuf *sb)
{
	sb->len = sb->base;
}

/*
 * Reset strbuf to its initializing status.
 */
static inline void strbuf_reset(struct strbuf *sb)
{
	strbuf_reset_length(sb);
	sb->str[sb->len] = 0;
}

/*
 * Full-cleanup strbuf, and setup base.
 */
void strbuf_reset_from(struct strbuf *sb, const char *base);

/*
 * Return space available for storing characters (excluding null terminator).
 * NB: Don't call this function on an empty strbuf (cap is zero).
 */
static inline uint strbuf_avail(struct strbuf *sb)
{
	return sb->cap - sb->len - 1;
}

/*
 * Similar to strbuf_concat(), except it includes a position argument
 * that specifies the buffer position where string is appended.
 *
 * NB:	The behavior is undefined if the offset exceeds the length of
 *	strbuf.
 */
uint strbuf_oconcat(struct strbuf *sb, uint offset, const char *str);

/*
 * Append str to the tail of strbuf.
 */
static inline uint strbuf_concat(struct strbuf *sb, const char *str)
{
	return strbuf_oconcat(sb, sb->len, str);
}

/*
 * Append str to the base of strbuf.
 */
static inline uint strbuf_boconcat(struct strbuf *sb, const char *str)
{
	return strbuf_oconcat(sb, sb->base, str);
}

uint strbuf_oconcat_char(struct strbuf *sb, uint offset, int c);

static inline uint strbuf_concat_char(struct strbuf *sb, int c)
{
	return strbuf_oconcat_char(sb, sb->len, c);
}

static inline uint strbuf_boconcat_char(struct strbuf *sb, int c)
{
	return strbuf_oconcat_char(sb, sb->base, c);
}

/*
 * Append /name to strbuf.
 */
uint strbuf_concat_basename(struct strbuf *sb, const char *name);

/*
 * Concat prefix/name to strbuf.
 */
uint strbuf_concat_pathname(struct strbuf *sb,
			    const char *prefix, const char *name);

/*
 * Produce output to strbuf according to format.
 */
#define strbuf_printf(__sb, ...) \
	strbuf_oprintf(__sb, (__sb)->len, __VA_ARGS__)

/*
 * Similar to strbuf_printf(), except it fills data at base.
 */
#define strbuf_boprintf(__sb, ...) \
	strbuf_oprintf(__sb, (__sb)->base, __VA_ARGS__)

/*
 * Similar to strbuf_printf(), except it takes a position argument that
 * specifies the buffer position at which printf starts to fill data.
 */
uint strbuf_oprintf(struct strbuf *sb,
		    uint offset, const char *format, ...) __nonnull(3);

static inline void strbuf_trunc(struct strbuf *sb, uint size)
{
	sb->len -= size;
	sb->str[sb->len] = 0;
}

/*
 * Remove space character from both ends of strbuf.
 */
void strbuf_trim(struct strbuf *sb);

/*
 * Count occurrences of c in strbuf.
 */
uint strbuf_count_char(struct strbuf *sb, int c);

/*
 * Return the pointer points to the last path separator.
 */
static inline char *strbuf_strrsep(struct strbuf *sb)
{
#ifdef _WIN32
	char *sep = strrchr(sb->str, '/');
	return sep ? : strrchr(sb->str, '\\');
#else
	return strrchr(sb->str, '/');
#endif
}

/*
 * Truncates the length of pathname stored in strbuf to the length of
 * its dirname.
 */
void strbuf_dirname(struct strbuf *sb);

/*
 * Unify path separator into slash.
 */
void strbuf_normalize_path(struct strbuf *sb);

extern int mkdirp2(char *name, size_t start, int dcheck);

/*
 * Make directory as well as its parent directories, starting from
 * initial string.
 */
static inline int strbuf_mkdirp(struct strbuf *sb)
{
	return mkdirp2(sb->str, sb->base, 0);
}

#endif /* STRBUF_H */
