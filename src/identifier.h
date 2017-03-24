#ifndef IDENTIFIER_H
#define IDENTIFIER_H
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

#include "chew.h"
#include <cctype>
#include <string>

/** \file identifier.h
 *	This file defines `namespace identifier` and contained free functions.
 */

/// `namespace identifier` encloses free functions associated with identifiers
namespace identifier {

/// Say whether a character can be the first of an identifier.
inline bool is_start_char(char ch) {
    return isalpha(ch) || ch == '_';
}

/// Say whether a character can occur in an identifier.
inline bool is_valid_char(char ch) {
    return isalnum(ch) || ch == '_';
}

/** \brief Find the first occurrence of an identifier
 *	within a terminal segment a `CharSeq`
 *
 *  \tparam CharSeq A character-sequence type
 *
 *  \param id The identifier to be searched for.
 *  \param chew  On entry, a `chewer<CharSeq>` positioned at the offset
 *      in the associated `CharSeq` from which to scan. On return
 *  `chew` is positioned just passed the
 *  first occurrence found of `id`, or at the end of the `CharSeq`
 *  if none.
 *
 *  \return The offset in the `CharSeq` of the first occurrence found of
 *  `id`, if any, else `string::npos`
 *
 */
template<class CharSeq>
size_t find_first_in(std::string const & id, chewer<CharSeq> & chew);

/** \brief Search a terminal portion of a `CharSeq`
 * for any identifier.
 *
 *  Search a terminal portion of a `CharSeq` to find the first
 *	syntactic occurrence of an identifier (parsed greedily)
 *
 *  \tparam CharSeq A character-sequence type
 *
 *  \param chew  On entry, a `chewer<CharSeq>` positioned at the offset
 *      in the associated `CharSeq` from which to scan. On return
 *  `chew` is positioned just passed the
 *  first syntactic occurrence found of identifier`, or at the end of the
 *  `CharSeq` if none.
 *
 *  \param off On return, receives the offset in the `CharSeq`
 * of the first identifier detected, if any, else is unchanged.
 *
 *   \return The identifier detected, if any, else any empty string.
 */
template<class CharSeq>
std::string find_any_in(chewer<CharSeq> & chew, size_t & off);

/** \brief Read an identifier from an `chewer<CharSeq>`
 *
 *  \tparam CharSeq A charcter-sequence type
 *  \param chew  On entry, a `chewer<CharSeq>` positioned at the offset in the
 *      associated `CharSeq` from which to scan for an identifier.
 *      On return `chew` is positioned at the first offset not consumed.
 *
 *   \return The parsed identifier.
 */
template<class CharSeq>
std::string read(chewer<CharSeq> & chew);


} // namespace identifier

#endif // EOF
