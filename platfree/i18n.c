// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "path.h"
#include "strbuf.h"
#include "alloc.h"

#ifdef HAVE_INTL

#define DOMAIN SAVESAVE_NAME

static char *get_locale_dir(void)
{
	const char *dir = get_executable_dirname();
	struct strbuf sb = strbuf_from2(dir, 0, sizeof("/locale"));

	strbuf_concat(&sb, "/locale");
	strbuf_normalize_path(&sb);

	return sb.str;
}

static void setup_gettext(const char *dir)
{
	textdomain(DOMAIN);
	bindtextdomain(DOMAIN, dir);
	bind_textdomain_codeset(DOMAIN, "UTF-8");
}

/*
 * CONFIG_TARGET_LOCALE > LANGUAGE > LANG
 */
static char *get_preferred_locale(void)
{
	const char *language = getenv("LANGUAGE");

	if (*CONFIG_TARGET_LOCALE == 0) {
		if (language && *language)
			return xstrdup(language);

		language = getenv("LANG");
		if (language && *language)
			return xstrdup(language);
	}

	struct strbuf locale = strbuf_from(CONFIG_TARGET_LOCALE, 0);
	if (*locale.str == 0)
		return locale.str;

	const char *sep = strchr(locale.str, '.');
	if (sep && sep[1] != 0)
		return locale.str;

	strbuf_concat(&locale, ".UTF-8");
	return locale.str;
}

static void setup_locale(const char *locale)
{
	setlocale(LC_TIME, locale);
	setlocale(LC_MONETARY, locale);
	setlocale(LC_MESSAGES, locale);
}

void setup_message_translation(void)
{
	char *dir = get_locale_dir();
	char *locale = get_preferred_locale();

	setup_gettext(dir);
	free(dir);

	/*
	 * gettext-runtime/intl/dcigettext.c:guess_category_value
	 * > // The highest priority value is the value of the 'LANGUAGE'
	 * > // environment variable.
	 * > language = getenv ("LANGUAGE");
	 *
	 * Setting LANGUAGE ensures that guess_category_value is preferred to
	 * return this value. Otherwise, it returns the locale variable, which
	 * may default to the default locale.
	 */
	setenv("LANGUAGE", locale, 1);

	setup_locale(locale);
	free(locale);
}

#endif