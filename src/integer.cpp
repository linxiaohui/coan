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
#include "diagnostic.h"
#include "citable.h"
#include <cassert>

/** \file integer.cpp
 *   This file `struct integer`.
 */

using namespace std;

int integer::sign() const
{
	switch(_type) {
	case INT_INT:
		return (int)_val  < 0 ? -1 : 1;
	case INT_UINT:
		return 1;
	case INT_LONG:
		return (long)_val  < 0L ? -1 : 1;
	case INT_ULONG:
		return 1;
	case INT_LLONG:
		return (long long)_val < 0LL ? -1 : 1;
	case INT_ULLONG:
		return 1;
	default:
		return 0;
	}
}

integer_type
integer::result_type(integer const & lhs, integer const & rhs)
{
	if (lhs.type() == INT_UNDEF || rhs.type() == INT_UNDEF) {
		return INT_UNDEF;
	}
	integer_type result = lhs.type() > rhs.type() ? lhs.type() : rhs.type();
	if (result != lhs._type) {
		if (!lhs.is_signed() && rank_of(result) == lhs.rank() + 1) {
			result = to_unsigned(result);
		}
		integer test(result,lhs._val);
		if (test.sign() != lhs.sign() && lhs.sign() < 0) {
			string orig = citable(lhs);
			string converted = citable(test);
			warning_sign_changed() << "Integer " << orig <<
			   " changed to " << converted <<
			   " by the usual arithmetic conversions" << emit();
		}
	} else if (result != rhs._type) {
		if (!rhs.is_signed() && rank_of(result) == rhs.rank() + 1) {
			result = to_unsigned(result);
		}
		integer test(result,rhs._val);
		if (test.sign() != rhs.sign() && rhs.sign() < 0) {
			string orig = citable(rhs);
			string converted = citable(test);
			warning_sign_changed() << "Integer " << orig <<
			   " changed to " << converted <<
			   " by the usual arithmetic conversions" << emit();
		}
	}
	return result;
}


bool
integer::valid_shift(int direction, integer const & lhs, integer const & rhs)
{
	bool ok = true;
	size_t type_bits = lhs.size() * 8;
	unsigned long long val = rhs.raw();
	if (rhs.is_signed()) {
		unsigned long long neg_bit = (unsigned long long)1 << (type_bits - 1);
		if (val & neg_bit) {
			char const * way = direction < 0 ? "left" : "right";
			warning_negative_shift() << "Negative " << way <<
				 "-shift has undefined behavior. "
				 "The expression will not be resolved" << emit();
			ok = false;
		}
	}
	if (ok && val >= type_bits) {
		warning_shift_overflow() <<
			 "Shifting " << lhs.type_desc() << ' ' << lhs.raw()
			 << " by " << type_bits << " bits has undefined behavior. "
			 << emit();
	}
	return ok;
}

