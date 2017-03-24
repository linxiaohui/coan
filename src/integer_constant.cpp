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

#include "integer_constant.h"
#include "parse_buffer.h"
#include "diagnostic.h"
#include "chew.h"
#include <limits>
#include <cassert>

/** \file integer_constant.cpp
 *   This file defines `struct integer_constant`.
 */

/// \cond NO_DOXYGEN

using namespace std;

template<class CharSeq>
unsigned long long
integer_constant::decode_utf8(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");

	unsigned long long val = numeric_limits<unsigned long long>::max();
	size_t bytes = 0;
	unsigned byte1 = *chew & 0xff;
	unsigned byte2, byte3, byte4;
	if (byte1 == unsigned(EOF)) {
		bytes = 0;
	} else if (byte1 >> 7 == 0) {
		/* Top bit of lead byte is 0. This is a plain ascii code */
		bytes = 1;
	} else if (byte1 >> 5 == 0x6) {
		/* Top 3 bits of lead byte are 110. A 2-byte code. */
		bytes = 2;
	} else if (byte1 >> 4 == 0xe) {
		/* Top 4 bits of lead byte are 1110. A 3-byte code. */
		bytes = 3;
	} else if (byte1 >> 3 == 0x1e) {
		/* Top 5 bits of lead byte are 11110. A 4-byte code. */
		bytes = 4;
	}
	switch(bytes) {
	case 1:
		val = byte1;
		chew(+1,continuation);
		break;
	case 2:
		byte1 &= 0x1F; /* Clear top three bits of lead byte. */
		chew(+1,continuation);
		byte2 = 0xff & *chew;
		if (byte2 == unsigned(EOF) || byte2 >> 6 != 2) {
			/* Top 2 bits of continuation byte must be 10 */
			break;
		}
		byte2 &= 0x3F;
		/* Value is concatenation of non-control bits */
		val = ((byte1 << 6) | byte2);
		chew(+1,continuation);
		break;
	case 3:
		byte1 &= 0x1F;
		chew(+1,continuation);
		byte2 = 0xff & *chew;
		if (byte2 == unsigned(EOF) || byte2 >> 6 != 2) {
			break;
		}
		byte2 &= 0x3F;
		chew(+1,continuation);
		byte3 = 0xff & *chew;
		if (byte3 == unsigned(EOF) || byte3 >> 6 != 2) {
			break;
		}
		byte3 &= 0x3F;
		val = (byte1 << 12) | (byte2 << 6) | byte3;
		chew(+1,continuation);
		break;
	case 4:
		byte1 &= 0x1F;
		chew(+1,continuation);
		byte2 = 0xff & *chew;
		if (byte2 == unsigned(EOF) || byte2 >> 6 != 2) {
			break;
		}
		byte2 &= 0x3F;
		chew(+1,continuation);
		byte3 = 0xff & *chew;
		if (byte3 == unsigned(EOF) || byte3 >> 6 != 2) {
			break;
		}
		byte3 &= 0x3F;
		chew(+1,continuation);
		byte4 = 0xff & *chew;
		if (byte4 == unsigned(EOF) || byte4 >> 6 != 2) {
			break;
		}
		byte4 &= 0x3F;
		val = (byte1 << 18) | (byte2 << 12) | (byte3 << 6) | byte4;
		chew(+1,continuation);
		break;
	default:
		break;
	}
	return val;
}

template<class CharSeq>
pair<unsigned long long,bool>
integer_constant::read_based_numeral(
	unsigned base, chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	int dval = 0;
	unsigned long long val = 0;
	unsigned long long tmp;
	bool overflow = false;
	for (	;; chew(+1,continuation)) {
		switch(*chew) {
		case '0':
			dval = 0;
			break;
		case '1':
			dval = 1;
			break;
		case '2':
			dval = 2;
			break;
		case '3':
			dval = 3;
			break;
		case '4':
			dval = 4;
			break;
		case '5':
			dval = 5;
			break;
		case '6':
			dval = 6;
			break;
		case '7':
			dval = 7;
			break;
		case '8':
			dval = 8;
			break;
		case '9':
			dval = 9;
			break;
		case 'a':
		case 'A':
			dval = 10;
			break;
		case 'b':
		case 'B':
			dval = 11;
			break;
		case 'c':
		case 'C':
			dval = 12;
			break;
		case 'd':
		case 'D':
			dval = 13;
			break;
		case 'e':
		case 'E':
			dval = 14;
			break;
		case 'f':
		case 'F':
			dval = 15;
			break;
		default:
			dval = 16;
		}
		if (unsigned(dval) < base) {
			tmp = val;
			val *= base;
			if (val / base != tmp) {
				overflow = true;
			} else if (dval) {
				tmp = val;
				val += dval;
				if (val <= tmp) {
					overflow = true;
				}
			}
		} else {
			break;
		}
	}
	return pair<unsigned long long, bool>(val,overflow);
}

