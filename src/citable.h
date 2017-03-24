#ifndef CITABLE_H
#define CITABLE_H
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

#include "parse_buffer.h"
#include "integer.h"
#include "chew.h"
#include <string>

/** \file citable.h
 *	This file defines overloads of free function `citable`
 */

///@{
/** \brief Make a citable version of length-delimited text.
 *
 *	Make a citable version of text - suitable for
 *	quoting in a diagnostic - by stripping out linefeeds and
 *	replacing tabs with spaces.
 *
 *	\param chew On entry, a `chewer<CharSeq>` positioned at the offset
 *  in its associated `CharSeq` from which to scan. On return `chew` is
 *  positioned at the first offset not consumed.
 *	\param len The length of the `CharSeq` to be scanned. `std::string::npos`
 *		denotes all the remainder.
 *	\return A citable version of the delimited text.
 */
std::string citable(chewer<std::string> & chew, size_t len = std::string::npos);
std::string citable(chewer<parse_buffer> & chew, size_t len = std::string::npos);
///@}


///@{
/*! \brief Make a citable version of part of a character-sequence
 *
 *	Make a citable version of a part of a character-sequence - suitable for
 *	quoting in a diagnostic - by stripping out linefeeds and
 *	replacing tabs with spaces.
 *	\tparam CharSeq A character-sequence type
 *	\param cxx Scanning c/c++ code?
 *	\param seq The character-sequence to scan.
 *	\param start The offset in `seq` from which to scan.
 *	\param len The length to be scanned.`std::string::npos` denotes the
 *		remainder of the buffer.
 *	\return A citable version of the delimited text.
 */
inline std::string citable(
    bool cxx,
    std::string const & seq,
    size_t start = 0,
    size_t len = std::string::npos)
{
	chewer<std::string> chew(cxx,const_cast<std::string &>(seq),start);
	return citable(chew,len);
}

inline std::string citable(
    bool cxx,
    parse_buffer const & seq,
    size_t start = 0,
    size_t len = std::string::npos)
{
	chewer<parse_buffer> chew(cxx,const_cast<parse_buffer &>(seq),start);
	return citable(chew,len);
}
///@}

/** \brief Make a citable version of an integral object.
 *	Make a citable version of an integral object - i.e. suitable for
 *	quoting in a diagnostic.
 *	\param  n   The integer to be converted to a citable string.
 *	\return A citable string representation of `n`.
 */
std::string citable(integer const & n);


#endif //EOF
