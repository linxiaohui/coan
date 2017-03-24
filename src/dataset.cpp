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

#include "dataset.h"
#include "if_control.h"
#include "chew.h"
#include "io.h"
#include "diagnostic.h"
#include "line_despatch.h"
#include <iostream>
#include <algorithm>

/** \file dataset.cpp
 *   This file implements `struct dataset`
 */

using namespace std;

/*! Debugging function to facilitate a debug break when
    a known line of an input file is reached

    \param line If a negative value is passed then
        source line numbers are written to the standard output.
        Otherwise "Here" is written to the standard output when
        the specified source line number is reached.
*/
static void
line_debug(int line)
{
	if (line < 0) {
		/* Call with negative value to write line numbers*/
		cout << line_despatch::cur_line().num() << '\n';
	} else if (line > 0 && (unsigned)line == line_despatch::cur_line().num()) {
		/* Call with positive line number and put breakpoint here ...*/
		cerr << "Here\n";
	}
}

file_tree dataset::_ftree_;
dataset::selector dataset::_selector_;
dataset::driver dataset::_driver_;

dataset::selector::selector(string const & extensions)
	: _files(0)
{
	if (extensions.length()) {
		size_t start = 0;
		size_t comma = extensions.find_first_of(',');
		for (   ; comma != string::npos;
		        comma = extensions.find_first_of(',',start = comma + 1)) {
			_filter_extensions.push_back(
			    extensions.substr(start,comma - start));
		}
		_filter_extensions.push_back(extensions.substr(start));
	}
}

bool dataset::selector::operator()(string const & filename)
{
	bool verdict = false;
	if (!_filter_extensions.size()) {
		verdict = true;
	} else {
		size_t leaf_off = filename.find_last_of(PATH_DELIM);
		if (leaf_off == string::npos) {
			leaf_off = 0;
		}
		string::const_reverse_iterator start = filename.rbegin();
		string::const_reverse_iterator end(filename.begin() + leaf_off);
		string::const_reverse_iterator extn_pos = std::find(start,end,'.');
		if (extn_pos != end) {
			string extension(extn_pos.base(),filename.end());
			verdict = find(_filter_extensions.begin(),
			               _filter_extensions.end(),extension) !=
			          _filter_extensions.end();
		}
	}
	if (verdict) {
		_files += verdict;
		progress_added_file() <<
			"To do (" << _files << ") \"" << filename << '\"' << emit();
	}
	return verdict;
}

void dataset::driver::at_file(string const & filename)
{
	unsigned error = 0;
	line_type lineval;
	if_control::top();
	progress_processing_file() << "Processing file (" <<
		++_done_files << ") \""	<< filename << '\"' << emit();
	io::open(filename);
	try {
		while ((lineval = line_despatch::next()) != LT_EOF) {
			line_debug(0);
			if (lineval == LT_DIRECTIVE_DROP) {
				line_despatch::drop();
			} else {
				if_control::transition(lineval);
			}
		}
	} catch(unsigned ex) {
		error = ex;
		++_error_files;

	}
	io::close(error);
}

void dataset::add(string const & path)
{
	_ftree_.add(path,_selector_);
}

/* EOF*/
