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

/** \file fs_nix.cpp
 * This file implements the member functions of namespace `fs`
 * for Unix-like systems.
*/
#include "platform.h"

/// \cond NO_DOXYGEN

#ifdef NIX

#include "filesys.h"
#include "diagnostic.h"
#include "syserr.h"
#include "path.h"
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <climits>
#include <cerrno>

using namespace std;
namespace fs {

string real_path(string const & relname)
{
	char buf[PATH_MAX];
	if (!realpath(relname.c_str(),buf)) {
		error_cant_get_realpath() <<  "Can't obtain real path of \""
			  << relname << "\": " << strerror(errno) << emit();
	}
	return buf;
}

string cwd()
{
	char buf[PATH_MAX];
	if (!getcwd(buf,PATH_MAX)) {
		abend_cant_get_cwd() <<  "Can't obtain real path of "
			 "current working directory: " << strerror(errno) << emit();
	}
	return buf;
}

bool is_absolute(std::string pathname)
{
	return !pathname.empty() && pathname[0] == '/';
}

obj_type_t obj_type(string const & name)
{
	obj_type_t type = OBJ_NONE;
	struct stat obj_info;
	int res = lstat(name.c_str(),&obj_info);
	if (!res) {
		if (S_ISLNK(obj_info.st_mode)) {
			type |= OBJ_SLINK;
		}
		res = stat(name.c_str(),&obj_info);
		if (!res) {
			if (S_ISREG(obj_info.st_mode)) {
				type |= OBJ_FILE;
			} else if (S_ISDIR(obj_info.st_mode)) {
				type |= OBJ_DIR;
			}
		}
	}
	return type;
}

void make_dir(std::string const & abs_path, bool recursive)
{
	mode_t mode = S_IRWXU | S_IXGRP | S_IRGRP | S_IXOTH | S_IROTH;
	int result = 0;
	if (recursive) {
		path_t path(abs_path);
		int elements = path.elements() - 1;
		errno = 0;
		for (int element = 0; !result && element < elements;) {
			string prefix = path.segment(0,++element).c_str();
			result =  mkdir(prefix.c_str(),mode);
			result *= (errno != EEXIST && errno != EISDIR);
		}
	}
	if (!result) {
		result = mkdir(abs_path.c_str(),mode);
		result *= (errno != EEXIST && errno != EISDIR);
	}
	if (result) {
		abend_cant_create_dir() << "Can't create directory \"" << abs_path
			<< "\": " << system_error_message(errno) << emit();
	}
}

permissions get_permissions(std::string const & filename)
{
	permissions p = -1;
	struct stat obj_info;
	int res = stat(filename.c_str(),&obj_info);
	if (!res) {
		p = obj_info.st_mode & 0777;
	}
	return p;
}

int set_permissions(std::string const & filename, permissions p)
{
	return chmod(filename.c_str(),p & 0777);
}

} // namespace fs

#endif

/// \endcond NO_DOXYGEN

/* EOF*/
