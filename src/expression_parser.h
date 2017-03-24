#ifndef EXPRESSION_PARSER_H
#define EXPRESSION_PARSER_H
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
#include "evaluation.h"
#include <string>
#include <vector>

/** \file expression_parser.h
 *   This defines class `expression_parser`
 */

// Forward decl
struct diagnostic_base;
struct reference;

/** \brief Encapsulates parsing of preprocessor expressions.
 *
 *   `template struct expression_parser` encapsulates the evaluation of
 *   all contexts that require resolution with respect
 *   to the defined and undefined symbols.
 *
*/
template<class CharSeq>
struct expression_parser
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");

    /// Type of character-sequence containing the parsed expression
    using sequence_type = CharSeq;
	/// Codes for logical deletion status.
	enum deletion_code {
		/// Not logically deleted
		not_deleted,
		/// A logically deleted non-parenthesis
		non_paren = 1,
		/// A logically deleted parenthesis
		paren
	};

	/// Codes for operator precedences
	enum precedence {
		max = 10,
		ternary_either_or = 10,
		ternary_if = 10,
		comma = 9,
		boolean_or = 8,
		boolean_and = 7,
		bit_or = 6,
		bit_xor = 6,
		bit_and = 6,
		eq = 5,
		neq = 5,
		le = 4,
		ge = 4,
		lt = 4,
		gt = 4,
		lshift = 3,
		rshift = 3,
		add = 2,
		subtract = 2,
		mult = 1,
		divide = 1,
		mod = 1,
	};

	/** \brief Construct given a `chewer<sequence_type>`
     *
	 *   \param chew On entry, a `chewer<sequence_type>` positioned to the
	 *      offset in the associated `sequence_type`
	 *		to scan. On return `chew` is positioned to the first offset
	 *      not consumed.
	 *	\param ref A pointer to the `reference` if any, that calls for
	 *		expression parsing.
	 */
    expression_parser(
        chewer<sequence_type> & chew,
        reference const * ref = nullptr)
    : 	_ternary_cond_stack(0),_seq(chew.buf()),
        _start(chew),_cuts(0),_last_deletion(-1),_ref(ref) {
        parse(chew);
    }

	/** \brief Construct to evaluate a `sequence_type`
	 *	\param seq A `sequence_type` to be evaluated.
	 *	\param ref A pointer to the `reference` if any, that calls for
	 *		expression parsing.
	 *	\param start The offset at which to start evaluation.
     *
	 *	The constructed object will have parsed and `seq` offset `start.
	 */
	expression_parser(
		sequence_type & seq,
		reference const * ref = nullptr,
		size_t start = 0);

	/** \brief Get the result of parsing the expression.
	 *  \return The `evaluation` that results from parsing.
	 */
	evaluation result() const {
		return _eval;
	}

	/// Cast the object to an `evaluation`.
	explicit operator evaluation () const {
		return result();
	}

	/** \brief Get the most simplified form of the expression.
     *
	 *	\return A string representing the expression simplified
	 *	as much as possible by resolution of subexpressions.
	 */
	std::string simplified() const;

	/// Say whether the expression has been simplified.
	bool is_simplified() const {
		return _cuts != 0;
	}

