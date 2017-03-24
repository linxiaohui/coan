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
#include "argument_list.h"
#include "canonical.h"
#include "parse_buffer.h"

/** \file argument_list.cpp
 *   This file implements `struct argument_list`.
 */

using namespace std;

template<class CharSeq>
void argument_list::read(chewer<CharSeq> & chew)
{
	_defect = argument_list::none;
	if (*chew == '(') {
		unique_ptr<vector<string>> args(new vector<string>) ;
		chew(+1,greyspace);
		string arg;
		while (chew) {
			arg = canonical<macro_argument>(chew);
			if (*chew == ',') {
				args->push_back(arg);
				if (arg.empty()) {
					_defect = argument_list::empty_param;
				}
				arg.clear();
				++chew;
			} else if (*chew == ')') {
				if (arg.empty()) {
					if (args->size()) {
						_defect = argument_list::empty_param;
					}
				} else {
					args->push_back(arg);
					arg.clear();
				}
				break;
			}
		}
		if (!arg.empty()) {
			args->push_back(arg);
		}
		if (!chew || *chew != ')') {
			_defect = argument_list::unclosed;
		} else {
			++chew;
		}
		if (args->size()) {
			_params.reset(args.release());
			_expand_flags.reset(new vector<bool>(_params->size(),false));
		}
		return;
	}
	_params.reset();
    _expand_flags.reset();
}

template
void argument_list::read(chewer<string> & chew);
template
void argument_list::read(chewer<parse_buffer> & chew);

// EOF
