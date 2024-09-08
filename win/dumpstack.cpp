// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#pragma GCC diagnostic ignored "-Wreturn-type"

#include "win/dumpstack.hpp"
#include "win/termas.hpp"
#include "strlist.h"
#include "list.h"
#include "alloc.h"
#include "debug.h"

static void assert_failure_routine()
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
	unsigned short frames = CaptureStackBackTrace(0, sizeof_array(stack),
						      stack, NULL);

	SYMBOL_INFO *symbol;
	char buf[sizeof(*symbol) + MAX_SYM_NAME * sizeof(TCHAR)];

	symbol = (SYMBOL_INFO *)buf;
	symbol->SizeOfStruct = sizeof(*symbol);
	symbol->MaxNameLen = MAX_SYM_NAME;

	unsigned short i;
	IMAGEHLP_LINE64 line = {
		.SizeOfStruct = sizeof(line),
	};

	for_each_idx(i, frames) {
		DWORD64 addr = (DWORD64)(stack[i]);

		err = !SymFromAddr(proc, addr, 0, symbol);
		if (err)
			die_winerr(
_("failed to retrieve symbol for 0x" PRIxMAX ""), addr);

		printf("%5d: 0x%" PRIxMAX " - %s\n",
		       frames - i - 1, symbol->Address, symbol->Name);
	}

	exit(128);
}

static int savesave_report_hook(int type, char *message, int *)
{
	size_t i;
	struct strlist sl = STRLIST_INIT;

	strlist_strsplt_every2(&sl, message, CONFIG_DO_LINE_WRAP_THRESHOLD);

	for_each_idx(i, sl.nl)
		puts(sl.list[i].str);

	switch (type) {
	case _CRT_ASSERT:
		assert_failure_routine();
	}

	strlist_destroy(&sl);
	return 0;
}

void setup_crt_report_hook()
{
	_CrtSetReportHook(savesave_report_hook);
}
