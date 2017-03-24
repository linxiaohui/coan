#ifndef DIRECTORY_COMMON_H
#define DIRECTORY_COMMON_H
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

/** \file directory_common.h
 *   This file defines `struct common::directory`
 */

#include <string>

/// Namespace containing OS-neutral directory functionality
namespace common
{

/// `struct common::directory` encapsulates OS-neutral directory functionality.
struct directory {
	/// Get the absolute path of the `directory`
	std::string const & path() const {
		return _abs_path;
	}
	/** \brief Get the last system error code resulting from an operation on
	 *	this directory.
	 */
	unsigned last_error() const {
		return _last_error;
	}

	/** \brief Clear any system error code resulting from an operation on this
	 *   directory.
	 */
	void clear_error() {
		_last_error = 0;
	}

	/// Say whether the directory is in a valid state.
	bool good() const {
		return !_last_error;
	}
	/// Explicitly Cast the directory to bool.
	explicit operator bool () const {
		return good();
	}

protected:

	/** \brief Explicitly construct a directory given a path.
	 *   \param path The pathname of the \c directory
	 */
	explicit directory(std::string const & path);

	/** \brief Say whether a file leafname consists of 1 or 2 dots
	 *   \param  leafname    The leafname to test.
	 *   \return True iff `leafname` consists of 1 or 2 dots
     *
	 *   The house-keeping directory entries "." and ".." are ignored
	 *   in file selection.
	 */
	static bool is_dot_name(char const *leafname) {
		return leafname[0] == '.' &&
		       (leafname[1] == 0 || (leafname[1] == '.' && leafname[2] == 0));
	}

	/// The absolute pathname of the directory
	std::string _abs_path;
	/// The last error system code returned by a directory operation.
	unsigned _last_error = 0;
};


} // namespace common

#endif
