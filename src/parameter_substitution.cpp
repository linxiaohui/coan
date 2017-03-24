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

#include "parameter_substitution.h"
#include "diagnostic.h"
#include "symbol.h"

/** \file parameter_substitution.cpp
 *   This file implements the members of `namespace parameter_substitution`.
 */

/// \cond NO_DOXYGEN

using namespace std;

namespace parameter_substitution {

string specifier::legible() const
{
	string s = "%{";
	s += to_string(get_param_index());
	s += ':';
	switch(get_handling()) {
	case handling::substitute_arg:
		s += "as-is";
		break;
	case handling::substitute_expanded_arg:
		s += "expanded";
		break;
	case handling::substitute_quoted_arg:
		s += "quoted";
		break;
	default:;
	}
	s += "}%";
	return s;
}

ptrdiff_t format::adjust_for_stringify_op(size_t pos, size_t nparams)
{
	size_t mark = pos;
	if (++pos >= _fmt.length() ||
			(pos += bool(isspace(_fmt[pos]))) >= _fmt.length()) {
		return 0;
	}
	size_t i_off;
	size_t h_off;
	auto found = specifier::get_at(_fmt,pos,i_off,h_off);
	if (found != string::npos) {
		_fmt[h_off] = char(handling::substitute_quoted_arg);
		_fmt.erase(mark,pos - mark);
		return mark - pos;
	} else if (nparams) {
		throw error_stringify_non_param();
	}
	return 0;
}

ptrdiff_t format::adjust_for_token_paste_op(size_t pos)
{
	size_t mark = pos;
	ptrdiff_t p(pos);
	while (--p >= 0 && isspace(_fmt[p])){}
	if (p < 0) {
		throw error_misplaced_token_paste();
	}
	pos = p;
	size_t i_off;
	size_t h_off;
	if (specifier::get_ending_at(_fmt,pos,i_off,h_off) != string::npos) {
		_fmt[h_off] = char(handling::substitute_arg);
	} else if (!identifier::is_valid_char(_fmt[pos]) && _fmt[pos] != '#') {
		throw error_bad_token_paste();
	}
	size_t start_cut = pos + 1;
	pos = mark + 2;
	for (	;pos < _fmt.length() && isspace(_fmt[pos]); ++pos){}
	if (pos >= _fmt.length()) {
		throw error_misplaced_token_paste();
	}
	if (specifier::get_at(_fmt,pos,i_off,h_off) != string::npos) {
		_fmt[h_off] = char(handling::substitute_arg);
	} else if (!identifier::is_valid_char(_fmt[pos]) && _fmt[pos] != '#') {
		throw error_bad_token_paste();
	}
	size_t cut_len = pos - start_cut;
	_fmt.erase(start_cut,cut_len);
	return -cut_len;
}

void format::do_stringify_adjustments(size_t nparams)
{
	ptrdiff_t netdiff = 0;
	for (size_t pos: _stringify_offs) {
		netdiff += adjust_for_stringify_op(pos + netdiff,nparams);
	}
}

void format::do_token_paste_adjustments()
{
	ptrdiff_t netdiff = 0;
	for (size_t pos: _token_paste_offs) {
		netdiff += adjust_for_token_paste_op(pos + netdiff);
	}
}

void format::build_format(symbol & sym)
{
	string & s = const_cast<string &>(*sym.defn());
	formal_parameter_list const & params = sym.parameters();
	chewer<string> chew(chew_mode::plaintext,s);
	while (chew) {
		auto mark = size_t(chew);
		size_t diff = mark - _fmt.length();
		chew(literal_space);
		if (size_t(chew) > mark) {
			_fmt += s.substr(mark,size_t(chew) - mark);
			if (!chew) {
				break;
			}
			continue;
		}
		chew(whitespace);
		if (size_t(chew) > mark) {
			if (!_fmt.size() || _fmt.back() != ' ') {
				_fmt += ' ';
			}
			if (!chew) {
				break;
			}
			continue;
		}
		if (*chew == '#') {
			_fmt += '#';
			if (++chew && *chew == '#') {
				_fmt += '#';
				_token_paste_offs.push_back(mark - diff);
				++chew;
				continue;
			}
			_stringify_offs.push_back(mark - diff);
			continue;
		}
		chew(name);
		if (size_t(chew) == mark) {
            _fmt += *chew;
            ++chew;
			continue;
		}
		string id = s.substr(mark,size_t(chew) - mark);
		size_t param_i = params.which(id);
		if (param_i != string::npos) {
			specifier spec(param_i,handling::substitute_expanded_arg);
			spec.append_to(_fmt);
		} else {
			_fmt += id;

		}
	}
	do_stringify_adjustments(params.size());
	do_token_paste_adjustments();
}

format::format(symbol & sym)
{
	if (!sym.defn()) {
		return;
	}
	try {
		build_format(sym);
	}
	catch(error_stringify_non_param & gripe) {
		gripe << "#-operator must precede a macro parameter in definition >>"
		<< *sym.defn() << "<< of \"" << sym.signature() << '\"' << emit();
	}
	catch(error_misplaced_token_paste & gripe) {
		gripe << "##-operator cannot begin or end the definition >>"
		<< *sym.defn() << "<< of \"" << sym.signature() << '\"' << emit();
	}
	catch(error_bad_token_paste & gripe) {
		gripe << "##-operator does not compose a token in definition >>"
		<< *sym.defn() << "<< of \"" << sym.signature() << '\"' << emit();
	}
}

std::string format::legible(string const & str)
{
	string s;
	chewer<string> chew(chew_mode::plaintext,const_cast<string &>(str));
	for (	;chew; chew(literal_space)) {
		size_t mark = size_t(chew);
		chew(literal_space);
		if (size_t(chew) > mark) {
			s += str.substr(mark,size_t(chew) - mark);
		}
		specifier spec = specifier::read(chew);
		if (spec) {
			s += spec.legible();
			continue;
		}
		s += *chew;
		++chew;
	}
	return s;
}

std::string format::legible() const
{
	return parameter_substitution::format::legible(_fmt);
}

} // namespace parameter_substitution.

/// \endcond NO_DOXYGEN

// EOF
