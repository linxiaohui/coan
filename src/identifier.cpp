/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
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

#include "identifier.h"
#include "parse_buffer.h"
#include "canonical.h"
#include "diagnostic.h"
#include <cstring>

/** \file identifier.cpp
 *   This file implements the member functions of `namespace identifier`
 */

/// \cond NO_DOXYGEN

using namespace std;

namespace identifier {

template<class CharSeq>
string find_any_in(chewer<CharSeq> & chew, size_t & off)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	for(chew(literal_space); chew; chew(+1,literal_space)) {
		if (is_start_char(*chew)) {
			off = size_t(chew);
			string name(1,*chew);
			for (++chew; chew && is_valid_char(*chew); name += *chew,++chew) {}
			return name;
		}
	}
	return string();
}

template
string find_any_in(chewer<string> &, size_t &);
template
string find_any_in(chewer<parse_buffer> &, size_t &);

template<class CharSeq>
size_t
find_first_in(
	string const & id,
	chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	size_t len = id.length();
	char prior = 0;
	for(chew(literal_space); chew;
			prior = *chew,chew(+1,literal_space)) {
		if (!strncmp(&*chew,id.data(),len)) {
			size_t where = size_t(chew);
			chew += len;
			if (!is_valid_char(prior) &&
				(!chew || !is_valid_char(*chew))) {
				return where;
			}
			--chew;
		}
	}
	return string::npos;
}

template
size_t find_first_in(string const &, chewer<string> &);
template
size_t find_first_in(string const &, chewer<parse_buffer> &);

template<class CharSeq>
string read(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	string symname = canonical<symbol>(chew);
	if (!symname.length()) {
		error_not_identifier() <<
			   "Identifier needed instead of \""
			   << chew.buf().substr(size_t(chew)) << '\"' << emit();
	}
	return symname;
}

template
string read(chewer<string> &);
template
string read(chewer<parse_buffer> &);

} //namespace identifier

/// \endcond NO_DOXYGEN

// EOF
