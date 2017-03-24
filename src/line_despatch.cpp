/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2002, 2003 Tony Finch <dot@dotat.at>.                   *
 *   All rights reserved.                                                  *
 *   Copyright (C) 1985, 1993 The Regents of the University of California. *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed by Mike Kinghan, imk@burroingroingjoing.com,              *
 *   derived from the code of Tony Finch                                   *
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

#include "line_despatch.h"
#include "citable.h"
#include "io.h"
#include "contradiction.h"
#include "if_control.h"
#include "canonical.h"
#include "directive.h"

/** \file line_despatch.cpp
 *  This file implements `struct line_despatch`
 */
using namespace std;

unsigned line_despatch::_lines_suppressed_(0);
unsigned line_despatch::_lines_changed_(0);
std::unique_ptr<parsed_line> line_despatch::_cur_line_(nullptr);

void line_despatch::top()
{
	_cur_line_.reset(new parsed_line(io::input(),io::output()));
}

void line_despatch::substitute(string const & replacement)
{
	if (options::have_source_output()) {
		*io::output() << replacement;
		--_lines_suppressed_;
		++_lines_changed_;
	}
}

line_type line_despatch::next()
{
	if (!_cur_line_->get()) {
		contradiction::flush();
		if_control::transition(LT_EOF);
		return LT_EOF;
	}
	chewer<parse_buffer> chew(!options::plaintext(),*_cur_line_);
	line_type retval = LT_PLAIN;
	chew(greyspace);
	if (*chew != '#') {
		contradiction::flush();
		chew(code);
		return retval;
	}
	_cur_line_->indent() = size_t(chew);
	chew(+1,greyspace);
	size_t keyword_off = size_t(chew);
	string keyword = canonical<symbol>(chew);
	cur_line().mark_keyword(keyword_off,keyword.length());
	retval = directive_base::eval(keyword,chew);
	if (retval == LT_DIRECTIVE_KEEP) {
		/* A non-if directive is the same as a non-directive for if-control */
		retval = LT_PLAIN;
	}
	return retval;
}

string line_despatch::pretty() {
	return citable(!options::plaintext(),_cur_line_->str());
}

/* EOF*/
