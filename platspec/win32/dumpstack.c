// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"
#include "strlist.h"
#include "iter.h"
#include "constructor.h"

#ifdef HAVE_RPHK
static void assert_failure_routine(void)
{
	BUG_ON(CONFIG_MAX_DUMP_STACK > USHRT_MAX);

	int err;
	DWORD opt = SymGetOptions();
	HANDLE proc = GetCurrentProcess();

	SymSetOptions(opt | SYMOPT_LOAD_LINES);
	err = !SymInitialize(proc, NULL, TRUE);
	if (err)
		die_winerr(_("failed to initialize symbol handler"));

	void *stack[CONFIG_MAX_DUMP_STACK];
	ushort frames = CaptureStackBackTrace(0, sizeof_array(stack),
					      stack, NULL);

	SYMBOL_INFO *symbol;
	char buf[sizeof(*symbol) + MAX_SYM_NAME * sizeof(TCHAR)];

	symbol = (SYMBOL_INFO *)buf;
	symbol->SizeOfStruct = sizeof(*symbol);
	symbol->MaxNameLen = MAX_SYM_NAME;

	ushort i;
	IMAGEHLP_LINE64 line = {
		.SizeOfStruct = sizeof(line),
	};

	for_each_idx(i, frames) {
		DWORD64 addr = (DWORD64)(stack[i]);

		err = !SymFromAddr(proc, addr, 0, symbol);
		if (err) {
			warn_winerr(_("failed to retrieve symbol for 0x" PRIxMAX),
				    addr);
			continue;
		}

		printf("%5d: 0x%" PRIxMAX " - %s\n",
		       frames - i - 1, symbol->Address, symbol->Name);
	}

	exit(128);
}

static int savesave_report_hook(int type, char *message, int *_)
{
	struct strlist sl;

	strlist_init(&sl, STRLIST_STORE_REF);
	strlist_split_word(&sl, message, CONFIG_LINEWRAP_THRESHOLD);

	while (39) {
		const char *str = strlist_pop(&sl);
		if (!str)
			break;
		puts(str);
	}

	switch (type) {
	case _CRT_ASSERT:
		assert_failure_routine();
	}

	strlist_destroy(&sl);
	return 0;
}
#endif

CONSTRUCTOR(setup_crt_report_hook)
{
#ifdef HAVE_RPHK
	_CrtSetReportHook(savesave_report_hook);
#endif
}