private:

    /// Type of an infix operation
    using infix_operation =
    evaluation 	(expression_parser::*)(evaluation const &, evaluation const &);

	/// Less-than operator.
	evaluation op_lt(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() < rhs.value());
	}

	/// Greater-than operator.
	evaluation op_gt(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() > rhs.value());
	}

	/// Less-than-or-equal operator.
	evaluation op_le(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() <= rhs.value());
	}

	/// Greater-than-or-equal operator.
	evaluation op_ge(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() >= rhs.value());
	}

	/// Equality operator.
	evaluation op_eq(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() == rhs.value());
	}

	/// Inequality operator.
	evaluation op_ne(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() != rhs.value());
	}

	/// Inclusive-or operator.
	evaluation op_or(evaluation const & lhs, evaluation const & rhs);

	//! And operator.
	evaluation op_and(evaluation const & lhs, evaluation const & rhs);

	/// The '?' "operator". Stacks its left operand; returns its right.
	evaluation
	op_ternary_if(evaluation const & lhs, evaluation const & rhs) {
		_ternary_cond_stack.push_back(lhs);
		return rhs;
	}

	/** \brief The ':' "operator".
	 *  Pops the evaluation of the preceding `cond ?' from the stack.
	 *  If `cond` is true, returns `lhs`; if `cond` is false returns
	 *  `rhs`, else returns `evaluation()`
	 */
	evaluation op_ternary_either_or(evaluation const & lhs,
											evaluation const & rhs);

	/// Bitwise AND operator.
	evaluation
	op_bit_and(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() & rhs.value());
	}

	/// Bitwise OR operator.
	evaluation
	op_bit_or(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() | rhs.value());
	}

	/// Bitwise XOR operator.
	evaluation
	op_bit_xor(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() ^ rhs.value());
	}

	/// Left-shift operator.
	evaluation
	op_lshift(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() << rhs.value());
	}

	/// Right-shift operator.
	evaluation
	op_rshift(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() >> rhs.value());
	}

	/// Addition operator.
	evaluation op_add(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() + rhs.value());
	}

	/// Subtraction operator.
	evaluation
	op_subtract(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() - rhs.value());
	}

	/// Comma operator.
	evaluation
	op_comma(evaluation const & lhs, evaluation const & rhs) {
		return rhs;
	}

	/// Multiplication operator.
	evaluation op_mult(evaluation const & lhs, evaluation const & rhs) {
		return evaluation(lhs.value() * rhs.value());
	}

	/// Division operator.
	evaluation op_divide(evaluation const & lhs, evaluation const & rhs);

	/// Modulus operator.
	evaluation op_mod(evaluation const & lhs, evaluation const & rhs);

	/** \brief Evaluate text from a `chewer<sequence_type>`
     *
	 *  \param chew On entry, a `chewer<sequence_type>` positioned to the
	 *      offset in the associated `sequence_type` from which
	 *		to scan. On return `chew` is positioned to the first
	 *		offset not consumed.
	 */
	void parse(chewer<sequence_type> & chew);

	/** \brief Evaluator for innermost subexpressions.
     *
	 *  \param chew On entry, a `chewer<sequence_type>` positioned to the
	 *      offset in the associated `sequence_type` from which
	 *		to scan. On return `chew` is positioned to the first
	 *		offset not consumed.
     *  \param end Offset beyond which not to evaluate.
	 *  \return	An `evaluation` representing the result of evaluation.
	 *
	 *  The member function evaluates symbols, numbers and unary operations,
     *  including the parenthesis operation.
     */
	evaluation
	unary_op(chewer<sequence_type> & chew, size_t end);

	/** \brief Evaluator for infix operations.
     *
	 *  The function evaluates subexpressions that are infix operations.
     *
     * The evaluator does shortcircuit evaluation for && and ||.
	 * It also simplifies binary subexpressions that can be solved
	 * only on one side.
     *
     * \tparam	Precedence	The precedence of the operators that
	 *		may be considered for evaluation.
	 *  \param chew On entry, a `chewer<sequence_type>` positioned to the
	 *      offset in the associated `sequence_type` from which
	 *		to scan. On return `chew` is positioned to the first
	 *		offset not consumed.
     *  \param end Offset beyond which not to evaluate
     *
	 * \return	An `evaluation` representing the result of evaluation.
	 */
	template<unsigned Precedence>
	evaluation
	infix_op(chewer<sequence_type> & chew, size_t end);

	///@{
	/** \brief Select the member function to recursively evaluate
	 *	expressions in which the major operator has a given precedence.
	 */
	template<unsigned Precedence>
	typename std::enable_if<(Precedence == 0),evaluation>::type
	next_evaluator(chewer<sequence_type> & chew, size_t end) {
		return unary_op(chew,end);
	}

	template<unsigned Precedence>
	typename std::enable_if<(Precedence > 0),evaluation>::type
	next_evaluator(chewer<sequence_type> & chew, size_t end) {
		return infix_op<Precedence>(chew,end);
	}
	///@}


	/** \brief Apply a infix operation to arguments
	 *	\param	op	Pointer to the operation to apply
	 *	\param	lhs	The left-hand argument
	 *	\param	rhs	The right-hand argument
	 *	\return The result of applying `op` to `lhs` and `rhs`
	 */
	evaluation apply(infix_operation op, evaluation & lhs, evaluation &rhs);

	/** \brief Search for the rightmost binary operator at a given level
	 *		of precedence within delimited text.
     *
	 *	\tparam	Precedence	The precedence of the operators to be sought.
     *
	 *	\param chew A `chewer<sequence_type>` positioned to the offset
     *  in the associated `sequence_type` from which
	 *		to search. On return `chew` is at the same position.
	 *	\param end Offset beyond which not to seach.
	 *	\param op_start Insignificant on entry. On return, receives
	 *		the offset of the rightmost operator found, if any.
	 *	\param op_end Insignificant on entry. On return, receives
	 *		the offset just past the rightmost operator found,if any.
     *
	 *	\return A pair whose first element is pointer to the `operation` whose
	 *  operator was found, if any, else nullptr, and whose second boolean
	 *  element is true if the operation found is shortcuitable, else false.
	 */
	template<unsigned Precedence>
	std::pair<expression_parser::infix_operation,bool>
	seek_rightmost_infix(	chewer<sequence_type> & chew,
							size_t end,
							size_t & op_start,
							size_t & op_end);

	/// Logically delete redundant parentheses.
	void delete_paren(size_t loff, size_t roff) {
		_deletions.resize(_seq.size(), deletion_code::not_deleted);
		_deletions[loff] = deletion_code::paren;
		_deletions[roff] = deletion_code::paren;
		if (ptrdiff_t(roff) > _last_deletion) {
			_last_deletion = roff;
		}
	}

	/// Restore a prematurely deleted pair of parentheses
	void restore_paren(size_t loff, size_t roff) {
		if (_deletions.size() > roff) {
			_deletions[loff] = deletion_code::not_deleted;
			_deletions[roff] = deletion_code::not_deleted;
		}
		for(	;_last_deletion >= 0 &&
				_deletions[_last_deletion] == deletion_code::not_deleted;
				--_last_deletion){}
	}

	/// Restore all logically deleted parentheses
	void restore_paren() {
		for (ptrdiff_t i = _start; i <= _last_deletion; ++i) {
			if (_deletions[i] == deletion_code::paren) {
				_deletions[i] = deletion_code::not_deleted;
			}
		}
	}

	/**	\brief Logically delete part of the evaluated text.
     *
	 *  \param	start	Text offset at which to cut.
	 *  \param	end		Text offset just past cut.
     *
	 *  The chunk to be deleted is only logically deleted by this function.
	 *  This avoids shuffling down terminal segments of the text in the course
	 *  of evaluation, when the effect of such editing may be obliterated by
	 *  subsequent deletions. Physical deletion is performed only once at the
	 *  end of evaluation, by removing all logically deleted portions.
     *
	 *  The member function is only called to delete a truth-functionally
	 *  redundant operator and one of its operands.
	 */
	void cut(size_t start, size_t end) {
		_deletions.resize(_seq.size(),deletion_code::not_deleted);
		for (	;start < end; ++start) {
			_deletions[start] = deletion_code::non_paren;
		}
		if (ptrdiff_t(end) > _last_deletion) {
			_last_deletion = end;
		}
		++_cuts;
	}

	/** \brief Defer a diagnostic, with the current line context appended
	 *	if a source line is being parsed.
	 */
	void defer_diagnostic(diagnostic_base & gripe);

	/// The evaluation that results from parsing the expression.
	evaluation _eval;
	/// Stack of ternary conditional results;
	std::vector<evaluation> _ternary_cond_stack;
	/// A reference to the character sequence being parsed.
	sequence_type & _seq;
	/// Vector of logical deletion codes;
	std::vector<deletion_code> _deletions;
	/// Text offset at which evaluation starts.
	size_t _start;
	/// Number of logically deleted operands
	size_t _cuts;
	/// Index of rightmost deletion, -1 if none
	ptrdiff_t _last_deletion;
	/** \brief A pointer to the `reference` if any, that calls for
     *   expression parsing.
	 */
	reference const * _ref;
};

#endif /* EOF*/
