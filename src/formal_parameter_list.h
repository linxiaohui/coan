#ifndef FORMAL_PARAMETER_LIST_H
#define FORMAL_PARAMETER_LIST_H
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

/** \file formal_parameter_list.h
 *   This file defines `struct formal_parameter_list`.
 */

/** \brief `struct formal_parameter_list` encapsulates a list of
 *   formal macro parameters.
 */
struct formal_parameter_list : innards::parameter_list_base {

    /// Base class
    using base = innards::parameter_list_base;

	/// Explicitly construct from a `parameter_list_base`
	explicit formal_parameter_list(parameter_list_base const & parms)
	: base(parms){}

	/** \brief Construct for `n` arguments
     *
     *  \param n The number of arguments.
     *
     *   If `n` > 0, the list of placeholder arguments `$1,...,$n` is
     *constructed.
	 */
	explicit formal_parameter_list(size_t n = 0)
	: base(n){}

	/** \brief Explicitly construct given a `chewer<CharSeq>`
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
	 *
	 *  \tparam A character-sequence type
	 */
	template<class CharSeq>
	explicit formal_parameter_list(chewer<CharSeq> & chew) {
		read(chew);
	}

	/** \brief Read the `formal_parameter_list` from a `chewer<CharSeq>`
     *
     *  \tparam CharSeq A character-sequence type
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     *
     *  The `formal_parameter_list` is emptied and replaced by parsing the text
	 *   from `chew`
	 */
    template<class CharSeq>
	void read(chewer<CharSeq> & chew);

private:

    using parameter_list_base::none;
    using parameter_list_base::empty_param;
    using parameter_list_base::unclosed;

};

#endif //EOF
