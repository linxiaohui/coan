#ifndef ARGUMENT_LIST_H
#define ARGUMENT_LIST_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
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
#include "parameter_list_base.h"

/** \file argument_list.h
 * This file defines class `argument_list`
 */

/** \brief Class `argument_list` encapsulates a list of macro arguments, i.e.
 * the arguments to a macro reference.
 *
 *  Associated with each argument is a boolean flag indicating whether
 *  that argument is eligible for macro expansion. By default this flag
 *  is false.
 */
struct argument_list : innards::parameter_list_base {

    /// Base class
    using base = innards::parameter_list_base;

	/// Explicitly construct from an `innards::parameter_list_base`
	explicit argument_list(parameter_list_base const & parms)
	: base(parms){}

    /** \brief Construct for `n` arguments
     *
     *  \param n The number of arguments of the argument list
     *
     * If `n` > 0, the list of placeholder arguments `$1,...,$n` is
     * constructed.
     */
 	explicit argument_list(size_t n = 0)
	: base(n){}

    /** \brief Explicitly construct from a `chewer<CharSeq>`
     *
     * \tparam CharSeq A character-sequence type
     * \param chew  On entry, a `chewer<CharSeq>` positioned at the offset in the
     *      associated `CharSeq` from which to scan. On return is positioned
     *      at the first offset not consumed.
     */
	template<class CharSeq>
	explicit argument_list(chewer<CharSeq> & chew) {
		read(chew);
	}

    /// Equality
	bool operator==(argument_list const & other) const {
		return parameter_list_base::operator==(other) &&
            (_expand_flags == other._expand_flags ||
                *_expand_flags == *other._expand_flags);
	}

    /// Inequality
	bool operator!=(argument_list const & other) const {
		return !(*this == other);
	}

    /** \brief Read the `argument_list` from a `chewer<CharSeq>`
     *
     * \tparam CharSeq A character-sequence type
     * \param chew On entry, a `chewer<CharSeq>` positioned at the offset in the
     *      associated `CharSeq` from which to scan. On return is positioned
     *      at the first offset not consumed.
     */
    template<class CharSeq>
	void read(chewer<CharSeq> & chew);


    /** \brief Set the `n`th expand-flag to indicate whether the `n`th argument,
     *  if any, is eligible for macro expansion.
     *
     * \param n Index of the argument to be flagged
     * \param expandable A boolean denoting that that the `n`th argument
     *      is to be flagged as expandable or as not expandable.
     * \return True iff the `n`th member of the `argument_list` exists.
     */
	bool set_expandable(size_t n, bool expandable = false)  {
        if (_expand_flags && n < _expand_flags->size()) {
            (*_expand_flags)[n] = expandable;
            return true;
        }
        return false;
	}

    /** \brief Say whether the `n`th argument,
     *  if any, is eligible for macro expansion.
     *
     * \param n Index of the argument to be queried
     * \return True iff the `n`th member of the `argument_list` exists and
     *  is eligible for expansion.
     */
	bool is_expandable(size_t n) const {
        return _expand_flags && n < _expand_flags->size() ?
            (*_expand_flags)[n] : false;
	}

private:

    using parameter_list_base::none;
    using parameter_list_base::empty_param;
    using parameter_list_base::unclosed;

	/** \brief Say whether a character can validly appear in a member of
     * an `argument list`.
     *
     * \param  ch  The character to be tested.
     * \return True iff `ch` can validly appear in a member of an
     * `argument list`.
	 */
	static bool is_valid_char(char ch) {
		return ch && ch != ',' && ch != '(' && ch != ')';
	}

    /** The `n`th flag indicates whether the nth argument is eligible
        for macro expansion.
    */
    std::shared_ptr<std::vector<bool>> _expand_flags;

	/** \brief Append an argument to the argument list.
     *
     *  \param  arg  The putative argument to be appended.
     * \return True if `arg` has a non-0-length, else false,
	 *       in which case the `argument_list` is unmodified.
     */
	bool append(std::string const & arg);
};

#endif //EOF
