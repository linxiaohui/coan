/***************************************************************************
 *   Copyright (C) 2013 Mike Kinghan, imk@burroingroingjoing.com           *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed originally by Mike Kinghan, imk@burroingroingjoing.com    *
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
 *   Neither the name of Mike Kinghan nor the names of its                 *
 *   contributors may be used to endorse or promote products derived from  *
 *   this software without specific prior written permission.              *
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

#include "citable.h"
#include <cassert>

/** \file citable.cpp
 *	This file implements the overloads of free function `citable`
 */

/// \cond NO_DOXYGEN

using namespace std;

namespace innards {

template<class CharSeq>
std::string citable(chewer<CharSeq> & chew, size_t len)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	std::string citable;
	bool escaped = false;
	size_t ndx = 0;
	char const space = ' ';
	char last_ch = 0;
	if (len == std::string::npos) {
		len = chew.remaining();
	}
	chew(greyspace);
	for (   ; ndx < len; ++ndx, ++chew) {
		if (!last_ch && isspace(*chew)) {
			continue;
		}
		switch(*chew) {
		case '\\':
			if (!escaped) {
				if (ndx + 2 < len &&
				    chew.atoff(1) == '\r' &&
				    chew.atoff(2) == '\n') {
					last_ch = *chew;
					ndx += 2;
					chew += 2;
				} else if (ndx + 1 < len && chew.atoff(1) == '\n') {
					last_ch = *chew;
					++ndx;
					++chew;

				} else {
					citable.append(1,*chew);
					last_ch = *chew;
					escaped = true;
				}
				continue;
			}
			break;
		case '\r':
			if (ndx + 1 < len &&
			    chew.atoff(1) == '\n') {
				++ndx;
				++chew;
			}
			if (last_ch != space) {
				citable.append(1,space);
				last_ch = space;
			}
			break;
		case '\n':
		case '\t':
		case space:
			if (last_ch != space) {
				citable.append(1,space);
				last_ch = space;
			}
			break;
		default:
			if (isprint(*chew)) {
				citable.append(1,*chew);
				last_ch = *chew;
			}
		}
		escaped = false;
	}
	ndx = citable.length();
	if (ndx) {
		while (ndx-- > 0 && isspace(citable[ndx])) {}
		citable.resize(ndx + 1);
	}
	return citable;
}

} // namespace innards

string citable(chewer<std::string> & chew, size_t len)
{
    return innards::citable(chew,len);
}
std::string citable(chewer<parse_buffer> & chew, size_t len)
{
    return innards::citable(chew,len);
}

string citable(integer const & integ)
{
	switch(integ.type()) {
	case INT_INT:
		return to_string((int)integ.raw());
	case INT_UINT:
		return to_string((unsigned)integ.raw());
	case INT_LONG:
		return to_string((long)integ.raw());
	case INT_ULONG:
		return to_string((unsigned long)integ.raw());
	case INT_LLONG:
		return to_string((long long)integ.raw());
	case INT_ULLONG:
		return to_string((unsigned long long)integ.raw());
	default:
		assert(false);
	}
	return string();
}

/// \endcond NO_DOXYGEN

// EOF


