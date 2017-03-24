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
#include "unexplained_expansion.h"
#include "diagnostic.h"

/** \file unexplained_expansion.cpp
 *   This file implements `struct unexplained_expansion`.
 */

/// \cond NO_DOXYGEN

using namespace std;

unsigned unexplained_expansion::expand()
{
	unsigned edits = 0;
	if (args()) {
		for (	auto i = seek_expandable_arg();
				i < args().size(); ++_cur_arg,
				i = seek_expandable_arg()) {
			edits += expand(args()[i]);
		}
		edits += substitute();
	} else if (!callee()->defn()) {
		_value = "0";
		++edits;
	} else if (callee()->defn()->empty()) {
		_value = "";
		++edits;
	}
	else {
		_value = callee()->format()->str();
		++edits;
	}
	return edits += expand(_value);
}

unsigned unexplained_expansion::expand(string & str)
{
	unsigned edits = 0;
	symbol::locator sym;
	size_t off;
	chewer<string> chew(chew_mode::plaintext,str);
	for (	;(sym = symbol::find_any_in(chew,off)),sym; chew.sync()) {
		reference ref(sym,chew,this);
		if (!sym->configured() && !sym->in_progress()) {
			chew = off + sym->id().length();
			continue;
		}
		unexplained_expansion e(ref);
		try {
            if (!e.expand()) {
                continue;
            }
		}
		catch(expansion_base const & eb) {
            warning_incomplete_expansion()
                << "Macro expansion of \"" << eb.reference::invocation()
                << "\" abandoned. Will exceed max expansion size "
                << max_expansion_size() << " bytes" << emit();
            throw_self();
		}
		size_t len = size_t(chew) - off;
		if (len && str.compare(off,len,e.value()) == 0) {
			continue;
		}
        edit(str,off,size_t(chew) - off,e.value());
        ++edits;
        edits += edit_buf(str,e,off + e.invocation().size());
        edits += edit_trailing_args(e,_cur_arg + 1);
	}
	return edits;
}

/// \endcond NO_DOXYGEN

//EOF
