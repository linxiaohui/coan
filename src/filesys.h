#ifndef FILESYS_H
#define FILESYS_H
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
#include <string>

/** \file filesys.h
 *	This file defines an API of file-syetem operations
 */

/// Namespace `fs` encloses filesystem operations.
namespace fs {

/// Symbolic constants denoting filesystem objects.
enum {
    /// No such object
    OBJ_NONE,
    /// A symbolic link
    OBJ_SLINK = 1,
    /// A file
    OBJ_FILE = 2,
    /// A directory
    OBJ_DIR = 4
};

/// Abstract type of filesystem object types.
using obj_type_t = unsigned;

/** \brief Say whether an object type is a file.
 *  \param  type    The object type to be tested.
 *  \return True iff `type` denotes a file.
 */
inline bool is_file(obj_type_t type)
{
    return (type & OBJ_FILE) != 0;
}

/** \brief Say whether an object type is a directory.
 *  \param  type    The object type to be tested.
 *  \return True iff `type` denotes a directory.
 */
inline bool is_dir(obj_type_t type)
{
    return (type & OBJ_DIR) != 0;
}

/** \brief Say whether an object type is a symbolic link.
 *  \param  type    The object type to be tested.
 *  \return True iff `type` denotes a symbolic link.
*/
inline bool is_slink(obj_type_t type)
{
    return (type & OBJ_SLINK) != 0;
}

/** \brief Get the type of the object putatively designated
 *   by a filename.
 */
extern obj_type_t obj_type(std::string const & name);

/** \brief Get the absolute real pathname of a file or directory name.
 *
 *  Get the absolute real pathname of a file or directory name
 *  (which may be the name of a symbolic link).
 *
 *  \param relname	Possibly relative file or directory name.
 *
 *  \return The absolute real pathname of `relname`.
 */
extern std::string real_path(std::string const & relname);

/** \brief Get the absolute pathname for a filename.
 *  \param	filename An absolute or relative pathname that
 *      need not refer to any existing object.
 *  \return If `filename` refers to an existing object the returned
 *  string is as per `real_path`. Otherwise, if `filename`
 *  is an absolute path it is returned unchanged. Otherwise
 *  it is returned prefixed with the current working directory.
 */
extern std::string abs_path(std::string const & filename);

/// Say whether a filename is absolute or relative.
extern bool is_absolute(std::string pathname);

/// Get the absolute real pathname of the current working directory.
extern std::string cwd();

/** \brief Create a tempory filename from a template.
 *
 *  \param	format A pathname pattern from which to compose
 *               the temporary filename.
 *  \return A temporary filename if one can be created,
 *      otherwise an empty string.
 *
 *  The string `format` must terminate with `XXXXXX`
 *  This suffix will be replaced, if possible, with a string of
 *  characters to compose a filename different from that of
 *  any existing file.
 */
extern std::string tempname(std::string const & format);

/** \brief Create a directory given an absolute path name.
 *   \param abs_path The absolute pathname of the directory to create.
 *   \param recursive If true, then any non-existent prefixes of
 *   the required directory will also be created. Otherwise,
 *   the function will give a fatal error if any prefix does not exist.
 *
 *  The function suceeds if `abs_path` is an existing directory.
 */
extern void make_dir(std::string const & abs_path, bool recursive = true);

/// Type of file permissions mask.
using permissions = int;

/** Get the permissions of a file.
 *
 *   \param filename The file to query
 *   \return The file-permission bits of `filename` as `permissions`,
 *   unless error, in which case -1
 *
 *   On Windows the function always returns -1
 *
 */
extern permissions get_permissions(std::string const & filename);

/** Set the permissions of a file.
 *
 *   \param filename The file to modify
 *   \param p The permissions to apply.
 *   \return 0 unless error, in which case -1
 *
 *   The permission bits of `p` are applied to `filename`.
 *
 *   On Windows the function is a no-op.

 */
extern int set_permissions(std::string const & filename, permissions p);

} // namespace fs

#endif // EOF
