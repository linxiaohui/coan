#ifndef PARAMETER_SUBSTUTION_H
#define PARAMETER_SUBSTUTION_H
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
#include <string>
#include <vector>

// Forward decl
struct symbol;
struct parameter_list;

/** \file parameter_substitution.h
 *
 *	This file defines:-
 *	- `namespace parameter_substitution`
 *	- `enum class parameter_substitution::handling`
 *	- `struct parameter_substitution::specifier`
 *	- `struct parameter_substitution::format`
 */

/// Namespace enclosing parameter substitution facilities
namespace parameter_substitution {

/// Non-printable fields in a substitution format
enum class demarcator : char
{
	soh = '\001',
	stx = '\002',
	etx = '\003',
	ack = '\006',
	nak = '\025',
	sub = '\032',
	esc = '\033'
};


/** \brief Enumerated constants denoting the manner in which an argument
 *	is to be substituted for a macro parameter
 */
enum class handling : char
{
	/// Substitute the argument literally
	substitute_arg = char(demarcator::etx),
	/// Substitute the full macro expansion of the argument
	substitute_expanded_arg = char(demarcator::sub),
	/// Substitute the argument literally enclosed in double quotes
	substitute_quoted_arg = char(demarcator::esc),
};

/** \brief `struct paramater_substitution::specifier` encapsulates a
	parameter substitution specifier.
*/
struct specifier
{
    /// Serialized size of a substitution specifier
	static const size_t serialized_size = 4;

	/// The default constructor yields the null specifier
	specifier()
	: _bytes{0}{}

	/** \brief Construct a specifier for the parameter indexed `param_i`
	 *	to be handled in manner `h`
	 */
	specifier(unsigned short param_i, handling h) {
		_bytes[0] = char(demarcator::stx);
		set_param_index(param_i);
		set_handling(h);
	}

    /// Equality
	bool operator==(specifier const & other) const {
		return _bytes[0] == other._bytes[0] &&
			_bytes[1] == other._bytes[1] &&
			_bytes[2] == other._bytes[2] &&
			_bytes[3] == other._bytes[3];
	}

    /// Inequality
	bool operator!=(specifier const & other) const {
		return !(*this == other);
	}

	/// Append the specifier to a string
	void append_to(std::string & s) {
		for (size_t i = 0; i < sizeof(_bytes); ++i) {
			s += _bytes[i];
		}
	}

	/// Set the index of the specified parameter
	void set_param_index(unsigned short i) {
		_bytes[1] = i >> 8;
		_bytes[2] = (char)i;
	}

	/// Get the index of the specified parameter
	unsigned short get_param_index() const {
		return (_bytes[1] << 8) | _bytes[2];
	}

	/// Set the handling of the specified parameter
	void set_handling(handling h) {
		_bytes[3] = (char)h;
	}

	/// Get the handling of the specified parameter
	handling get_handling() const {
		return handling(_bytes[3]);
	}

	/// Say whether the specifier is null
	bool null() const {
		return _bytes[0] == 0;
	}

	/// Explicit cast to `bool` returns !null()
	explicit operator bool() const {
		return !null();
	}

	/** \brief Attempt to read a specifier from a `chewer<std::string>`.
	 *
	 *  \param chew On entry, a `chewer<std::string>` positioned at the offset
	 *  in the associated string from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed if a specifier is
	 *  read, else is unchanged.
	 *	\return The specifier read, if any, else the null specifier
	 */
	static specifier read(chewer<std::string> & chew) {
		if (chew && *chew == char(demarcator::stx) && !chew.overshoot(3)) {
			char h = chew[3];
			if (h == char(handling::substitute_arg) ||
				h == char(handling::substitute_expanded_arg) ||
				h == char(handling::substitute_quoted_arg)) {
				unsigned short param_i = (chew[1] << 8) | chew[2];
				chew += serialized_size;
				return specifier(param_i,handling(h));
			}
		}
		return specifier();
	}

	/** \brief Attempt to detect a specifier from a string position.
     *
     *  \param str	The string to examine.
	 *	\param off The offset in `str` at which to start.
	 *	\param i_off If a specifier is detected at `off` in `str`,
	 *			receives on return the offset of the specifier's
	 *			parameter index field.
	 *	\param h_off If a specifier is detected at `off` in `str`,
	 *			receives on return the offset of the specifier's
	 *			handling field.
	 *	\return `off` if a specifier is detected, else `string::npos`.
	 */
	static size_t get_at(
		std::string const & str,
		size_t off,
		size_t & i_off,
		size_t & h_off)  {
		if (str[off] == char(demarcator::stx) && off + 3 < str.size()) {
			char h = str[off + 3];
			if (h == char(handling::substitute_arg) ||
					h == char(handling::substitute_expanded_arg) ||
						h == char(handling::substitute_quoted_arg) ) {
				i_off = off + 1;
				h_off = off + 3;
				return off;
			}
		}
		return std::string::npos;
	}

