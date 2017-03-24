/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *                                                                         *
 *   Redistributions of source code must retain the above copyright        *
 *   notice, this list of conditions and the following disclaimer.         *
 *                                                                         *
 *   Redistributions in binary form must reproduce the above copyright     *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *                                                                         *
 *   Neither the name of Mike Kinghan nor the names of its contributors    *
 *   may be used to endorse or promote products derived from this software *
 *   without specific prior written permission.                            *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS    *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,   *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS *
 *   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED    *
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,*
 *   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF *
 *   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH  *
 *   DAMAGE.                                                               *
 *                                                                         *
 **************************************************************************/
#include "parameter_list_base.h"
#include <algorithm>
#include <cassert>

/** \file parameter_list_base.cpp
 *   This file implements `struct innards::parameter_list_base`
 */

using namespace std;
using namespace innards;

parameter_list_base::parameter_list_base(size_t n)
:	_params(n ? new vector<string>(n) : nullptr),
    _defect(none),_variadic(false)
{
    if (_params) {
        for (unsigned i = 0; i < size(); ++i) {
            (*_params)[i] = string("$") + to_string(i + 1);
        }
    }
}

string parameter_list_base::str() const
{
	string list;
	if (_params) {
		list.append(1,'(');
		auto start = _params->begin();
		auto const end = _params->end();
		for (	; start < end - 1; ++start) {
			list.append(*start);
			list.append(1,',');
		}
		list.append(*start);
		if (_defect != unclosed) {
			list.append(1,')');
		}
	}
	return list;
}

size_t parameter_list_base::which(std::string const & str) const
{
	if (!_params) {
		return -1;
	}
	auto where = find(_params->begin(),_params->end(),str);
	return where == _params->end() ? string::npos : where - _params->begin();
}

// EOF
