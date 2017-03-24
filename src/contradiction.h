#ifndef CONTRADICTION_H
#define CONTRADICTION_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
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
 *   Neither the name of Symbian Software Ltd. nor the names of its        *
 *   contributors may be used to endorse or promote products derived from  *
 *   this software without specific prior written permission.              *
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

#include "options.h"

/*! \file contradiction.h
 *   This file defines class `contradiction`.
 */

/** \brief Class `contradiction` handles contradictions
 *   between commandline options and directives.
 *
 *  The `contradiction` class manages contradictions detected
 *  between commandline `-D` or `-U` options and
 *  in-source `\#define` or `#undef` directives.
 *
 *  If, for example, a commandline `-DFOO` option is given
 *  and an `#undef FOO` directive is parsed in-source,
 *  a contradiction arises. It will be handled in accordance
 *  with the specified or default `--conflict` option.
 */
struct contradiction
{
    /// Reasons for reporting a contadiction.
    enum class cause {
        /// An in-souce \c \#define differently redefines a -D option.
        DIFFERENTLY_REDEFING_D,
        /// An in-souce \c \#undef contradicts a -D option.
        UNDEFING_D,
        /// An in-souce \c \#define contradicts a -U option.
        DEFINING_U
    };


	/** \brief Set the operative contradiction policy.
	 *   \param p   The contradiction policy to be applied.
	 */
	static void set_contradiction_policy(contradiction_policy p) {
		_policy_ = p;
	}

	/*! \brief Forget about an apparent contradiction.
     *
     *  The function causes the program to forget about a contradiction
	 *  that would be governed by the `--conflict` policy when later input
	 *  shows it to be innocuous. Its purpose is to cope with the complication
	 *  created by the common precautionary idioms:
     *
	 *  `#undef FOO`
	 *  `#define FOO`
     *
	 *  and:
     *
	 *  `#undef FOO`
	 *  `#define FOO	XYZ`
     *
	 *  These idioms may be encountered when `FOO` is the subject of a
	 *	`--define` option. In this case the precautionary `#undef FOO` will
	 *	at first appear to conflict with that `--define`, but the ensuing
	 *	`#define` will show that the conflict is only apparent.
     *
	 *  To cater for these cases, we defer diagnostic action on the
	 *  `#undef` until we see what follows. If a `#define` follows that
	 *  agrees with the `--define` options, with only whitespace and
	 *  comments intervening, then we forget about the pending contradiction.
	 *  This function clears the pending diagnostic action.
     *
	 *  \note	An `#undef` that is prima facie conflicting will always be
	 *  be dropped, whether or not it is anulled by a following `#define`.
	 *  If is not anulled by a following `#define`, then it will dropped
	 *  because any of the 3 possible `--conflict` policies will require
	 *  it either to be simply dropped, or else dropped and replaced with a
	 *  diagnostic insertion. If the `#undef` is anulled by a following
	 *  `#define`, then both will be dropped because the `#define` in
	 *  isolation would be dropped.
	 */
	static void forget();

	/**	\brief Discharge any pending contradiction.
     *
	 *  When an apparent contradiction transpires to be genuine in the light of
	 *	the following input we have a pending diagnostic action to discharge. It
	 *  will consist of writing a warning to cerr, unless warnings are
	 *  suppressed, and inserting a diagnostic comment or `#error` in the
	 *  output, unless the operative `--conflict` policy is `delete`.
	 *  This function discharges any such the pending action and otherwise
	 *  is a no-op.
     */
	static void flush();

	/** \brief Insert an error diagnostic into the output as a `#error`
	 *  directive or comment.
     *
	 *  \param why The reason for the potential contradiction.
	 *  \param  symname The name of symbol in the conflicting `#define` or
	 *       `#undef`.
     *
	 *  The functions inserts a diagnostic as a `#error` directive or comment
	 *  (depending on the operative `--conflict` policy)
	 *  to replace a `#undef` or `#define` directive that conflicts with a
	 *  `--define` or `--undef` option. The diagnostic is also written as a
	 *  warning to cerr.
	 */
	static void insert(cause why, std::string const & symname);

	/** \brief Save diagnostic details of a potential contradiction.
     *
	 *  \param why The reason for the potential contradiction.
	 *  \param  symname The name of symbol in the conflicting `#define` or
	 *       `#undef`.
     *
	 *  A potential contradiction can require confirmation after
	 *  parsing further input. When an `#undef` directive is read that
	 *  conflicts with a `--define` option, we will diagnose a conflict only
	 *  if the `#undef` is not followed by a `#define` that agrees with the
	 *  conflicting `--define`. This function stores the relevant diagnostics
	 *  for writing if the contradiction is confirmed. Otherwise the
	 *  contradiction is forgotten.
	 */
	static void save(cause why, std::string const & symname);

	/** \brief Get the name of the latest `#undef`-ed symbol.
     *
	 *   Return the symbol name that was undefined by the last
	 *   `#undef` directive that conflicts with
	 *   a `-define` option, if any.
     *
	 *   \return The relevant symbol name, or an empty string if none.
	 */
	static std::string const & last_conflicted_symbol_id() {
		return _last_conflicted_symbol_;
	}

private:

	/** \brief Insert a stored error diagnostic into output.
     *
	 *  Insert a stored error diagnostic into the output as a `#error`
	 *  directive or a comment (depending on the `--conflict` policy)
	 *  to replace a `#undef` or `#define` directive that conflicts with a
	 *  `--define`  option. Reiterate the diagnostic as a warning on cerr.
     *
	 *  If the `--conflict delete` is in force, there will be
	 *  no diagnostic to insert in output, but the diagnostic is still
	 *  written to `cerr`.
	 */
	static void insert_pending();

	/// The operative contradiction policy.
	static contradiction_policy		_policy_;

	/** \brief The reason-code of diagnostics associated with the operative
	 *  contradiction policy.
	 */
	static unsigned _policy_code_;

	/** \brief Text for substitution in output.
     *
	 *  The substitute text that will be inserted in output
	 *  is placed of a contradicted directive.
	 */
	static std::string _subst_text_;

	/** \brief The name of the latest `#undef`-ed symbol.
	 *
	 *  The symbol name that was undefined by the last
	 *  `#undef` directive that conflicts with
	 *  a `--define` option, if any.
	 */
	static std::string	_last_conflicted_symbol_;
	/// The number of lines converted to `#error` directives.
	static unsigned _errored_lines_;
	/// The number of lines converted to unconditional `#error` directives.
	static unsigned _unconditional_errored_lines_;
};

#endif /* EOF*/
