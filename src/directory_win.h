#ifndef DIRECTORY_WIN_H
#define DIRECTORY_WIN_H
#pragma once
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

#include "prohibit.h"
#include "directory_common.h"
#include <windows.h>


/** \file directory_win.h
 *   This file defines `struct win::directory`
 */

/// Namespace containing windows-specific directory functionality
namespace win
{

/** \brief `struct win::directory` encapsulates a MS-Windows specific
 *   directory functionality.
 */
struct directory : common::directory, private no_copy {
	/** \brief Explicitly construct given a path
	 *   \param path The pathname of the directory.
	 */
	explicit directory(std::string const & path)
		: common::directory(path),_handle(INVALID_HANDLE_VALUE){}

	/// Destructor
	~directory() {
		close();
	}

	/// Get the leafname of the current member of the directory.
	std::string cur_memb() const {
		return _obj_info.cFileName;
	}

	/** \brief Open the directory.
	 *   \return True if the directory is opened.
	 */
	bool open() {
		return true;
	}

	/** \brief Close the directory.
	 *   \return True if the `directory` is closed.
	 */
	bool close() {
		return (_handle != INVALID_HANDLE_VALUE && !FindClose(_handle)) ?
		       !(_last_error = GetLastError()) : true;
	}

	/** \brief Move to the next entry in the directory.
     *
	 *   \return The leafname of the next entry, if any, else an empty string.
	 */
	std::string next() {
		bool found = true;
		int errnum = 0;
		_obj_info.cFileName[0] = '\0';
		if (_handle != INVALID_HANDLE_VALUE) {
			found = FindNextFile(_handle,&_obj_info);
			if (!found) {
				errnum = GetLastError();
				if (errnum == ERROR_NO_MORE_FILES) {
					errnum = 0;
				}
			}
		} else {
			path_t wildcard(_abs_path);
			wildcard.push_back("*");
			_handle = FindFirstFile(wildcard.str().c_str(),&_obj_info);
			if (_handle == INVALID_HANDLE_VALUE) {
				int errnum = GetLastError();
				if (errnum == ERROR_FILE_NOT_FOUND) {
					errnum = 0;
				}
			}
		}
		if (errnum) {
			_last_error = errnum;
		} else if (found && is_dot_name(_obj_info.cFileName)) {
			return next();
		}
		return _obj_info.cFileName;
	}

private:

	/// Handle returned by FindFirstFile()
	HANDLE _handle;
	/// Search data updated by FindFirstFile or FindFile()
	WIN32_FIND_DATA _obj_info;
};

} // namespace

#endif
