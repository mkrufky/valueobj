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

#include <stdio.h>
#include <sstream>
#include "array.h"
#include "value-macros.h"

using namespace dvbtee::decode;

#define DBG 0

static ReferencedValueUndefined& valueUndefined = ReferencedValueUndefined::instance();

Array::Array(std::string idx)
 : idxField(idx)
{
#if DBG
	fprintf(stderr, "%s\n", __func__);
#endif
}

Array::~Array()
{
#if DBG
	fprintf(stderr, "%s\n", __func__);
#endif
	clear();
}

Array::Array(const Array &obj)
{
	for (KeyValueVector::const_iterator it = obj.vector.begin(); it != obj.vector.end(); ++it) {
		ValueBase *v = *it;
		push(v);
		const std::string& n = v->getName();
		if (n.length()) updateIndex(n, v);
	}
#if DBG
	fprintf(stderr, "%s(copy) %lu\n", __func__, vector.size());
#endif
}

const std::string Array::toJson() const
{
	std::stringstream s;

	if (!vector.size()) return "[]";

	int count = 0;

	s << "[ ";

	for (KeyValueVector::const_iterator it = vector.begin(); it != vector.end(); ++it) {
		if (count) s << ", ";

		s << (*it)->toJson();

		count++;
	}
	s << " ]";

	return s.str();

}

const size_t Array::size() const
{
	return vector.size();
}

const std::string &Array::getIndex() const
{
	return idxField;
}

const ValueBase* Array::get(unsigned int idx) const
{
	if (idx <= vector.size())
		return vector[idx];

	return &valueUndefined;
}

void Array::updateIndex(std::string key, const ValueBase *val)
{
	if (key.length()) indices[key] = val;
}

std::string &Array::assignIndex(Object &obj, std::string &index)
{
	if (idxField.length()) {
		const ValueBase *val = obj.get(idxField);
		index = (typeid(std::string) == val->getType()) ?
			obj.get<std::string>(idxField) : index = val->toJson();
	}

	return index;
}

const ValueBase* Array::getByName(std::string idx) const
{
	std::map<std::string, const ValueBase*>::const_iterator it = indices.find(idx);
	if (it == indices.end())
		return &valueUndefined;

	return it->second;
}

const ValueBase* Array::getByName(unsigned int idx) const
{
	return getByName(intToStr(idx));
}

void Array::clear()
{
	for (KeyValueVector::iterator it = vector.begin(); it != vector.end(); ++it)
	{
		// decrement refcount. if refcount becomes zero, delete
		if (0 == (--(**it)).getRefCnt()) delete *it;
	}
	vector.clear();
	indices.clear();
}

const std::string Array::intToStr(int i) const
{
	std::stringstream s;
	s << i;
	return s.str();
}

const ValueBase* Array::pushObject(Object &val, std::string idx)
{
	bool extractIndex = (!idx.length());

	if (extractIndex) assignIndex(val, idx);

	const ValueBase *v = pushByRef<Object>(val, idx);

	if (extractIndex) updateIndex(idx, v);

	return v;
}

const ValueBase* Array::push(Object &o)
{
	return pushObject(o, "");
}

const ValueBase *Array::push(Object *o)
{
	return push(*o);
}

const ValueBase* Array::push(ValueBase *val)
{
	vector.push_back(val);
	++(*val);
	return val;
}

DEFINE_DEFAULT_GETTERS(Array, unsigned int)
TO_JSON_TPL(Array, VALUE.toJson().c_str())