template<class CharSeq>
int
integer_constant::read_char_escaping(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	int val = EOF;
	if (*chew == '\\') {
		chew(+1,continuation);
		switch(*chew) {
		case '\0':
			break;
		case 'a':
			val = '\a';
			break;
		case 'b':
			val = '\b';
			break;
		case 'f':
			val = '\f';
			break;
		case 'n':
			val = '\n';
			break;
		case 'r':
			val = '\r';
			break;
		case 't':
			val = '\t';
			break;
		case 'v':
			val = '\v';
			break;
		default:
			val = *chew;
		}
		chew(+1,continuation);
	} else if (chew) {
		val = *chew;
		chew(+1,continuation);
	}
	return val;
}

template<class CharSeq>
unsigned long long
integer_constant::read_encoded_char(int base, chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");

	unsigned long long const bad_val =
		numeric_limits<unsigned long long>::max();
	unsigned long long val = bad_val;
	assert(base == 8 || base == 16);
	size_t mark = size_t(chew);
	if (*chew == '\'') {
		chew(+1,continuation);
		if (*chew == '\\') {
			chew(+1,continuation);
			if (	(base == 8 && *chew == '0') ||
					(base == 16 && (*chew == 'x' || *chew == 'u' ||
					*chew == 'U'))) {
				chew(+1,continuation);
				std::pair<unsigned long long,bool> verdict =
					read_based_numeral(base,chew);
				val = verdict.first;
				bool overflow = verdict.second;
				if (overflow ||
					val > (unsigned long long)numeric_limits<int>::max()
							|| *chew != '\'') {
					val = bad_val;
				}
			}
		}
	}
	if (val == bad_val) {
		chew = mark;
	}
	return val;
}

template<class CharSeq>
integer
integer_constant::read_numeral(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	/*  Initially assume numeral may represent a long long
	    until a type suffix confirms or disconfirms.
	    Finally, in the absence of any type suffix, we will say the numeral
	    is an int (signed or unsigned) if it is not too big
	*/
	unsigned long long const bad_val =
		numeric_limits<unsigned long long>::max();
	integer result(INT_LLONG,bad_val);
	unsigned base = 10;
	unsigned long long max_val = bad_val;
	bool overflow = false;
	size_t num_len = 0;
	size_t mark = size_t(chew);
	char const *type_desc = nullptr;
	bool has_suffix = false;
	if (!chew) {
		return result;
	}
	if (*chew == '0') {
		chew(+1,continuation);
		if (*chew == 'x' || *chew == 'X') {
			chew(+1,continuation);
			base = 16;
		} else {
			base = 8;
		}
	}
	pair<unsigned long long, bool> verdict =
		read_based_numeral(base,chew);
	result._val = verdict.first;
	overflow = verdict.second;
	if (overflow) {
		result._type = INT_ULLONG;
		type_desc = "unsigned long long";
		max_val = bad_val;
	}
	num_len = size_t(chew) - mark;
	if (num_len == 1 && base == 16) {
		chew = mark;
	} else if (num_len > 0) {
		if (*chew == 'u' || *chew == 'U') {
			has_suffix = true;
			result._type = INT_UINT;
			chew(+1,continuation);
		}
		if (*chew == 'l' || *chew == 'L') {
			has_suffix = true;
			chew(+1,continuation);
			result._type = result.is_signed() ? INT_LONG : INT_ULONG;
		}
		if (*chew == 'l' || *chew == 'L') {
			has_suffix = true;
			chew(+1,continuation);
			result._type = result.is_signed() ? INT_LLONG : INT_ULLONG;
		}
		if (result.is_signed() && (*chew == 'u' || *chew == 'U')) {
			if (!has_suffix) {
				has_suffix = true;
				result._type = INT_UINT;
			} else {
				result.make_unsigned();
			}
			chew(+1,continuation);
		}
	}
	if (num_len == 0) {
		return result;
	}
	if (result.is_signed() &&
		result._val > (unsigned long long)numeric_limits<long long>::max() &&
			!has_suffix) {
		/* If we have no suffix explicitly specifying long or long long
		    then we allow the value to be an unsigned long long and emit
		    a warning. We store the warning at this point and will write it
		    later if it is not superceded by an overflow warning */
		warning_forced_unsigned() <<
			  "Integer constant \"" <<
			  chew.buf().substr(mark,size_t(chew) - mark)
				<< "\" is so large it is treated as unsigned" << defer();

		result._type = INT_ULLONG;
	}
	if (result.type() == INT_ULONG &&
			result._val > numeric_limits<unsigned long>::max()) {
		overflow = true;
		type_desc = "unsigned long";
		max_val = numeric_limits<unsigned long>::max();
	} else if (result.type() == INT_LONG &&
			result._val > (unsigned long long)numeric_limits<long>::max()) {
		overflow = true;
		type_desc = "long";
		max_val = numeric_limits<long>::max();
	} else if (result.type() == INT_LLONG &&
			result._val >
				(unsigned long long)numeric_limits<long long>::max()) {
		overflow = true;
		type_desc = "long long";
		max_val = numeric_limits<long long>::max();
	}
	if (overflow) {
		/* On overflow discard any stored warning */
		warning_forced_unsigned::discard();
		warning_int_overflow() << "Integer constant \"" <<
			   chew.buf().substr(mark,size_t(chew) - mark)
			   << "\" is too large for type " <<
			   type_desc << "(max " << max_val <<
			   "): expression will not be resolved" << emit();
		result._type = INT_UNDEF;
	} else {
		warning_forced_unsigned::flush(); /* Emit any stored warning */
		if (!has_suffix) {
			/* No type-suffix. Reduce type to (unsigned) int if value will fit
			*/
			if (result.is_signed()) {
				if (result._val <=
						(unsigned long long)numeric_limits<int>::max()) {
					result._type = INT_INT;
				}
			} else if (result._val <=
					(unsigned long long)numeric_limits<unsigned>::max()) {
				result._type = INT_UINT;
			}
		}
	}
	return result;
}

