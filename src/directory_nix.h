#ifndef DIRECTORY_NIX_H
#define DIRECTORY_NIX_H
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
#include <dirent.h>
#include <cerrno>

/** \file directory_nix.h
 *   This file defines `struct nix::directory`
 */

/// Namespace containing linux/unix-specific directory functionality
namespace nix
{

/** \brief Class `nix::directory` encapsulates linux/unix-specific directory
 *   functionality.
 */
struct directory : common::directory, private no_copy {
	/** \brief Explicitly construct a directory given a path.
	 *   \param path The pathname of the directory.
	 */
	explicit directory(std::string const & path)
		: common::directory(path),_dir(nullptr) {
		open();
	}

	/// Destructor
	~directory() {
		close();
	}

	/// Get the leafname of the current member of the directory.
	std::string const cur_memb() const {
		return _entry.d_name;
	}

	/** \brief Open the directory.
	 *   \return True if the directory is opened.
	 */
	bool open() {
		_dir = opendir(_abs_path.c_str());
		return !_dir ? !(_last_error = errno) : true;
	}

	/** \brief Close the directory.
	 *   \return True if the directory is closed.
	 */
	bool close() {
		return (_dir && closedir(_dir) != 0) ? !(_last_error = errno) : true;
	}

	/** \brief Move to the next entry in the directory.
     *
	 *   \return The leafname of the next entry, if any, else an empty string.
	 */
	std::string  next() {
		struct dirent * entry = nullptr;
		_entry.d_name[0] = '\0';
		int error = readdir_r(_dir,&_entry,&entry);
		if (error) {
			_last_error = error;
		} else if (entry) {
			if (is_dot_name(_entry.d_name)) {
				return next();
			}
		}
		return _entry.d_name;
	}

private:

	/// The directory handle of this `directory` handle
	DIR * _dir;
	//! The current entry in this `directory`.
	struct dirent _entry;
};

} //namespace nix

#endif
