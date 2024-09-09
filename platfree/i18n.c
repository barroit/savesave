// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "i18n.h"
#include "path.h"
#include "strbuf.h"

#ifdef HAVE_INTL

void setup_message_i18n(void)
{
	const char *dir = get_executable_dirname();
	struct strbuf sb = strbuf_from2(dir, 0, sizeof("/locale"));

	strbuf_concat(&sb, "/locale");
	strbuf_normalize_path(&sb);

	textdomain(SAVESAVE_NAME);
	bindtextdomain(SAVESAVE_NAME, sb.str);
	bind_textdomain_codeset(SAVESAVE_NAME, "UTF-8");

	strbuf_zerolen(&sb);
	if (*CONFIG_TARGET_LOCALE != 0)
		strbuf_printf(&sb, "%s.UTF-8", CONFIG_TARGET_LOCALE);

	setlocale(LC_TIME,     sb.str);
	setlocale(LC_MONETARY, sb.str);
	setlocale(LC_MESSAGES, sb.str);

	strbuf_destroy(&sb);
}

#endif
