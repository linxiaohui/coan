#ifndef INTEGER_H
#define INTEGER_H
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

#include <cstddef>

/** \file integer.h
 *   This file defines `struct integer`.
 */

/// Enumerated constants denoting integral types.
enum integer_type {
    /// Undetermined type or invalid
    INT_UNDEF,
    /// Type `int`
    INT_INT = (3 << 16) | (sizeof(int) << 8) | 1,
    /// Type `unsigned`
    INT_UINT = (3 << 16) | (sizeof(int) << 8) | 2,
    /// Type `long`
    INT_LONG = (4 << 16) | (sizeof(long) << 8) | 1,
    /// Type `unsigned long`
    INT_ULONG = (4 << 16) | (sizeof(long) << 8) | 2,
    /// Type `long long int`
    INT_LLONG = (5 << 16) | (sizeof(long long) << 8) | 1,
    /// Type `unsigned long long`
    INT_ULLONG = (5 << 16) | (sizeof(long long) << 8) | 2,
};

/// Class `integer` encapsulates an integer of some type.
struct integer {

	friend struct integer_constant;

	/** Explicitly construct given a type and value.
	 *  \param  type The type of the `integer`
	 *  \param  val The value of the `integer`
	 */
	explicit integer(integer_type type = INT_INT, unsigned long long val = 0)
		: _val(val),_type(type) {}

	/// Get the type of the `integer`
	integer_type type() const {
		return _type;
	}
	/// Say whether the `integer` is of valid type, not `INT_UNDEF`
	bool good() const {
		return _type != INT_UNDEF;
	}
	/// Get the size of the `integer`
	size_t size() const {
		return (_type & 0xff00) >> 8;
	}
	/// Say whether the `integer` is of signed type.
	bool is_signed() const {
		return _type & 1;
	}
	/// Get the bits comprising the integer as an `unsigned long long`
	unsigned long long raw() const {
		return _val;
	}

	/// Get the text descriptor of this integer's type.
	char const * type_desc() const {
		return type_desc(_type);
	}

	/** Return the sign of an integer's value.
     *
	 *  \return -1 if the value is of signed integral type and is negative,
	 *  1 if the value is of signed integral type and is non-negative;
	 *  otherwise 0.
	 */
	int sign() const;

	/** Test whether the `integer` is less than another.
     *  \param  rhs The other `integer` to be compared.
	 *  \return 1 iff the comparison is true, else 0.
	 */
	integer operator<(integer const & rhs) const;

	/** Test whether the `integer` is less than or equal to another.
	 *  \param  rhs The other `integer` to be compared.
	 *  \return 1 iff the comparison is true, else 0.
	 */
	integer operator<=(integer const & rhs) const;

	/** Test whether the `integer` is greater than another.
	 *  \param  rhs The other `integer` to be compared.
	 *  \return 1 iff the comparison is true, else 0.
	 */
	integer operator>(integer const & rhs) const;

	/** Test whether the `integer` is greater than or equal to another.
	 *   \param  rhs The other `integer` to be compared.
	 *   \return 1 iff the comparison is true, else 0.
	 */
	integer operator>=(integer const & rhs) const;

	/** Test whether the `integer` is equal to another.
	 *  \param  rhs The other `integer` to be compared.
	 *  \return 1 iff the comparison is true, else 0.
	 */
	integer operator==(integer const & rhs) const;

	/** Test whether the `integer` is not equal to another.
	 *  \param  rhs The other `integer` to be compared.
	 *  \return 1 iff the comparison is true, else 0.
	 */
	integer operator!=(integer const & rhs) const;
	/*! Logically AND the `integer` with another.
	    \param  rhs The other `integer` to be conjoined.
	    \return 1 iff the conjunction is true, else 0.
	*/
	integer operator&&(integer const & rhs) const;
	/*! Logically OR the `integer` with another.
	    \param  rhs The other `integer` to be disjoined.
	    \return 1 iff the disjunction is true, else 0.
	*/
	integer operator||(integer const & rhs) const;

	/** Bitwise AND the `integer` with another.
	 *  \param  rhs The other `integer`.
	 *  \return The result of the operation.
	 */
	integer operator&(integer const & rhs) const;

	/** Bitwise OR the `integer` with another.
	 *  \param  rhs The other `integer`.
	 *  \return The result of the operation.
	 */
	integer operator|(integer const & rhs) const;

	/** Bitwise XOR the `integer` with another.
	 *  \param  rhs The other `integer`.
	 *  \return The result of the operation.
	 */

