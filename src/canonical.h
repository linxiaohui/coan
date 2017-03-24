#ifndef CANONICAL_H
#define CANONICAL_H
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
#include "traits.h"
#include <string>

struct symbol;

/** \file canonical.h
 *	This file defines:
 *	- `struct innards::canonical_base`
 *	- `tempplate struct canonical<What>`
 */

/// A tag class for parameterizing template class `canonical<What>`
struct macro_argument {};

/// Encloses internals
namespace innards {

/// A base for classes representing canonical forms of various types.
struct canonical_base {
	/// Implicitly cast the canonical representation to string.
	operator std::string () const {
		return _canonical;
	}

protected:

	/// Canonicalizing C/C++ source?
	bool cxx() const;

	/// String storing the canonical representation of a value.
	std::string _canonical;
};

} // namespace innards

/** \brief `template class canonical<What>` encapsulates the canonical
 *    representation of values of type `What`.
 *
 *   \tparam What The type that is canonically represented.
 */
template<class What>
struct canonical : innards::canonical_base {

    /** \brief Explicitly construct from a character-sequence
     * \tparam CharSeq A character-sequence type
     * \param seq A `CharSeq`
     */
	template<class CharSeq>
	explicit canonical(CharSeq & seq) {
		canonicalize(seq);
	}

    /** \brief Explicitly construct from a `chewer<CharSeq>`
     * \tparam CharSeq A character-sequence type
     * \param chew A `chewer<CharSeq>` positioned at the offset in the
     *  associated `CharSeq` at which canonicalization is to start. The
     *  canonical form of the remainder of the `CharSeq` is constructed.
     */
	template<class CharSeq>
	explicit canonical(chewer<CharSeq> & chew) {
        static_assert(traits::is_random_access_char_sequence<CharSeq>::value,
            ">:[");
		canonicalize(chew);
	}

private:

    /** \brief Canonicalize from a `chewer<CharSeq>`
     * \tparam CharSeq A character-sequence type
     * \param chew A `chewer<CharSeq>` positioned at the offset in the
     *  associated `CharSeq` at which canonicalization is to start. The
     *  canonical form of the remainder of the `CharSeq` is canonicalized.
     */
    template<class CharSeq>
	void canonicalize(chewer<CharSeq> & chew);

    /** \brief Canonicalize from a `CharSeq>`
     * \tparam CharSeq A character-sequence type
     * \param seq A `CharSeq` to be canonicalized.
     */
	template<class CharSeq>
	void canonicalize(CharSeq & seq) {
	    chewer<CharSeq> chew(cxx(),seq);
	    canonicalize(chew);
	}
};

/// \cond NO_DOXYGEN

template<>
template<class CharSeq>
void canonical<std::string>::canonicalize(chewer<CharSeq> & chew)
{
	chew(greyspace);
	if (!chew) {
		return;
	}
	for (	;; ) {
		_canonical += *chew;
		if (!++chew) {
			break;
		}
		chew(continuation);
		if (!chew) {
			break;
		}
		size_t mark = size_t(chew);
		chew(greyspace);
		if (mark != size_t(chew)) {
			_canonical += ' ';
		}
		if (!chew) {
			break;
		}
	}
	size_t len = _canonical.length();
	if (_canonical[len - 1] == ' ') {
		_canonical.resize(len - 1);
	}
}

template<>
template<class CharSeq>
void canonical<macro_argument>::canonicalize(chewer<CharSeq> & chew)
{
	int paren_balance = 0;
	for (chew(c_comment); chew;
			++chew,chew(c_comment)) {
		if (*chew == '(') {
			++paren_balance;
			_canonical += '(';
			continue;
        }
		if (*chew == ')') {
			if (--paren_balance < 0) {
				break;
			}
			_canonical += ')';
			continue;
		}
		if (*chew == ',') {
			if (paren_balance <= 0) {
				break;
			}
			_canonical += ',';
			continue;
		}
		if (!isspace(*chew)) {
            _canonical += *chew;
            continue;
		}
		if (_canonical.size()) {
            auto last = _canonical.back();
            if (!isspace(last) && (last == '#' || !ispunct(last))) {
                _canonical += ' ';
            }
		}
	}
}

/// \endcond NO_DOXYGEN

#endif /* EOF */
