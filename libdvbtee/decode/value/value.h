/*****************************************************************************
 * Copyright (C) 2011-2015 Michael Ira Krufky
 *
 * Author: Michael Ira Krufky <mkrufky@linuxtv.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *****************************************************************************/

#ifndef __VALUE_H__
#define __VALUE_H__

#include <map>
#include <string>
#include <vector>

namespace dvbtee {

namespace decode {

class ValueBase {
public:
	enum Type {
		INTEGER,
		STRING,
		BOOLEAN,
		DOUBLE,
		OBJECT,
		ARRAY,
	};

	ValueBase(Type, std::string = "");
	virtual ~ValueBase();

	std::string toJson();

	const Type type;

	const std::string name;
};

#if 1
template <ValueBase::Type TYPE, typename T>
class Value : public ValueBase {
public:
	Value(std::string& n, T& v)
	 : ValueBase(TYPE, n)
	 , m_value(v)
	{}

	Value(T& v)
	 : ValueBase(TYPE)
	 , m_value(v)
	{}

	~Value()
	{}

	T& get()
	{
		return m_value;
	}

	void set(T& v)
	{
		m_value = v;
	}

private:
	T m_value;
};
#else
template <ValueBase::Type TYPE, typename T>
class Value : public ValueBase {
public:
	Value(std::string& n, T& v)
	 : ValueBase(TYPE, n)
	{
		pValue = new T(v);
	}

	Value(T& v)
	 : ValueBase(TYPE)
	{
		pValue = new T(v);
	}

	~Value()
	{
		delete pValue;
	}

	T& get()
	{
		return *pValue;
	}

	void set(T& v)
	{
		*pValue = v;
	}

private:
	T *pValue;
};
#endif

}

}

#endif /* __VALUE_H__ */