	/** \brief Attempt to detect a specifier ending at a string position.
	 *	\param str	The string to examine.
	 *	\param off The offset in `str` of the last byte of the presumed
	 *		specifier
	 *	\param i_off If a specifier is detected ending at `off` in `str`,
	 *			receives on return the offset of the specifier's
	 *			parameter index field.
	 *	\param h_off If a specifier is detected ending at `off` in `str`,
	 *			receives on return the offset of the specifier's
	 *			handling field.
	 *	\return `off` if a specifier is detected, else `string::npos`.
	 */
	static size_t get_ending_at(
		std::string const & str,
		size_t off,
		size_t & i_off,
		size_t & h_off)  {
		ptrdiff_t start = off - 3;
		if (start >= 0 && str[start] == char(demarcator::stx)) {
			char h = str[off];
			if (h == char(handling::substitute_arg) ||
					h == char(handling::substitute_expanded_arg) ||
						h == char(handling::substitute_quoted_arg) ) {
				i_off = off - 2;
				h_off = off;
				return start;
			}
		}
		return std::string::npos;
	}

	/// Get a string legibly representing the specifier
	std::string legible() const;

private:

    /// The specifier data.
	char _bytes[serialized_size];

};

/** \brief `struct parameter_substitution::format` encapsulates a
 *	parameter substitution format.
 *
 *	Such a format is a string with embedded specifiers that encodes
 *	the substitution of arguments for parameters into the format.
 */
struct format
{
	/// \brief Construct a parameter substitution format for a given symbol
	explicit format(symbol & sym);

	/// Get a reference to the format string.
	std::string const & str() const {
		return _fmt;
	}

	/// Get a string legibly representing the format
	std::string legible() const;

	/** \brief Get a string legibly representing a string with
	 *	embedded specifiers
	 */
	static std::string legible(std::string const & pb);

private:

	/** \brief Build a format with appropriate specifiers inserted
	 *	at all parameter positions
     *
	 *	\param sym The symbol for which
	 *		the format is built.
	 */
	void build_format(symbol & sym);

	/** \brief After building the first-cut format, make any necessary
     *  adjustments for the presence of '#'-operators.
     *
	 *	Occurrences of `#<whitespace><subsititute_expanded_arg_specifier>`
	 *	are replaced by the corresponding `<subsititute_quoted_arg_specifier>`
     *
	 *	\param nparams The number of parameters of the symbol requiring the
	 *	format.
     *
     *  \throw error_stringify_non_param if `nparams1` > 0 and an
     *  operand of `#` is not a parameter.
     *
	 */
	void do_stringify_adjustments(size_t nparams);

	/** \brief After building the first-cut format, make any necessary
	 *	adjustments for the presence of '##'-operators.
	 *
	 *	Consecutive occurrences of '##' are collapsed to one.
     *
	 *	If an operand is `<subsititute_expanded_arg_specifier>` that
	 *	operand is replaced with the corresponding `<substitute_arg_specifier>`
     *
	 *	Remaining occurrences of '##' and flanking whitespace are deleted.
	 *
     *  \throw error_misplaced_token_paste if an '##'-operator has no left
     *  operand or no right operand.
     *
     *  \throw error_bad_token_paste if pasting the operands of `##` will
     *  not form a token
     *
	 */
	void do_token_paste_adjustments();

	/**	\brief Adjust the format for a '#'-operator.
	 *
	 *	If `nparams` > 0 and the operand is a format specifier
	 *	then the operator and operand are replaced with the
	 *	corresponding `<substitute_quoted_arg_specifier>`
     *
	 *	\param pos The offset to the '#'-operator.
	 *	\param nparams The number of parameters of the symbol requiring the
	 *		format
	 *  \return The signed difference in the length of the format that
	 *  results from the adjustment.
     *  \throw error_stringify_non_param if `nparams1` > 0 and an
     *  operand of `#` is not a parameter.
     *
     *
	 */
	ptrdiff_t adjust_for_stringify_op(size_t pos, size_t nparams);

	/**	\brief Adjust the format for a '##'-operator.
	 *
	 *	If the either operand is a format specifier then it
	 *	is replaced with the corresponding `<substitute_arg_specifier>`
     *
	 *	The operator and flanking whitespace are deleted.
	 *	\param pos The offset to the '##'-operator.
     *  \return The signed difference in the length of the format that
	 *  results from the adjustment.
     *
     *  \throw error_misplaced_token_paste if the operator has no left
     *  operand or no right operand.
     *
     *  \throw error_bad_token_paste if pasting the operands will not form a
     *  token
     *
	 */
	ptrdiff_t adjust_for_token_paste_op(size_t pos);

	/// The format string.
	std::string _fmt;
	/// Vector of offsets of `#`-operators
	std::vector<size_t> _stringify_offs;
	/// Vector of offset of `##`-operators
	std::vector<size_t> _token_paste_offs;
};

} // namespace parameter_substitution

#endif // EOF
