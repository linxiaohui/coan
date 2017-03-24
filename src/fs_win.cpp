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

/** \file fs_win.cpp
 * This file implements the member functions of namespace `fs` for Windows
 */

#include "platform.h"

#ifdef WINDOWS

#include "filesys.h"
#include "syserr.h"
#include "diagnostic.h"
#include "path.h"
#include <cassert>
#include <ctype.h>
#include <stdlib.h>
#include <windows.h>
#include <direct.h>

using namespace std;

namespace fs {

string real_path(string const & relname)
{
	char buf[MAX_PATH] = "";
	_fullpath(buf,relname.c_str(),MAX_PATH);
	return buf;
}

std::string cwd()
{
	char buf[MAX_PATH] = "";
	GetCurrentDirectory(MAX_PATH,buf);
	return buf;
}

bool is_absolute(std::string pathname)
{
	return ((!pathname.empty() && pathname[0] == '\\') ||
	        (pathname.length() >= 3 && pathname[1] == ':' &&
				pathname[2] == '\\'));
}

obj_type_t obj_type(string const & name)
{
	WIN32_FILE_ATTRIBUTE_DATA obj_info;
	int res = GetFileAttributesEx(name.c_str(),GetFileExInfoStandard,&obj_info);
	if (res) {
		if (obj_info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			return OBJ_DIR;
		} else {
			return OBJ_FILE;
		}
	}
	return OBJ_NONE;
}

void make_dir(std::string const & abs_path, bool recursive)
{
	int result = 0;
	if (recursive) {
		path_t path(abs_path);
		int elements = int(path.elements()) - 1;
		errno = 0;
		for (int element = 0; !result && element < elements;) {
			result =  _mkdir(path.segment(0,++element).c_str());
			result *= errno != EEXIST;
		}
	}
	if (!result) {
		result = _mkdir(abs_path.c_str());
		result *= errno != EEXIST;
	}
	if (result) {
		abend_cant_create_dir() << "Can't create directory \"" << abs_path
			<< "\": " << system_error_message(errno) << emit();
	}
}

permissions get_permissions(std::string const & filename)
{
	return -1;
}

int set_permissions(std::string const & filename, permissions p)
{
	return 0;
}

} // namespace fs
#endif

/* EOF*/
