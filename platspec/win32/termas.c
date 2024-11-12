// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Copyright 2024 Jiamu Sun <barroit@linux.com>
 */

#include "termas.h"

const char *strwinerror(void)
{
	static char errbuf[256];
	int errnum = GetLastError();
	int langid = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
		      NULL, errnum, langid, errbuf,
		      sizeof_array(errbuf), NULL);

	errno = errnum;
	return errbuf;
}

int last_errno_error(void)
{
	DWORD code = GetLastError();

	switch (code) {
	case ERROR_FILE_NOT_FOUND:
	case ERROR_PATH_NOT_FOUND:
		return ENOENT;
	case ERROR_ACCESS_DENIED:
	case ERROR_SHARING_VIOLATION:
		return EACCES;
	case ERROR_INVALID_HANDLE:
		return EBADF;
	case ERROR_NOT_ENOUGH_MEMORY:
	case ERROR_OUTOFMEMORY:
		return ENOMEM;
	case ERROR_INVALID_DRIVE:
		return ENODEV;
	case ERROR_WRITE_PROTECT:
		return EROFS;
	case ERROR_FILE_EXISTS:
	case ERROR_ALREADY_EXISTS:
		return EEXIST;
	case ERROR_INVALID_PARAMETER:
		return EINVAL;
	case ERROR_BROKEN_PIPE:
		return EPIPE;
	case ERROR_DISK_FULL:
		return ENOSPC;
	case ERROR_CALL_NOT_IMPLEMENTED:
		return ENOSYS;
	case ERROR_NO_MORE_FILES:
		return ENFILE;
	default:
		return -1;
	}
}