	integer operator^(integer const & rhs) const;

	/** Logically negate an `integer`.
	 *   \return 1 if the `integer` == 0, else 0.
	 */
	integer operator!() const {
		return good() ? integer(_type,!_val) : integer(INT_UNDEF);
	}

	/** Bitwise negate an `integer`.
	 *   \return The result of the operation.
	 */
	integer operator~() const;

	/** Invert the sign of an `integer`.
	 *   \return The result of the operation.
	 */
	integer operator-() const;

	/** Apply unary plus to an `integer`.
	 *  \return The result of the operation.
	 */
	integer operator+() const {
		return good() ? *this : integer(INT_UNDEF);
	}

	/** Add the `integer` to another
	 *  \param  rhs The other `integer` to be added.
	 *  \return The result of the addition.
	 */
	integer operator+(integer const & rhs) const;

	/** Subtract an `integer` from `*this`.
	 *  \param  rhs The `integer` to be subtracted.
	 *  \return The result of the subtraction.
	 */
	integer operator-(integer const & rhs) const;

	/** Multiply the `integer` by another.
	 *  \param  rhs The multiplier.
	 *  \return The result of the multiplication.
	 */
	integer operator*(integer const & rhs) const;

	/** Divide the `integer` by another.
	 *  \param  rhs The divisor.
	 *  \return The result of the division.
	 */
	integer operator/(integer const & rhs) const;

	/** Mod the `integer` by another.
	 *  \param  rhs modulus.
	 *  \return The result of the mod operation.
	 */
	integer operator%(integer const & rhs) const;

	/** Left-shift `integer` by another.
	 *  \param  rhs The shift value.
	 *  \return The result of the left-shift.
	 */
	integer operator<<(integer const & rhs) const;

	/** Right-shift the `integer` by another.
	 *  \param  rhs The shift-value.
	 *  \return The result of the right-shift.
	 */
	integer operator>>(integer const & rhs) const;

private:

	/** Get the unsigned variant to an integer type
	 *  \param  type The type whose unsigned variant is wanted.
	 *  \return The unsigned variant of `type`.
	 */
	static integer_type to_unsigned(integer_type type) {
		return integer_type((type & ~3) | 2);
	}

	/** Get the signed variant to an integer type
	 *  \param  type The type whose signed variant is wanted.
	 *  \return The signed variant of `type`.
	 */
	static integer_type to_signed(integer_type type) {
		return integer_type((type & ~3) | 1);
	}

	/** Get the conversion rank of an integer type
	 *  \param  type The type whose conversion rank is wanted.
	 *  \return The conversion rank of `type`.
	 */
	static unsigned rank_of(integer_type type) {
		return type >> 16;
	}

	/// Get the conversion rank of an `integer`
	unsigned rank() const {
		return rank_of(_type);
	}

	/// Make the `integer` unsigned
	void make_unsigned() {
		_type = to_unsigned(_type);
	}

	/// Make the `integer` signed
	void make_signed() {
		_type = to_signed(_type);
	}

	/** Get the `integer`_type that results when the the usual arithmetic
	 *   conversions are applied to on a pair of `integer` arithmetic operands.
     *
	 *  \param  lhs The lefthand operand
	 *  \param  rhs The righthand operand
     *
	 *  \return If either operand's type is `INT_UNDEF`
	 *  the returned type is `INT_UNDEF`. Otherwise it is the
	 *  maximum of `lhs.type()` and `rhs.type()`. The constants of
	 *  `enum integer_type` are so defined as to make this
	 *  the correct result.
     *
	 *  The function emits a warning if the usual arithmetic conversions
	 *  result in a change of sign on either operand.
	 */
	static integer_type
	result_type(integer const & lhs, integer const & rhs);


	/** Check the validity of a shift operation. A shift has undefined
	 *  behaviour if the promoted righthand operand is negative or is >= the
	 *  width in bits of the lefthand operand.
     *
	 *  \param direction If < 0, a left shift is indicated, else a right-shift.
	 *  \param  lhs The lefthand operand.
	 *  \param  rhs   The righthand operand.
	 *  \return True if the shift operation is valid. Otherwise the
	 *  function returns false and issues a diagnostic.
     */
	static bool
	valid_shift(int direction, integer const & lhs, integer const & rhs);

	/// Get textual descriptor of an integral type
	static char const * type_desc(integer_type it);

	/// The value of the `integer`
	unsigned long long _val;
	/// The type of the `integer`.
	integer_type _type;
};

#endif /* EOF*/
