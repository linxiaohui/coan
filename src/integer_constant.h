#ifndef INTEGER_CONSTANT_H
#define INTEGER_CONSTANT_H
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

#include "integer.h"
#include "chew.h"
#include <utility>

/** \file integer_constant.h
 *   This file defines `struct integer_constant`.
 */

/// `struct integer_constant` encapsulates an integer constant
struct integer_constant {

	/** \brief Read a numeral from a `chewer<CharSeq>`
	 *
	 *  \tparam CharSeq A character-sequence type
	 *
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
	 *
	 *  \return An `integer` that represents the integer value of the numeral,
	 *  unless the value is too big for the explicit or implied type of the
	 *  numeral in which case `integer(INT_UNDEF)` is returned.
     *
	 *  The parsed numeral is attributed type signed int unless an
	 *  a type suffix indicates otherwise.
     *
	 *  The numeral may include any of the type-indicating suffixes
	 *  'L', 'LL' 'UL', 'LU', 'ULL', 'LLU' or their case-variants.
     *
	 *   The string "0x" or "0X" is parsed as a valid numeral "0" followed
	 *   by 'x' or 'X'.
	 */
	template<class CharSeq>
	static integer read_numeral(chewer<CharSeq> & chew);

	/** \brief Read a character constant from a text offset, returning
	 *   its value as an `integer`.
     *
	 *  \tparam CharSeq A character-sequence type
	 *
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     *
	 *  \return An `integer` that represents the integer value of
	 *  the parsed character constant, or `integer(UNDEF)`
	 *  is no valid constant is parsed.
     */
	template<class CharSeq>
	static integer read_char(chewer<CharSeq> & chew);

private:

	/** \brief Decode an UTF-8 encoded character from a text offset.
     *
	 *  \tparam CharSeq A character-sequence type
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
	 *	\return The integer value of the UTF-8 character parsed, or
	 *  `ULLONG_MAX` if none is parsed.
	 */
	template<class CharSeq>
	static unsigned long long
	decode_utf8(chewer<CharSeq> & chew);

	/** \brief Read a possibly escaped ASCII character from a text offset,
	 *   returning its escaped value.
     *
	 *  \tparam CharSeq A character-sequence type
	 *
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     *
     *  \return The integer value of the possibly escaped character consumed,
	 *  if any, else `EOF`.
     *
	 *  If `chew` is not out of range and addresses the escape character
	 *	'\\' on entry then that character is consumed and, if `chew + 1` is not
	 *	out of range, the escaped value of the next character is returned.
	 *	If `chew + 1` is out of range then `EOF` is returned.
     *
	 *  If `chew` does not address the escape character on entry and is
	 *	is not out range, the integer value of the character addressed is
	 *	returned; otherwise EOF.
	 */
	template<class CharSeq>
	static int
	read_char_escaping(chewer<CharSeq> & chew);


	/** \brief Read a numeral of known base from a text pointer, returning a
	 *   value and overflow indicator.
	 *
	 *  \tparam CharSeq A character-sequence type.
	 *  \param  base The arithmetic base in which the numeral is to be evaluated.
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
	 *  \return An `std::pair` containing, first, the integer value of the
	 *  parsed numeral; second, a bool that is true if the value of
	 *  the parsed numeral overflows unsigned long long, in which case
	 *  the returned integer value is not valid.
     *
	 *  The function consumes characters from `chew` for as long as
	 *  they can form part of a numeral with the indicated base.
	 */
	template<class CharSeq>
	static std::pair<unsigned long long,bool>
	read_based_numeral(unsigned base, chewer<CharSeq> & chew);

	/** \brief Read a numerically encoded character constant from a
	 *	text offset, returning its value as an `integer`.
	 *
     *  \tparam CharSeq A character-sequence type.
	 *  \param base The presumed base of the constant, either 8 (octal) or
	 *  16 (hex).
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
	 *  \return The integer value of encoded constant, if any is found,
	 *  else `ULONG_MAX`.
     *
	 *  A constant at `chew` will be enclosed in single quotes.
	 *  On entry `chew` shall address the opening single quote.
	 */
	template<class CharSeq>
	static unsigned long long
	read_encoded_char(int base, chewer<CharSeq> & chew);
};

#endif // EOF
