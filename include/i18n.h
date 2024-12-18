/* SPDX-License-Identifier: GPL-3.0-or-later */
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#ifndef I18N_H
#define I18N_H

#ifdef HAVE_INTL

#define gettext_noop(masid) masid

#define _(masid)  gettext(masid)
#define N_(masid) gettext_noop(masid)

void i18n_setup(void);

#else /* NOT HAVE_INTL */

#define _(masid)  (masid)
#define N_(masid) masid

#define textdomain(...)     do {} while (0)
#define bindtextdomain(...) do {} while (0)

#define setup_message_translation() do {} while (0)

#endif /* HAVE_INTL */

#endif /* I18N_H */
