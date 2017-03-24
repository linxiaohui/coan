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
#include "expansion_base.h"
#include "unexplained_expansion.h"
#include "explained_expansion.h"
#include "diagnostic.h"
#include "reference.h"
#include "citable.h"
#include <string>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>

/** \file expansion_base.cpp
 *   This file implements `struct expansion_base`.
 */

using namespace std;
using namespace parameter_substitution;

/*! \brief Return the quotation of a string.

	\param str The string to be quoted.

	\return `str` surrounded in quotes and with each embedded `\` or `"`
	preceded by a `\`
*/
static string quote(string const & str)
{
	string s(1,'\"');
	for (auto ch : str) {
		if (ch == '\\' || ch == '\"') {
			s += '\\';
		}
		s += ch;
	}
	s += '\"';
	return s;
}


expansion_base::expansion_base(reference const & ref)
: 	reference(ref),
	_value(ref.invocation())
{
	if (callee()->configured() && args() && callee()->parameters()) {
		set_expansion_flags();
	}
}

void expansion_base::set_expansion_flags()
{

	chewer<string> chew(chew_mode::plaintext,
        const_cast<string &>(callee()->format()->str()));
	for (chew(literal_space); chew; chew(literal_space)) {
		specifier spec = specifier::read(chew);
		if (spec) {
			size_t param_i = spec.get_param_index();
			switch(spec.get_handling()) {
			case handling::substitute_arg:
				args().set_expandable(param_i,false);
				break;
			case handling::substitute_quoted_arg: {
				args().set_expandable(param_i,false);
				auto quoted_arg = quote(args().at(param_i));
				args()[param_i] = quoted_arg;
				break;
            }
			case handling::substitute_expanded_arg:
				args().set_expandable(param_i,true);
				break;
			default:
				assert(false);
			}
			continue;
		}
		++chew;
	}
}

unsigned expansion_base::edit_buf(
	string & str,
	expansion_base const & e,
	size_t off )
{
    unsigned edits = 0;
	chewer<string> chew(chew_mode::plaintext,str,off);
	for (	;(off = identifier::find_first_in(e.id(),chew)) != string::npos;
				chew.sync()) {
		argument_list args(chew);
		if (e.args().str() == args.str()) {
			size_t len = size_t(chew) - off;
			edit(str,off,len,e.value());
			++edits;
		} else {
			chew = off + e.id().size();
		}
	}
	return edits;
}

unsigned expansion_base::edit_trailing_args(
	expansion_base const & e,
	size_t start)
{
    unsigned edits = 0;
    for (   ;start < args().size(); ++start) {
        if (args().is_expandable(start)) {
            edits += edit_buf(args()[start],e);
        }
    }
	return edits;
}


bool expansion_base::substitute()
{
    string s;
    string & format = const_cast<string &>(callee()->format()->str());
	chewer<string> chew(chew_mode::plaintext,format);
    while (chew) {
        size_t mark = size_t(chew);
        chew(literal_space);
        if (size_t(chew) > mark) {
            s += format.substr(mark,size_t(chew) - mark);
        }
        specifier spec = specifier::read(chew);
        if (spec) {
            size_t param_i = spec.get_param_index();
            size_t next_size = s.size() + args().at(param_i).size();
            if (next_size > max_expansion_size()) {
                warning_incomplete_expansion()
                << "Argument substitution in \"" << this->reference::invocation()
                << "\" not done. Will exceed max expansion size "
                << max_expansion_size() << " bytes" << emit();
                s = _value;
                break;
            }
            s += args().at(param_i);
        } else if (chew) {
            s += *chew;
            ++chew;
        }
    }
	if (s != _value) {
		_value = s;
		return true;
	}
	return false;
}

unique_ptr<expansion_base>
expansion_base::factory(bool explain, reference const & ref) {
    expansion_base * peb = explain ?
        dynamic_cast<expansion_base*>(new explained_expansion(ref))
        : dynamic_cast<expansion_base*>(new unexplained_expansion(ref));
    return unique_ptr<expansion_base>(peb);
}

// EOF
