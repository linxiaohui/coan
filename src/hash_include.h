#ifndef HASH_INCLUDE_H
#define HASH_INCLUDE_H
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
#include "reference.h"
#include "directive.h"

/** \file hash_include.h
 *	This file defines `struct hash_include`
 */

struct reference;

/// \brief `struct hash_include` encapsulates an `#include` directive.
struct hash_include : private no_copy {

    hash_include() = delete;

	/** \brief Explicitly construct from the argument of an
	 *   `#include`  directive.
	 *   \param  arg The body of the `#include` directive
	 */
	explicit hash_include(std::string const & arg)
	: _directive(arg) {}

	/** \brief Explicitly construct from the argument of an
	 *   `#include`  directive.
	 *  \param chew On entry, a `chewer<parse_buffer>` positioned at the offset
	 *  in the associated `parse_buffer` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     */
	explicit hash_include(chewer<parse_buffer> & chew)
	: _directive(read(chew)) {}


	/** \brief Say whether the body of an `#include` directive is
	 *  a macro-reference.
     *
	 *  \return If the body of the directive is a putative macro reference
	 *	a pointer to that reference is returned, else a null pointer.
	*/
	std::shared_ptr<reference> symbolic_argument() const {
		return _ref;
	}

	/** \brief Return the header file expression.
     *
	 *  \return If the body of the `#include` is a macro-reference of
	 *  a configured symbol then result of resolving that reference is
	 *  returned. Otherwise the returned string is the same as returned by
	 *  `argument()`.
	 */
	std::string filename() const {
		return  _ref ? _ref->expansion() : _directive.argument();
	}


	/** \brief Test whether the `#include` specifies a system header,
	 *  i.e. `<headername>`
     *
	 *  \return True iff the `#include` specifies a system header.
	 */
	bool system_header() const {
		return filename()[0] == '<';
	}

	/** \brief Test whether the `#include` specifies a local header,
	 *   i.e. "headername".
     *
	 *   \return True iff the `#include` specifies a local header.
     *
	 *  `system_header()` and `local_header()` are not mutually
	 *  exclusive, as a symbolic `#include` argument may fail
	 *  to resolve as either.
	 */
	bool local_header() const {
		return filename()[0] == '\"';
	}

	/// Say whether the `#include` directive is valid.
	bool valid() const;


	/// Report the `#include` directive.
	void report();

private:

	/** \brief Read the body of an `#include` directive.
	 *  \param chew On entry, a `chewer<parse_buffer>` positioned at the offset
	 *  in the associated `parse_buffer` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     *
	 *   \return The parsed argument.
	 */
	std::string read(chewer<parse_buffer> & chew);

	/** \brief A pointer to the `#include` argument as a `reference`, if
	 *	it one, else a null pointer.
	 */
	std::shared_ptr<reference> _ref;
	/// The member handles reporting the `#include` directive.
	directive<HASH_INCLUDE> _directive;

};

#endif /* EOF*/
