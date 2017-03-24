#ifndef PLATFORM_H
#define PLATFORM_H
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

#include "configure.h"

/**	\file platform.h
 *   This file defines macros for values that vary with the host environment.
 */

#if defined(_WIN32) || defined(_WIN64)
	/// \brief Character that delimits elements of a filesystem path for Windows.
	#define PATH_DELIM '\\'
	/// String that represents the root of the filesystem for Windows.
	#define FS_ROOT_PREFIX ""

	#ifdef _WIN64
		/// Name of the host operating system type when 64-bit Windows.
		#define OS_TYPE "64 bit Windows"
	#else
		/// Name of the host operating system type when 32-bit Windows.
		#define OS_TYPE "32 bit Windows"
	#endif

	/// This is a Windows build
	#define WINDOWS

	#include <stdlib.h>

	#ifndef PATH_MAX
		/// Maximum length of pathname.
		#define PATH_MAX _MAX_PATH
	#endif

	#ifdef __GNUC__
		/// This is a GNU Windows build (probably mingw)
		#define GNUWIN
	#endif

#else //!defined(_WIN32) && defined(_WIN64)

	// Character that delimits elements of a filesystem path for Unix/Linux.
	#define PATH_DELIM '/'
	/// String that represents the root of the filesystem for Unix/Linux.
	#define FS_ROOT_PREFIX "/"

	#ifdef __LP64__
	/// Name of the host operating system type when 64-bit Unix/Linux.
	#define OS_TYPE "64 bit Unix/Linux"
	#else
	/// Name of the host operating system type when 32-bit Unix/Linux.
	#define OS_TYPE "32 bit Unix/Linux"
	#endif

	/// This is a Unix/Linux build
	#define NIX

#endif /* ! _WIN32 || _WIN64*/

#ifdef _MSC_VER
	#if _MSC_VER < 1600
		#error Sorry, you need MS C++ >= 1600 (i.e. Visual C++ 2010) \
		or later to build coan
	#else
		// Disable VC++ warning for ignored exception specifications
		#pragma warning(disable : 4290)
		// Disable VC++ warning for converting BOOL to bool
		#pragma warning(disable : 4800)
		// Disable VC++ warning unary minus applied to unsigned type.
		#pragma warning(disable : 4146)
	#endif
#endif

#ifdef __clang__
	#if (__clang_major__ < 3) || (__clang_major__ == 3 && \
			__clang_minor__ < 1)
		#error Sorry, you need clang 3.1 or later to build coan
	#endif
#endif

#if defined(__GNUC__) && !defined(__clang__)
	#if (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
		#error Sorry, you need GCC 4.3 or later to build coan
	#endif // (__GNUC__ < 4) || (__GNUC__ == 4 && __GNUC_MINOR__ < 3)
#endif //  __GNUC__ && !__clang__

#endif // EOF
