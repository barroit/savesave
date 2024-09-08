/* SPDX-License-Identifier: GPL-3.0-only */
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#ifndef AUTOVAR_HPP
#define AUTOVAR_HPP

template <typename type_name>
class autovar {
public:
	typedef type_name (*initializer)();
	typedef void (*finalizer)(type_name *);

private:
	type_name resource;
	finalizer cleanup;

public:
	autovar(initializer init, finalizer cln)
		: resource(init()), cleanup(cln) {}

	~autovar()
	{
		cleanup(&resource);
	}

	operator type_name &() = delete;

	operator type_name *()
	{
		return &resource;
	}
};

#endif /* AUTOVAR_HPP */
