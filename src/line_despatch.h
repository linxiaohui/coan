#ifndef LINE_DESPATCH_H
#define LINE_DESPATCH_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2002, 2003 Tony Finch <dot@dotat.at>.                   *
 *   All rights reserved.                                                  *
 *   Copyright (C) 1985, 1993 The Regents of the University of California. *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed by Mike Kinghan, imk@burroingroingjoing.com,              *
 *   derived from the code of Tony Finch                                   *
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

#include "line_type.h"
#include "parsed_line.h"
#include <string>
#include <memory>

/** \file line_despatch.h
 *   This defines `struct line_despatch`
 */

/** \brief Manages printing, discarding and replacement of lines.
 *
 *   `struct line_despatch` manages the printing of lines
 *   selected or generated for output and the dropping or
 *   replacing of lines not selected for output.
 */
struct line_despatch {

	/// Reinitialize the class static state.
	static void top();

	/// Get a reference to the current output line
	static parsed_line & cur_line() {
		return *_cur_line_;
	}

	///	Drop the current output line
	static void drop() {
		_cur_line_->drop();
	}

	/// Print the current output line.
	static void print() {
		_cur_line_->output();
	}

	/** \brief Process the current input line and return its line type.
     *  \return The `line_type` of the current input line.
	 */
	static line_type next();

	/*! \brief Substitute a diagnostic insert for the line in the line-buffer
	 *   and print it to output.
     *
	 *  \param	replacement	The diagnostic insert to print.
     *
	 *  Depending on the policy selected by the specified or default value of
	 *  the `--conflict option`, a `#define` or `#undef` directive read
     *  from input that contradicts one of the `--define` or `--undefine`
     *  options may be replaced on output with a diagnostic comment or a
	 *	diagnostic `#error` directive.
	 */
	static void  substitute(std::string const & replacement);

	/// Get a reference to the count of suppressed lines.
	static unsigned & lines_suppressed() {
		return _lines_suppressed_;
	}

	/// Get a reference to the count of changed lines.
	static unsigned & lines_changed() {
		return _lines_changed_;
	}

	/// Get a pretty printable version of the current input line
	static std::string pretty();

private:

	/// Number of input lines suppressed.
	static unsigned _lines_suppressed_;
	/// Number of input lines changed
	static unsigned _lines_changed_;
	/// The current output line
	static std::unique_ptr<parsed_line> _cur_line_;

};

#endif // EOF
