/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef I18N_H
#define I18N_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_INTL

#define gettext_noop(masid) masid

#define _(masid)  gettext(masid)
#define N_(masid) gettext_noop(masid)

void setup_message_translation(void);

#else /* NOT HAVE_INTL */

#define _(masid)  (masid)
#define N_(masid) masid

#define textdomain(...)     do {} while (0)
#define bindtextdomain(...) do {} while (0)

#define setup_message_i18n() do {} while (0)

#endif /* HAVE_INTL */

#ifdef __cplusplus
}
#endif

#endif /* I18N_H */
