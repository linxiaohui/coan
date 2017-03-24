/***************************************************************************
 *   Copyright (C) 2013 Mike Kinghan, imk@burroingroingjoing.com           *
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

/** \file parsed_line.cpp
 *	This file implements `struct parsed_line`
 */

#include "parsed_line.h"
#include "io.h"
#include "lexicon.h"
#include "if_control.h"
#include "line_despatch.h"
#include "options.h"
#include "diagnostic.h"
#include "citable.h"

/// \cond NO_DOXYGEN

using namespace std;

size_t parsed_line::extend()
{
	string line;
	getline(*_in,line);
	if (_in->fail() && !_in->eof()) {
		abend_cant_read_input() << "Read error on file "
		                        << io::in_file_name() << emit();
		return 0;
	}
	size_t bytes = line.length();
	if (!_in->eof()) {
		line += '\n';
		++bytes;
	} else if (bytes > 0) {
		warning_missing_eof_newline() <<
			"Missing newline at end of file" << emit();
	}
	_text += line;
	_lineno += bytes != 0;
	return bytes;
}

size_t parsed_line::extend(size_t skip)
{
	if (skip) {
		++_extensions;
		size_t bytes = extend();
		if (bytes) {
			return skip;
		}
		error_eof_too_soon() << "Unexpected end of file" << emit();
	}
	return skip;
}

bool parsed_line::get()
{
	bool got;
	_extensions = 0;
	_simplified = false;
	clear();
	got = extend();
	set_dropping();
	return got;
}


void parsed_line::keyword_lop(std::string const & keyword) {
	_text.resize(_keyword_posn);
	_text += keyword;
	_text += '\n';
	_keyword_len = keyword.length();
	++line_despatch::lines_changed();
}

void parsed_line::keyword_swap(std::string const & directive) {
	_text.replace(_keyword_posn,_keyword_len,directive);
	_keyword_len = directive.length();
	++line_despatch::lines_changed();
}

template<> void parsed_line::keyword_edit<directive_type::HASH_IF>() {
	keyword_swap(TOK_IF);
}

template<> void parsed_line::keyword_edit<directive_type::HASH_IFDEF>() {
	keyword_swap(TOK_IFDEF);
}

template<> void parsed_line::keyword_edit<directive_type::HASH_IFNDEF>() {
	keyword_swap(TOK_IFNDEF);
}

template<> void parsed_line::keyword_edit<directive_type::HASH_ELSE>() {
	keyword_lop(TOK_ELSE);
}

template<> void parsed_line::keyword_edit<directive_type::HASH_ELIF>() {
	keyword_swap(TOK_ELIF);
}

template<> void parsed_line::keyword_edit<directive_type::HASH_ENDIF>() {
	keyword_lop(TOK_ENDIF);
}

void parsed_line::set_dropping()
{
	_dropping = if_control::dead_line() ^ options::complement();
}

void parsed_line::set_reportable()
{
	if (_dtype == COMMANDLINE) {
		_reportable = true;
		return;
	}
	bool verdict;
	switch(options::get_command()) {
	case CMD_SYMBOLS:
		switch(_dtype) {
		case HASH_IFDEF:
		case HASH_IFNDEF:
		case HASH_IF:
			verdict = options::list_symbols_in_ifs();
			break;
		case HASH_DEFINE:
			verdict = options::list_symbols_in_defs();
			break;
		case HASH_UNDEF:
			verdict = options::list_symbols_in_undefs();
			break;
		case HASH_INCLUDE:
			verdict = options::list_symbols_in_includes();
			break;
		case HASH_LINE:
			verdict = options::list_symbols_in_lines();
			break;
		case HASH_UNKNOWN:
			verdict = false;
			break;
		default:
			verdict = true;
		}
		break;
	case CMD_INCLUDES:
		verdict = _dtype == HASH_INCLUDE;
		break;
	case CMD_DEFS:
		verdict = _dtype == HASH_DEFINE || _dtype == HASH_UNDEF;
		break;
	case CMD_PRAGMAS:
		verdict = _dtype == HASH_PRAGMA;
		break;
	case CMD_ERRORS:
		verdict = _dtype == HASH_ERROR;
		break;
	case CMD_LINES:
		verdict = _dtype == HASH_LINE;
		break;
	case CMD_DIRECTIVES:
		verdict = _dtype != HASH_UNKNOWN;
		break;
	default:
		verdict = false;
	}
	if (verdict) {
		if (options::list_only_inactive()) {
			verdict = _dropping && !if_control::is_unconditional_line();
		} else if (options::list_only_active()) {
			verdict = !_dropping && if_control::is_unconditional_line();
		}
	}
	_reportable = verdict;
}

void parsed_line::write_fast() {
	*_out << _text;
}

void parsed_line::write_slow()
{
	size_t i = 0;
	for (	;i < _indent; ++i) {
		*_out << _text[i];
	}
	string output = citable(!options::plaintext(),*this,i);
	line_despatch::lines_changed() += _extensions + 1;
	*_out << output << '\n';
	if (options::get_discard_policy() == DISCARD_BLANK) {
		for (	; _extensions; --_extensions) {
			*_out << '\n';
		}
	} else if (options::get_discard_policy() == DISCARD_COMMENT) {
		for (	; _extensions; --_extensions) {
			*_out << "//coan <\n";
		}
	}
}

void parsed_line::write_commented_out()
{
	if (!is_simplified()) {
		++line_despatch::lines_changed();
		if (!_extensions) {
			*_out << "//coan < ";
			write_fast();
		} else {
			line_despatch::lines_changed() += _extensions;
			size_t start = 0;
			size_t end = _text.find('\n');
			for (	;end != string::npos;
					start = end + 1,end = _text.find('\n',start)) {
				*_out << "//coan < ";
				*_out << _text.substr(start,1 + end - start);
			}
		}
	} else {
		*_out << "//coan < ";
		write_slow();
	}
}

void parsed_line::write()
{
	if (!is_simplified()) {
		write_fast();
	} else {
		write_slow();
	}
}

void parsed_line::write(bool keep)
{
	if (keep ^ options::complement()) {
		write();
	} else {
		if (options::get_discard_policy() == DISCARD_BLANK) {
			line_despatch::lines_changed() += _extensions + 1;
			*_out << '\n';
			for (	; _extensions; --_extensions) {
				*_out << '\n';
			}
		} else if (options::get_discard_policy() == DISCARD_DROP) {
			line_despatch::lines_suppressed() += _extensions + 1;
		} else {
			write_commented_out();
		}
	}
}

void parsed_line::output()
{
	command_code cmd = options::get_command();
	if (cmd == CMD_SOURCE || cmd == CMD_SPIN) {
		if (options::line_directives()) {
			if (_drop_run_length) {
				_drop_run_length = 0;
				if (_extensions) {
					--_lineno;
				}
				*_out << "#line " << _lineno << '\n';
				--line_despatch::lines_suppressed();
				++line_despatch::lines_changed();
			}
		}
		write(true);
	}
}

void parsed_line::drop()
{
	if (options::get_command() == CMD_SOURCE
		|| options::get_command() == CMD_SPIN) {
		write(false);
		if (options::line_directives()) {
			++_drop_run_length;
		}
	}
}

/// \cond NO_DOXYGEN

// EOF
