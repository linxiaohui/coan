/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
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
 *   Neither the name of Symbian Software Ltd. nor the names of its        *
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

#include "contradiction.h"
#include "diagnostic.h"
#include "io.h"
#include "if_control.h"
#include "line_despatch.h"
#include "canonical.h"

/** \file contradiction.cpp
 *   This file implements class `contradiction`
 */
using namespace std;

string contradiction::_subst_text_;
string	contradiction::_last_conflicted_symbol_;
contradiction_policy contradiction::_policy_ = CONTRADICTION_COMMENT;
unsigned contradiction::_policy_code_(0);


void contradiction::insert_pending()
{
	diagnostic_base::flush(_policy_code_);
	_policy_code_ = 0;
	if (_policy_ != CONTRADICTION_DELETE) {
		line_despatch::substitute(_subst_text_);
		if (_policy_ == CONTRADICTION_ERROR) {
			warning_error_generated() << "An #error directive was generated "
			                          " by the --conflict policy" << emit();
			if (if_control::is_unconditional_line()) {
				warning_unconditional_error_output() <<
					"An operative #error directive was outpout" << emit();
			}
		}
	}
}

void contradiction::insert(cause why,string const & symname)
{
	contradiction::save(why,symname);
	insert_pending();
}

void contradiction::flush()
{
	if (_policy_code_) {
		insert_pending();
	}
}

void contradiction::forget()
{
	diagnostic_base::discard(_policy_code_);
	_policy_code_ = 0;
}

void contradiction::save(cause why, string const & symname)
{
	_last_conflicted_symbol_ = symname;
	string line = canonical<string>(line_despatch::cur_line().str());
	string gripe("\"");
	switch(why) {
	case cause::DIFFERENTLY_REDEFING_D:
		gripe += line + "\" differently redefines -D symbol";
		break;
	case cause::DEFINING_U:
		gripe += line + "\" contradicts -U or --implicit";
		break;
	case cause::UNDEFING_D:
		gripe += line + "\" contradicts -D symbol";
		break;
	default:
		assert(false);
	}
	stringstream ss("error : inserted by coan: ");
	ss.seekp(0,ios_base::end);
	ss << gripe << " at " << io::in_file_name() << '(' <<
	   line_despatch::cur_line().num() << ")\n";
	switch(_policy_) {
	case CONTRADICTION_DELETE: {
		warning_deleted_contradiction diagnostic;
		_policy_code_ = diagnostic.encode();
		diagnostic << gripe << defer();
		break;
	}
	case CONTRADICTION_COMMENT: {
		warning_commented_contradiction diagnostic;
		_policy_code_ = diagnostic.encode();
		diagnostic << gripe << defer();
		_subst_text_ = string("// ") + ss.str();
		break;
	}
	case CONTRADICTION_ERROR: {
		warning_errored_contradiction diagnostic;
		_policy_code_ = diagnostic.encode();
		diagnostic << gripe << defer();
		_subst_text_ = string("#") + ss.str();
		break;
	}
	default:
		assert(false);
	}
}

/* EOF*/
