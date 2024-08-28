// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "strlist.h"
#include "alloc.h"
#include "termsg.h"
#include "debug.h"

extern "C"{
const char *get_home_dir(void);

int calc_dir_size(const char *dir, off_t *size);

char *dirname(char *path);
}

const char *get_home_dir()
{
	return getenv("USERPROFILE");
}

static int dirent_sizeof(uintmax_t *size,
			 const std::filesystem::directory_entry &ent,
			 struct strlist *sl)
{
	using std::filesystem::file_type;

	const std::string &tmp = ent.path().string();
	const char *path = tmp.c_str();
	std::filesystem::file_type type = ent.status().type();

	switch (type) {
	case file_type::regular:
		*size += ent.file_size();
		break;
	case file_type::directory:
		strlist_push(sl, path);
		break;
	case file_type::symlink:
		*size += 64;
		break;
	case file_type::unknown:
		return error("can’t determine file type of ‘%s’", path);
	default:
		warn("‘%s’ has unsupported file type ‘%d’; file size retrieval for this file is skipped",
		     path, static_cast<int>(type));
	}

	return 0;
}

static int do_calc_dir_size(const std::filesystem::path &dir,
			    uintmax_t *size, struct strlist *sl)
{
	using std::filesystem::directory_entry;
	using std::filesystem::directory_iterator;

	int err;

	for (const directory_entry &ent : directory_iterator(dir)) {
		err = dirent_sizeof(size, ent, sl);
		if (err)
			return 1;
	}

	return 0;
}

static int my_calc_dir_size(const char *dir, off_t *size)
{
	int err;
	char *path = xstrdup(dir);
	struct strlist sl = STRLIST_INIT_DUPE;
	std::filesystem::path d = path;
	/*
	 * dir shall be an abs path, and if dir is abs path then, all
	 * calls to the ent.path().c_str() will return abs path
	 */
	BUG_ON(d.is_relative());

	uintmax_t sz = 0;
	do {
		d = path;
		err = do_calc_dir_size(d, &sz, &sl);
		free(path);
		if (err) {
			strlist_destroy(&sl);
			return 1;
		}
	} while ((path = strlist_pop(&sl)) != NULL);

	if (sz > max_int_val_of_type(off_t))
		sz = max_int_val_of_type(off_t);
	*size = sz;

	strlist_destroy(&sl);
	return 0;
}

int calc_dir_size(const char *dir, off_t *size)
{
	try {
		return my_calc_dir_size(dir, size);
	} catch (const std::exception &e) {
		return error("an exception occurred while calculating directory size for ‘%s’\n%s",
			     dir, e.what());
	}
}

char *dirname(char *path)
{
	std::filesystem::path p = path;
	const std::filesystem::path &dir = p.parent_path();
	const std::string &tmp = dir.string();
	memcpy(path, tmp.c_str(), tmp.length() + 1);
	return path;
}