integer integer::operator<(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val < (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val < (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val < (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val < (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val < (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val < (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator<=(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val <= (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val <= (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val <= (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
		   (unsigned long)_val <= (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val <= (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
		   (unsigned long long)_val <= (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator>(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val > (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val > (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val > (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val > (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val > (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val > (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator>=(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val >= (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val >= (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val >= (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val >= (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val >= (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
		   (unsigned long long)_val >= (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator!=(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val != (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val != (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val != (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			(unsigned long)_val != (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val != (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
		   (unsigned long long)_val != (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator==(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val == (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val == (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val == (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val == (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val == (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val == (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator&&(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val && (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val && (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val && (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val && (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val && (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val && (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator||(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val || (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val || (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val || (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val || (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val || (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val || (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator&(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val & (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val & (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val & (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val & (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val & (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val & (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator|(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val | (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val | (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val | (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val | (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val | (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val | (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator^(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(INT_INT,(int)_val ^ (int)rhs.raw());
	case INT_UINT:
		return integer(INT_INT,(unsigned)_val ^ (unsigned)rhs.raw());
	case INT_LONG:
		return integer(INT_INT,(long)_val ^ (long)rhs.raw());
	case INT_ULONG:
		return integer(INT_INT,
			   (unsigned long)_val ^ (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(INT_INT,(long long)_val ^ (long long)rhs.raw());
	case INT_ULLONG:
		return integer(INT_INT,
			   (unsigned long long)_val ^ (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator-() const
{
	switch(_type) {
	case INT_INT:
		return integer(_type,-(int)_val);
	case INT_UINT:
		return integer(_type,-(unsigned)_val);
	case INT_LONG:
		return integer(_type,-(long)_val);
	case INT_ULONG:
		return integer(_type,-(unsigned long)_val);
	case INT_LLONG:
		return integer(_type,-(long long)_val);
	case INT_ULLONG:
		return integer(_type,-(unsigned long long)_val);
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator~() const
{
	switch(_type) {
	case INT_INT:
		return integer(_type,~(int)_val);
	case INT_UINT:
		return integer(_type,~(unsigned)_val);
	case INT_LONG:
		return integer(_type,~(long)_val);
	case INT_ULONG:
		return integer(_type,~(unsigned long)_val);
	case INT_LLONG:
		return integer(_type,~(long long)_val);
	case INT_ULLONG:
		return integer(_type,~(unsigned long long)_val);
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator+(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(type,(int)_val + (int)rhs.raw());
	case INT_UINT:
		return integer(type,(unsigned)_val + (unsigned)rhs.raw());
	case INT_LONG:
		return integer(type,(long)_val + (long)rhs.raw());
	case INT_ULONG:
		return integer(type,
			   (unsigned long)_val + (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(type,(long long)_val + (long long)rhs.raw());
	case INT_ULLONG:
		return integer(type,
			   (unsigned long long)_val + (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator-(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(type,(int)_val - (int)rhs.raw());
	case INT_UINT:
		return integer(type,(unsigned)_val - (unsigned)rhs.raw());
	case INT_LONG:
		return integer(type,(long)_val - (long)rhs.raw());
	case INT_ULONG:
		return integer(type,
			   (unsigned long)_val - (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(type,(long long)_val - (long long)rhs.raw());
	case INT_ULLONG:
		return integer(type,
			   (unsigned long long)_val - (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator*(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(type,(int)_val * (int)rhs.raw());
	case INT_UINT:
		return integer(type,(unsigned)_val * (unsigned)rhs.raw());
	case INT_LONG:
		return integer(type,(long)_val * (long)rhs.raw());
	case INT_ULONG:
		return integer(type,
			   (unsigned long)_val * (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(type,(long long)_val * (long long)rhs.raw());
	case INT_ULLONG:
		return integer(type,
			   (unsigned long long)_val * (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator/(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(type,(int)_val / (int)rhs.raw());
	case INT_UINT:
		return integer(type,(unsigned)_val / (unsigned)rhs.raw());
	case INT_LONG:
		return integer(type,(long)_val / (long)rhs.raw());
	case INT_ULONG:
		return integer(type,
			   (unsigned long)_val / (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(type,(long long)_val / (long long)rhs.raw());
	case INT_ULLONG:
		return integer(type,
			   (unsigned long long)_val / (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator%(integer const & rhs) const
{
	integer_type type = integer::result_type(*this,rhs);
	switch(type) {
	case INT_INT:
		return integer(type,(int)_val % (int)rhs.raw());
	case INT_UINT:
		return integer(type,(unsigned)_val % (unsigned)rhs.raw());
	case INT_LONG:
		return integer(type,(long)_val % (long)rhs.raw());
	case INT_ULONG:
		return integer(type,
			   (unsigned long)_val % (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(type,(long long)_val % (long long)rhs.raw());
	case INT_ULLONG:
		return integer(type,
			   (unsigned long long)_val % (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator<<(integer const & rhs) const
{
	integer_type type_check = integer::result_type(*this,rhs);
	if (type_check == INT_UNDEF || !integer::valid_shift(-1,*this,rhs)) {
		return integer(INT_UNDEF);
	}
	switch(_type) {
	case INT_INT:
		return integer(_type,(int)_val << (int)rhs.raw());
	case INT_UINT:
		return integer(_type,(unsigned)_val << (unsigned)rhs.raw());
	case INT_LONG:
		return integer(_type,(long)_val << (long)rhs.raw());
	case INT_ULONG:
		return integer(_type,
			   (unsigned long)_val << (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(_type,(long long)_val << (long long)rhs.raw());
	case INT_ULLONG:
		return integer(_type,
			   (unsigned long long)_val << (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

integer integer::operator>>(integer const & rhs) const
{
	integer_type type_check = integer::result_type(*this,rhs);
	if (type_check == INT_UNDEF || !valid_shift(1,*this,rhs)) {
		return integer(INT_UNDEF);
	}
	switch(_type) {
	case INT_INT:
		return integer(_type,(int)_val >> (int)rhs.raw());
	case INT_UINT:
		return integer(_type,(unsigned)_val >> (unsigned)rhs.raw());
	case INT_LONG:
		return integer(_type,(long)_val >> (long)rhs.raw());
	case INT_ULONG:
		return integer(_type,
			   (unsigned long)_val >> (unsigned long)rhs.raw());
	case INT_LLONG:
		return integer(_type,(long long)_val >> (long long)rhs.raw());
	case INT_ULLONG:
		return integer(_type,
			   (unsigned long long)_val >> (unsigned long long)rhs.raw());
	default:
		return integer(INT_UNDEF);
	}
}

char const * integer::type_desc(integer_type it)
{
	switch(it) {
    case INT_INT:
		return "int";
    case INT_UINT:
		return "unsigned int";
    case INT_LONG:
		return "long int";
    case INT_ULONG:
		return "unsigned long int";
    case INT_LLONG:
		return "long long int";
	case INT_ULLONG:
		return "unsigned long long int";
	default:
		assert(false);
	}
	return "";
}

// EOF
