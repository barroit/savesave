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

#define TEXTDOMAIN SAVESAVE_NAME

/*
 * CONFIG_TARGET_LOCALE > LANGUAGE > LANG
 */
static const char *detect_locale(void)
{
	static const char *lang;

	if (!lang) {
		if (*CONFIG_TARGET_LOCALE == 0) {
			lang = getenv("LANGUAGE");
			if (lang)
				return lang;

			lang = getenv("LANG");
			if (lang)
				return lang;

			lang = "";
		} else {
			struct strbuf sb;
			strbuf_init2(&sb, CONFIG_TARGET_LOCALE, 0);

			const char *sep = strchr(sb.str, '.');
			if (!sep)
				strbuf_concat(&sb, ".UTF-8");

			lang = sb.str;
		}
	}

	return lang;
}

void setup_message_translation(void)
{
	const char *dir = locale_dir();
	const char *locale = detect_locale();

	textdomain(TEXTDOMAIN);
	bindtextdomain(TEXTDOMAIN, dir);
	bind_textdomain_codeset(TEXTDOMAIN, "UTF-8");

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

	setlocale(LC_TIME, locale);
	setlocale(LC_MONETARY, locale);
	setlocale(LC_MESSAGES, locale);
}

#endif
