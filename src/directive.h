#ifndef DIRECTIVE_H
#define DIRECTIVE_H
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

#include "line_despatch.h"
#include "chew.h"
#include <map>
#include <vector>

/** \file directive.h
 *   This defines:-
 *	- `struct directive_base`
 *	- `template struct directive<Type>`
*/

/** \brief `template struct directive_base` is a base for specializations of
 *   `template struct directive<Type>`
 */
struct directive_base : private no_copy
{
	/** \brief Evaluate a directive
	 *  \param  keyword The keyword indicating the directive type.
	 *  \param  chew  On entry, a `chewer<parse_buffer>` positioned to the
	 *      offset in the associated `parse_buffer` from which to to scan.
	 *			On return receives `chew` is positioned to the first
	 *			offset not consumed.
	 *   \return A `line_type` determined by evaluation
	 *       denoting the action to be taken on this line.
	 */
	static line_type eval(std::string keyword, chewer<parse_buffer> & chew) {
		return eval(get_type(keyword),chew);
	}

	/// Forget all directives table used by the operative command.
	static void erase_all();

protected:

	/** \brief Report a directive
	 *  \param  keyword The keyword indicating the directive type.
	 *  \param  arg The body of the directive.
	 */
	static void report(std::string const & keyword, std::string const & arg);

	/** \brief Report a directive.
	 *  \param  seen Has this directive already been seen?
	 *  \param  keyword The keyword indicating the directive type.
	 *  \param  arg The body of the directive.
     *
	 *  The member function reports the directive if it has not already been
	 *  seen or if option `--once-only` is not in effect.
	 */
	static void report(bool seen,
	                   std::string const & keyword, std::string const & arg);

	/** \brief Evaluate a directive.
	 *  \param  type The `directive_type` of the directive.
	 *  \param  chew  On entry, a `chewer<parse_buffer>` positioned to the
	 *      offset in the associated `parse_buffer` from which
	 *			to scan. On return `chew` is positioned to the first
	 *			offset not consumed.
	 *   \return A `line_type` determined by evaluation.
	 */
	static line_type eval(directive_type type, chewer<parse_buffer> & chew);

	/** \brief Evaluate an `#ifdef` or `#ifndef` directive
	 *   \param  type    The `directive_type` of the directive.
	 *  \param  chew  On entry, a `chewer<parse_buffer>` positioned to the
	 *      offset in the associated `parse_buffer` from which
	 *			to scan. On return `chew` is positioned to the first
	 *			offset not consumed.
	 *   \return A `line_type` determined by evaluation.
	 */
	static line_type
	eval_ifdef_or_ifndef(directive_type type, chewer<parse_buffer> & chew);

private:

	/// Type of evaluation functions applied to directive bodies
	using evaluator = line_type (*)(chewer<parse_buffer> &);
	/// Get the `directive_type` of a directive given a keyword
	static directive_type get_type(std::string const & keyword);
	/// Map from keywords to directive types.
	static std::map<std::string, directive_type> _keyword_to_type_map_;
	/** \brief Table of evaluation functions for directive types, indexed
	 *   by `directive_type`.
	 */
	static std::vector<evaluator> _evaluator_tab_;
};

/** \brief Encapsulates a directive of a given type
 *
 *   `template struct directive<Type>` encapsulates a directive of
 *   a given type and provides a global lookup container for tracking
 *   occurrences of directives of that type.
 *
 *  The class template supports the coan commands that report occurrences of
 *  directives. The	container is keyed by the canonicalised text of directive
 *	occurrence. Storing the directive occurrences in canonicalised form enables
 *	occurrences to be compared regardless of insignificant differences of
 *	whitespace or comments.
 *
* \tparam Type  The `directive_type` of directives contained by this class.
*/
template<directive_type Type>
struct directive : directive_base
{

	friend struct directive_base;
	/// The `directive_type` of this class.
	static directive_type const _type_ = Type;

	//! Destructor
	virtual ~directive() = default;

	/** \brief Explicitly construct a directive from a directive body.
	 *   \param arg  The directive body.
	 */
	explicit directive(std::string const & arg)
		:  _loc(insert(arg)) {}

	/// Get the body of the directive.
	std::string const & argument() const {
		return _loc->first;
	}

	/// Report the directive.
	virtual void report() {
		directive_base::report(_loc->second,_keyword_,_loc->first);
		_loc->second = true;

	}

	/// Say whether the directive has been reported.
	bool reported() const {
		return _loc->second;
	}

	/// Set the status of the directive as reported or unreported.
	void set_reported(bool reported = true) {
		_loc->second = reported;
	}

protected:

	/** \brief Evaluate a directive of this type.
	 *  \param  chew  On entry, a `chewer<parse_buffer>` positioned to the
	 *      offset in the associated `parse_buffer` from which
	 *			to scan. On return `chew` is positioned to the first
	 *			offset not consumed.
	 *   \return A `line_type` determined by evaluation
	 *           denoting the action to be taken on this line.
	 */
	static line_type eval(chewer<parse_buffer> & chew);

	/** \brief Type of global lookup table for directives of this type.
     *
	 *   The container maps the canonicalized text of the directive to
	 *   a `bool` indicating whether the directive has been reported.
	 */
	using directives_table = std::map<std::string,bool> ;
	/// Type of entry in `directives_table`.
	using table_entry = directives_table::value_type;

	/**\brief Insert a directive into the global lookup table.
     *
	 *   \param  arg The body of the directive.
	 *   \return An interator to the inserted entry.
	 */
	static directives_table::iterator insert(std::string const & arg) {
		return _directives_tab_.insert(table_entry(arg,false)).first;
	}

	/// An iterator locating this directive in the global lookup table.
	directives_table::iterator _loc;

	/// The keyword for directives of the type.
	static std::string const _keyword_;
	/// The global lookup table for directives of this type.
	static directives_table _directives_tab_;
};

/// \cond NO_DOXYGEN

template<>
line_type directive<HASH_UNKNOWN>::eval(chewer<parse_buffer> &);

template<>
line_type directive<HASH_IF>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_IFDEF>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_IFNDEF>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_ELSE>::eval(chewer<parse_buffer> &);

template<>
line_type directive<HASH_ELIF>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_ENDIF>::eval(chewer<parse_buffer> &);

template<>
line_type directive<HASH_DEFINE>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_UNDEF>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_INCLUDE>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_PRAGMA>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_ERROR>::eval(chewer<parse_buffer> & chew);

template<>
line_type directive<HASH_LINE>::eval(chewer<parse_buffer> & chew);


/// \endcond

#endif /* EOF*/
