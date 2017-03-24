#ifndef REFERENCE_H
#define REFERENCE_H
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

#include "prohibit.h"
#include "symbol.h"
#include "argument_list.h"
#include "reference_cache.h"
#include <string>


/** \file reference.h
 *   This file defines `struct reference`.
 */

/** Class `reference` encapsulates a reference of a symbol,
 *
 *  A reference is a use of the symbol, possibly with macro
 *  arguments.
 */
struct reference
{
    /// Type of result from cache insertion.
	using insert_result = reference_cache::insert_result;

	/** \brief Construct given a symbol locator
     *
	 *	\param loc Locator of the referenced symbol
	 *	\param invoker Pointer to the symbol reference, if any,
	 *		in whose resolution the symbol is referenced,
     *
	 *	This constructor represents a simple reference to the
	 *	a symbol, optionallly in the context
	 *	of resolving the reference's `invoker`
	 */
	explicit
	reference(symbol::locator loc, reference const * invoker = nullptr)
	: 	_referee(loc),
		_args(_referee->parameters()),
		_invoker(invoker),
		_key(_referee.id() + _args.str()){}


	/** \brief Construct given a symbol locator `argument_list`
	 *	and optional parent reference.
	 */
	reference(
		symbol::locator loc,
		argument_list const & args,
		reference const * invoker = nullptr)
	: 	_referee(loc),_args(args),
		_invoker(invoker),
		_key(_referee.id() + _args.str()){
			_referee->set_invoked();
		}


	/** \brief Construct given a symbol locator, a `chewer<CharSeq>`,
	 *	and optional parent reference.
     *
     *  \tparam CharSeq A character-sequence type
	 *	\param loc Locator of the referenced symbol.
     *  \param chew  On entry, a `chewer<CharSeq>` positioned at the offset in the
     *      associated `CharSeq` from which to scan for an argument list.
     *      On return `chew` is positioned at the first offset not consumed.
	 *	\param invoker Pointer to the symbol reference, if any,
	 *		in whose resolution the symbol is referenced,
	 */
	template<class CharSeq>
	reference(
		symbol::locator loc,
		chewer<CharSeq> & chew,
		reference const * invoker = nullptr)
	: 	_referee(loc),_args(chew),
		_invoker(invoker),_key(_referee.id() + _args.str()) {
        static_assert(traits::is_random_access_char_sequence<CharSeq>::value,
            ">:[");
		_referee->set_invoked();
	}

	virtual ~reference() = default;

	/** \brief Equality.
     *
     *  \param other A `reference` compared with `*this`.
     *  \return True iff `other == *this`.
	 *	References are equal iff they are references of the
	 *	the same symbol with the same arguments.
	 */
	bool operator==(reference const & other) const {
		return _referee.id() == other._referee.id() &&
			_args == other._args;
	}

    /// Inequality
	bool operator!=(reference const & other) const {
		return !(*this == other);
	}

	///@{
	/// \brief Get a [const] the state of the referenced symbol
	symbol::locator const & callee() const {
		return _referee;
	}
	symbol::locator callee() {
		return _referee;
	}
	///@}

	/// Get the name of the symbol referenced.
	std::string const & id() const {
		return _referee.id();
	}

    ///@{
	/// Get a [const] reference to the `argument_list` of the reference
	argument_list const & args() const {
		return _args;
	}
	argument_list & args() {
		return _args;
	}
	///@}

	//! Get a string representation of the reference.
	virtual std::string const & invocation() const {
		return _key;
	}

	/// Get the expansion of the reference
	std::string const & expansion() {
		return lookup().first->second.expansion();
	}

	/// Get the evaluation of the expansion
	evaluation const & eval() {
		return lookup().first->second.eval();
	}

	/// Say whether this reference has been reported.
	bool reported() {
		return lookup().first->second.reported();
	}

	/// Say whether this reference has been fully expanded.
	bool complete() {
		return lookup().first->second.complete();
	}

	/// Report this reference appropriately.
	void report() {
		if (reportable()) {
			do_report();
		}
	}

protected:

	/// Say whether we are to explain this reference
	bool explain() const {
		return _referee->configured() && explaining();
	}

	/** \brief Say whether this reference is to be explained or
	 *	is descended from one being explained.
	 */
	bool explaining() const;

	/// Say whether this reference is eligible for reporting
	bool reportable() const;

	/// Do reporting of the reference if reportable
	void do_report();


	/// Get a lower bound to this reference in the reference cache.
	reference_cache::iterator lower_bound() const {
		return reference_cache::lower_bound(_key);
	}

	/** \brief Lookup the reference in the cache.
     *
     *  If the reference is not found in the cache it is
     *  inserted now.
	 *	\return A `reference_cache::insert_result` whose
	 *	first element is an iterator to the cache record of
	 *	this reference and whose second boolean element
	 *	is true if the reference was inserted and false if
	 *	there was already a cached record of this reference.
	 */
	reference_cache::insert_result lookup();

	/// Return a cache entry for this reference
	reference_cache::value_type digest();

	/** \brief Expand the reference
     *
	 *	\param explain True if the `--explain` option is operative.
	 *  \return A `reference_cache::entry` recording the expansion.
     */
	reference_cache::entry expand(bool explain);

	/** \brief Diagnose a syntactically invalid reference.
     *
	 *	\return An `evaluation` that is set insoluble
	 *	if the reference is syntactically invalid.
	 */
	evaluation validate() const;

	/// The `symbol_state` of the referenced symbol
	symbol::locator _referee;
	/// The `argument_list` of the reference
	argument_list _args;
	/** \brief Pointer to the reference that invokes this one, if any,
	 *	else null.
	 */
	reference const * _invoker;
	/// Key to this reference in the reference cache
	std::string _key;
};

#endif //EOF
