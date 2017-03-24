#ifndef EVALUATION_H
#define EVALUATION_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Originally contributed by Mike Kinghan, imk@burroingroingjoing.com.   *
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

/** \file evaluation.h
 *  This file defines `struct evaluation`.
 */

/** \brief `struct evaluation` represents the result of evaluating a putative
 *   expression.
 */
struct evaluation {

    evaluation() = default;

	/** \brief Explicitly construct from an `integer`.
	 *   \param  val An `integer` to be evaluated.
	 */
	explicit evaluation(integer const & val)
		: 	_value(val){}

	/** \brief Explicitly construct from `int`.
	 *   \param  val An int to be evaluated.
	 */
	explicit evaluation(int val)
		: 	_value(integer(INT_INT,val)){}

	/// Say whether the expression has been resolved.
	bool resolved() const {
		return _value.type() != INT_UNDEF;
	}

	/// Say whether the expression is true.
	bool is_true() const {
		return resolved() && _value.raw() != 0;
	}

	/// Say whether the expression is false.
	bool is_false() const {
		return resolved() && _value.raw() == 0;
	}

	/// Say whether the expression is insoluble.
	bool insoluble() const {
		return _insoluble;
	}

	/// Say whether the expression is empty.
	bool empty() const {
		return _empty;
	}

	/** \brief Get/set the residual number of binary operators in the
	 *  expression, after simplification.
	 *  \return A reference to the count of binary operators in the expression.
	 */
	unsigned short & net_infix_ops() {
		return _net_infix_ops;
	}

	/// Get the integral value of the expression.
	integer const & value() const {
		return _value;
	}

	/// Set the integral value of the expression.
	void set_value(integer const & val) {
		_value = val;
		_net_infix_ops = 0;
	}

	/// Set the integral value of the expression.
	void set_value(int val) {
		_value = integer(INT_INT,val);
		_net_infix_ops = 0;
	}

	/// Classify the expression as insoluble.
	void set_insoluble() {
		_value = integer(INT_UNDEF);
		_insoluble = true;
	}

	/// Classify the expression as empty.
	void set_empty() {
		_empty = true;
	}

	/// Set the text offsets of surrounding parentheses.
	void set_parens_off(size_t loff, size_t roff) {
		_lparen_off = loff;
		_rparen_off = roff;
	}

	/// Clear the text offsets of surrounding parentheses.
	void clear_parens_off() {
		_lparen_off = size_t(-1);
		_rparen_off = size_t(-1);
	}

	/// Get the text offset of the left parenthesis, if any. -1 if none.
	size_t lparen_off() const {
		return _lparen_off;
	}

	/// Get the text offset of the right parenthesis, if any. -1 if none.
	size_t rparen_off() const {
		return _rparen_off;
	}

private:
	/// The integer value of the evaluated expression if it is soluble
	integer _value{INT_UNDEF};
	/** \brief Number of binary operators remaining in the evaluated expression
	 *	after simplification.
	 */
	unsigned short _net_infix_ops = 0;
	/// Is the expression empty?
	bool _empty = false;
	/// Is the expression insoluble;
	bool _insoluble = false;
	/// Has the expression been simplified?
	/// Text offset of left parenethesis, if any
	size_t _lparen_off = size_t(-1);
	/// Text offset of right parenethesis, if any
	size_t _rparen_off = size_t(-1);
};

#endif /* EOF*/
