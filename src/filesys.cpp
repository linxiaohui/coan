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

#include "filesys.h"
#include "diagnostic.h"
#include "path.h"
#include <iomanip>

/*! \file filesys.cpp
    This file implements the class `fs`
*/

/// \cond NO_DOXYGEN

using namespace std;

string fs::tempname(string const & format)
{
	size_t ndx = format.find("XXXXXX");
	if (ndx != string::npos && ndx + 6 == format.length()) {
		ostringstream ostr(format.substr(0,ndx));
		ostr.seekp(ndx);
		for ( unsigned i = 0; i <= 0xffffff; ++i,ostr.seekp(ndx)) {
			ostr << setw(6) << setfill('0') << hex << i;
			if (obj_type(ostr.str()) == OBJ_NONE) {
				return ostr.str();
			}
		}
	}
	return string();
}

string fs::abs_path(string const & filename)
{
	obj_type_t type = obj_type(filename);
	if (type != OBJ_NONE) {
		return real_path(filename);
	} else if (fs::is_absolute(filename)) {
		return filename;
	} else {
		path_t cwd_path(cwd());
		cwd_path += filename;
		return cwd_path.str();
	}
}

/// \endcond NO_DOXYGEN

/* EOF*/
