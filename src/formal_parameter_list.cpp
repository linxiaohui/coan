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
#include "formal_parameter_list.h"
#include "canonical.h"
#include "parse_buffer.h"

/** \file formal_parameter_list.cpp
 *   This file implements `struct formal_parameter_list`.
 */

using namespace std;

template<class CharSeq>
void formal_parameter_list::read(chewer<CharSeq> & chew)
{
	_defect = formal_parameter_list::none;
	if (*chew == '(') {
		unique_ptr<vector<string>> params(new vector<string>);
		chew(+1,greyspace);
		string param;
		for ( ; chew; chew(+1,greyspace)) {
			param = canonical<symbol>(chew);
			chew(greyspace);
			if (param.empty()) {
				if (*chew == '.') {
					chew(+1,continuation);
					if (*chew != '.') {
						_defect = formal_parameter_list::unclosed;
						break;
					}
					chew(+1,continuation);
					if (*chew != '.') {
						_defect = formal_parameter_list::unclosed;
						break;
					}
					_variadic = true;
					param = "...";
					chew(+1,continuation);
				} else if (*chew == ')') {
					break;
				} else {
					_defect = formal_parameter_list::non_param;
				}
			}
			if (*chew == ',') {
				if (_variadic) {
					_defect = formal_parameter_list::non_param;
				}
				else {
					params->push_back(param);
					if (param.empty()) {
						_defect = formal_parameter_list::empty_param;
					} else {
						param.clear();
					}
				}
			} else if (*chew == ')') {
				if (param.empty()) {
					if (params->size()) {
						_defect = formal_parameter_list::empty_param;
					}
				} else {
					params->push_back(param);
					param.clear();
				}
				break;
			}
		}
		if (!param.empty()) {
			params->push_back(param);
		}
		if (!chew || *chew != ')') {
			_defect = formal_parameter_list::unclosed;
		} else {
			++chew;
		}
		if (params->size()) {
			_params.reset(params.release());
		}
		return;
	}
	_params.reset();
}

template
void formal_parameter_list::read(chewer<string> & chew);
template
void formal_parameter_list::read(chewer<parse_buffer> & chew);

// EOF