template
integer
integer_constant::read_numeral(chewer<parse_buffer> & chew);
template
integer
integer_constant::read_numeral(chewer<string> & chew);

template<class CharSeq>
integer integer_constant::read_char(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	integer result(INT_UNDEF);
	char const *type_desc = "int";
	unsigned long long const bad_val =
		numeric_limits<unsigned long long>::max();
	unsigned long long val = bad_val;
	unsigned long max_val = numeric_limits<int>::max();
	size_t mark = size_t(chew);
	/* Assume multi-byte constant.*/
	if (*chew == 'L') { /* No, this is a wide-character constant */
		max_val = numeric_limits<wchar_t>::max();
		type_desc = "wchar_t";
		chew(+1,continuation);
	} else if (*chew == 'u') {
		max_val = numeric_limits<char16_t>::max();
		type_desc = "char16_t";
		chew(+1,continuation);
	} else if (*chew == 'U') {
		max_val = numeric_limits<char32_t>::max();
		type_desc = "char32_t";
		chew(+1,continuation);
	}
	if (*chew != '\'') {
		chew = mark;
		return result;
	}
	val = read_encoded_char(16,chew); // hex?
	if (val == bad_val) {
		val = read_encoded_char(8,chew); // octal?
	}
	if (val == bad_val) {
		/* Not a hex or octal constant. Try UTF-8 or multibyte */
		chew(+1,continuation);  // Consume opening quote
		size_t restart = size_t(chew);
		// Try to read UTF-8.
		val = decode_utf8(chew);
		chew(continuation);
		if (*chew != '\'') { // Not UTF-8
			chew = restart;
			val = 0;
			/* Last resort. Read as concatentation of 8 bit-ints,
				possibly overflowing int.
			*/
			int nbytes = 0;
			for (	; chew && *chew != '\'' && *chew != '\n'; ++nbytes) {
					int chval =
						integer_constant::read_char_escaping(chew);
					if (chval == EOF) {
						break;
					}
					val = (val << 8 | chval);
			}
			if (*chew == '\'') {
				if (nbytes > 1) {
					warning_mulitbyte_char_constant() <<
						"Multi-byte character constant "
						<< chew.buf().substr(
							mark, size_t(chew) - mark + 1)
						<< emit();
				}
			} else {
				val = bad_val;
				warning_missing_terminator()
					<< "Missing \"'\" terminator after \""
					<< chew.buf().substr(mark) << '\"' << emit();
			}
		}
	}
	chew += (*chew == '\'');
	if (val != bad_val && val > max_val) {
		warning_char_constant_too_long() << "Character constant "
			<< chew.buf().substr(mark, size_t(chew) - mark)
			<< " overflows type " <<
			type_desc << "(max " << max_val
			<< "). Will not be resolved" << emit();
		val = bad_val;
	}
	if (val != bad_val) {
		result._val = val;
		result._type = INT_INT;

	} else {
		result._type = INT_UNDEF;
	}
	return result;
}

template
integer
integer_constant::read_char(chewer<parse_buffer> & chew);
template
integer
integer_constant::read_char(chewer<string> & chew);

/// \endcond NO_DOXYGEN

// EOF
