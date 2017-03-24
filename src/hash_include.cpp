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
#include "hash_include.h"
#include "diagnostic.h"
#include "citable.h"
#include "options.h"
#include "io.h"
#include "canonical.h"
#include "reference.h"
#include <iostream>

/** \file hash_include.cpp
 *   This file implements `struct hash_include`.
 */

using namespace std;

bool hash_include::valid() const
{
	string fn = filename();
	size_t len = fn.length();
	bool valid = len > 2U && len == fn.length();
	if (valid) {
		valid = ((fn[0] == '<' && fn[len -1] == '>') ||
		         (fn[0] == '\"' && fn[len - 1] == '\"'));
	}
	return valid;
}

string hash_include::read(chewer<parse_buffer> & chew)
{
	string arg;
	if (identifier::is_start_char(*chew)) {
		string id = identifier::read(chew);
		symbol::locator sloc(id);
		_ref.reset(new reference(sloc,chew));
		arg = _ref->invocation();
	} else {
		size_t startoff = size_t(chew);
		size_t endoff = size_t(chew(header_name));
		if (startoff < endoff) {
			chew.cursor(startoff);
			arg = citable(chew,endoff - startoff);
		} else {
			arg = canonical<string>(chew);
		}
	}
	return arg;
}

void hash_include::report()
{
	if (!line_despatch::cur_line().reportable() ||
			options::get_command() == CMD_SYMBOLS) {
		return;
	}
	shared_ptr<reference> ref = symbolic_argument();
	if (_directive.reported()) {
		if (ref && ref->callee()->configured() && !ref->reported()) {
			_directive.set_reported(false);
		}
	}
	if (_directive.reported() && options::list_only_once()) {
		return;
	}
	if ((options::list_local_includes() == options::list_system_includes()) ||
	    (options::list_local_includes() && local_header()) ||
	    (options::list_system_includes() && system_header())) {
		cout << "#include ";
		if (ref) {
			cout << _directive.argument() << ": symbolic argument, ";
			if (ref->callee()->configured()) {
				if (ref->callee()->self_referential()) {
					cout << "insoluble, because of circular definitions";
				} else {
					cout << "expands as >>" << filename() << "<<";
				}
			} else {
				cout << "insoluble, because unconfigured";
			}

		} else {
			cout << filename();
		}
		if (options::list_location()) {
			cout << ": " << io::in_file_name()
			<< '(' << line_despatch::cur_line().num() << ')';
		}
		cout << '\n';
		_directive.set_reported();
	}
}

/* EOF */
