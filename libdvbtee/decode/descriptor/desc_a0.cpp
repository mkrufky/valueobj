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

#include "string.h"

#include "desc_a0.h"

#include "dvbpsi/dr_a0.h" /* Extended Channel Name descriptor */

#include "functions.h"

#define CLASS_MODULE "[ext channel name]"

#define dprintf(fmt, arg...) __dprintf(DBG_DESC, fmt, ##arg)

using namespace dvbtee::decode;

static std::string DESC_NAME = "DR[A0]";

desc_a0::desc_a0(Decoder *parent, dvbpsi_descriptor_t *p_descriptor)
 : Descriptor(parent, DESC_NAME, p_descriptor)
{
	if (!desc_check_tag(m_tag, 0xa0)) return;

	dvbpsi_extended_channel_name_dr_t *dr = dvbpsi_ExtendedChannelNameDr(p_descriptor);
	if (desc_dr_failed(dr)) return;

	unsigned char name[256] = { 0 };
	decode_multiple_string(dr->i_long_channel_name, dr->i_long_channel_name_length, name, sizeof(name));

	set("extChannelName", std::string((const char *)name));

	dprintf("%s", toJson().c_str());

	setValid(true);
}

desc_a0::~desc_a0()
{
	//
}

REGISTER_DESCRIPTOR_FACTORY(0xa0, desc_a0);
