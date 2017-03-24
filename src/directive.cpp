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
#include "directive.h"
#include "expression_parser.h"
#include "integer_constant.h"
#include "diagnostic.h"
#include "lexicon.h"
#include "io.h"
#include "hash_include.h"
#include "if_control.h"
#include "contradiction.h"
#include "canonical.h"
#include "symbol.h"
#include <iostream>

/** \file directive.cpp
 *	This file implements specializations of `template struct directive<Type>`
 */

using namespace std;

/// \cond NO_DOXYGEN

template<> std::string const directive<HASH_IF>::_keyword_ = TOK_IF;
template<> std::string const directive<HASH_IFDEF>::_keyword_ = TOK_IFDEF;
template<> std::string const directive<HASH_IFNDEF>::_keyword_  = TOK_IFNDEF;
template<> std::string const directive<HASH_ELSE>::_keyword_  = TOK_ELSE;
template<> std::string const directive<HASH_ELIF>::_keyword_  = TOK_ELIF;
template<> std::string const directive<HASH_ENDIF>::_keyword_  = TOK_ENDIF;
template<> std::string const directive<HASH_DEFINE>::_keyword_  = TOK_DEFINE;
template<> std::string const directive<HASH_UNDEF>::_keyword_  = TOK_UNDEF;
template<> std::string const directive<HASH_INCLUDE>::_keyword_  = TOK_INCLUDE;
template<> std::string const directive<HASH_PRAGMA>::_keyword_  = TOK_PRAGMA;
template<> std::string const directive<HASH_ERROR>::_keyword_  = TOK_ERROR;
template<> std::string const directive<HASH_LINE>::_keyword_  = TOK_LINE;

template<>
map<string,bool> directive<HASH_IF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_IFDEF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_IFNDEF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_ELSE>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_ELIF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_ENDIF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_DEFINE>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_UNDEF>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_INCLUDE>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_PRAGMA>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_ERROR>::_directives_tab_ = map<string,bool>();
template<>
map<string,bool> directive<HASH_LINE>::_directives_tab_ = map<string,bool>();

vector<directive_base::evaluator> directive_base::_evaluator_tab_ = {
	&directive<HASH_UNKNOWN>::eval,
	&directive<HASH_IF>::eval,
	&directive<HASH_IFDEF>::eval,
	&directive<HASH_IFNDEF>::eval,
	&directive<HASH_ELSE>::eval,
	&directive<HASH_ELIF>::eval,
	&directive<HASH_ENDIF>::eval,
	&directive<HASH_DEFINE>::eval,
	&directive<HASH_UNDEF>::eval,
	&directive<HASH_INCLUDE>::eval,
	&directive<HASH_PRAGMA>::eval,
	&directive<HASH_ERROR>::eval,
	&directive<HASH_LINE>::eval
};
map<string, directive_type>
directive_base::_keyword_to_type_map_ = {
	{	string(TOK_IF),         	HASH_IF },
	{	string(TOK_IFDEF),      	HASH_IFDEF },
	{	string(TOK_IFNDEF),     	HASH_IFNDEF},
	{	string(TOK_ELSE),       	HASH_ELSE },
	{	string(TOK_ELIF),       	HASH_ELIF },
	{	string(TOK_ENDIF),      	HASH_ENDIF },
	{	string(TOK_DEFINE),     	HASH_DEFINE },
	{	string(TOK_UNDEF),      	HASH_UNDEF },
	{	string(TOK_INCLUDE),    	HASH_INCLUDE },
	{	string(TOK_PRAGMA),     	HASH_PRAGMA },
	{	string(TOK_ERROR),      	HASH_ERROR },
	{	string(TOK_LINE),			HASH_LINE }
};

directive_type directive_base::get_type(std::string const & keyword)
{
	auto where = _keyword_to_type_map_.find(keyword);
	if (where == _keyword_to_type_map_.end()) {
		warning_unknown_directive() << "Unknown directive #"
		                            << keyword << emit();
		return HASH_UNKNOWN;
	}
	return where->second;
}

void directive_base::report(std::string const & keyword,
                            std::string const & arg)
{
	if (!line_despatch::cur_line().reportable() ||
			options::get_command() == CMD_SYMBOLS) {
		return;
	}
	std::cout << '#' << keyword << ' ' << arg;
	if (options::list_location()) {
		std::cout << ": " << io::in_file_name()
			<< '(' << line_despatch::cur_line().num() << ')';
	}
	std::cout << '\n';
}

void directive_base::report(bool seen,
                            std::string const & keyword,
                            std::string const & arg)
{
	if (!seen) {
		report(keyword,arg);
	} else if (!options::list_only_once()) {
		report(keyword,arg);
	}
}

line_type
directive_base::eval_ifdef_or_ifndef(
    directive_type type,
    chewer<parse_buffer> & chew)
{
	chew(greyspace);
	symbol::locator sloc(chew);
	chew(greyspace);
	line_type retval = LT_IF;
	if (sloc->configured()) {
		if (sloc->defined()) {
			/* symbol is -Ded*/
			retval = type == HASH_IFDEF ?
			         LT_TRUE : LT_FALSE;
		} else {
			/* symbol is -Ued*/
			retval = type == HASH_IFNDEF ?
			         LT_TRUE : LT_FALSE;
		}
	} else {
		if (options::implicit()) {
			retval = type == HASH_IFDEF ?
			         LT_FALSE : LT_TRUE;
		}
	}
	sloc->report();
	return retval;
}

line_type directive_base::eval(directive_type type, chewer<parse_buffer> & chew)
{
	chew(greyspace);
	line_despatch::cur_line().set_directive_type(type);
	line_type retval = _evaluator_tab_[type](chew);
	chew(greyspace);
	if (chew) {
		if (retval != LT_PLAIN &&
			type != HASH_IF && type != HASH_ELIF) {
			string pretty = line_despatch::pretty();
			string good = pretty.substr(0,size_t(chew));
			string bad = pretty.substr(size_t(chew));
			warning_garbage_after_directive() <<
				"Superfluous \"" << bad << "\" after directive \""
				<< good << '\"' << emit();
		}
	}
	if (line_despatch::cur_line().dropping()) {
		contradiction::forget();
	} else if (type != HASH_UNDEF) {
		contradiction::flush();
	}
	return retval;
}

void directive_base::erase_all() {
	auto command = options::get_command();
	if (command == CMD_INCLUDES || command == CMD_DIRECTIVES) {
		directive<HASH_INCLUDE>::_directives_tab_.clear();
	}
	if (command == CMD_PRAGMAS || command == CMD_DIRECTIVES) {
		directive<HASH_PRAGMA>::_directives_tab_.clear();
	}
	if (command == CMD_DEFS || command == CMD_DIRECTIVES) {
		directive<HASH_DEFINE>::_directives_tab_.clear();
		directive<HASH_UNDEF>::_directives_tab_.clear();
	}
	if (command == CMD_ERRORS || command == CMD_DIRECTIVES) {
		directive<HASH_ERROR>::_directives_tab_.clear();
	}
	if (command == CMD_LINES || command == CMD_DIRECTIVES) {
		directive<HASH_LINE>::_directives_tab_.clear();
	}
}

template<>
line_type directive<HASH_UNKNOWN>::eval(chewer<parse_buffer> &)
{
	return LT_PLAIN;
}


template<>
line_type directive<HASH_IF>::eval(chewer<parse_buffer> & chew)
{
	line_type lineval = LT_IF;
	chew(greyspace);
	if (!chew) {
		error_if_without_cond() << "#if/elif has no argument." << emit();
	}
	if (!options::eval_wip()) {
		size_t mark = size_t(chew);
		integer i = integer_constant::read_numeral(chew);
		if (i.type() == INT_INT &&
				size_t(chew) == mark + 1 && (i.raw() == 1 || i.raw() == 0)) {
			chew(greyspace);
			if (!chew) {
				return lineval;
			}
		}
		chew = mark;
	}
	expression_parser<parse_buffer> ep(chew);
	evaluation ev = ep.result();
	if (ev.resolved()) {
		lineval = ev.is_true() ? LT_TRUE : LT_FALSE;
	}
	if (ep.is_simplified() && lineval == LT_IF) {
		line_despatch::cur_line().replace(ep.simplified());
		line_despatch::cur_line().set_simplified(ep.is_simplified());
	}
	return lineval;
}

template<>
line_type directive<HASH_IFDEF>::eval(chewer<parse_buffer> & chew)
{
	return eval_ifdef_or_ifndef(_type_,chew);
}

template<>
line_type directive<HASH_IFNDEF>::eval(chewer<parse_buffer> & chew)
{
	return eval_ifdef_or_ifndef(_type_,chew);
}

template<>
line_type directive<HASH_ELSE>::eval(chewer<parse_buffer> &)
{
	return LT_ELSE;
}

template<>
line_type directive<HASH_ELIF>::eval(chewer<parse_buffer> & chew)
{
	return
	    static_cast<line_type>(directive_base::eval(HASH_IF,chew)
	                           - LT_IF + LT_ELIF);
}

template<>
line_type directive<HASH_ENDIF>::eval(chewer<parse_buffer> &)
{
	return LT_ENDIF;
}

template<>
line_type directive<HASH_DEFINE>::eval(chewer<parse_buffer> & chew)
{
	line_type retval = line_despatch::cur_line().dropping() ?
	                   LT_DIRECTIVE_DROP : LT_DIRECTIVE_KEEP;
	chew(greyspace);
	symbol::locator sloc(chew);
	string definition;
	formal_parameter_list macro_params;
	chew(continuation);
	if (chew) {
		if (*chew == '(') {
			macro_params.read(chew);
			if (!macro_params.well_formed()) {
				error_malformed_macro()
				<< "Malformed macro parameter for symbol \""
				<< sloc.id() << "\"" << emit();
				return LT_DIRECTIVE_KEEP;
			}
		}
		definition = canonical<string>(chew);
	}
	if (line_despatch::cur_line().reportable()) {
		string arg = sloc.id();
		if (definition.length()) {
			arg += ' ';
			arg += definition;
		}
		directive<HASH_DEFINE>(arg).report();
	}
	if (!line_despatch::cur_line().dropping()) {
		retval = sloc->digest_transient_define(macro_params,definition);
	}
	sloc->report();
	diagnostic_base::discard(severity::error);
	return retval;
}

template<>
line_type directive<HASH_UNDEF>::eval(chewer<parse_buffer> & chew)
{
	line_type retval = line_despatch::cur_line().dropping() ?
	                   LT_DIRECTIVE_DROP : LT_DIRECTIVE_KEEP;

	chew(greyspace);
	symbol::locator sloc(chew);
	if (!line_despatch::cur_line().dropping()) {
		retval = sloc->digest_transient_undef();
	}
	directive<HASH_UNDEF>(sloc.id()).report();
	sloc->report();
	chew(greyspace);
	return retval;
}

template<>
line_type directive<HASH_INCLUDE>::eval(chewer<parse_buffer> & chew)
{
	line_type retval = line_despatch::cur_line().dropping() ?
	                   LT_DIRECTIVE_DROP : LT_DIRECTIVE_KEEP;
	chew(greyspace);
	if (!chew) {
		warning_no_argument() << "#include has no argument" << emit();
	} else {
		hash_include hi(chew);
		chew(greyspace);
		if (retval == LT_DIRECTIVE_KEEP && !hi.valid()) {
			warning_invalid_include()
				<< "#include expects <FILENAME> or \"FILENAME\""
				<< emit();
		}
		shared_ptr<reference> ref = hi.symbolic_argument();
		if (ref) {
			ref->report();
		}
		hi.report();
	}
	return retval;
}

template<>
line_type directive<HASH_PRAGMA>::eval(chewer<parse_buffer> & chew)
{
	if (line_despatch::cur_line().reportable()) {
		string str = canonical<string>(chew);
		directive<HASH_PRAGMA>(str).report();
	}
	return LT_PLAIN;
}

template<>
line_type directive<HASH_ERROR>::eval(chewer<parse_buffer> & chew)
{
	if (line_despatch::cur_line().reportable()) {
		string str = canonical<string>(chew);
		directive<HASH_ERROR>(str).report();
	}
	if (!line_despatch::cur_line().dropping()) {
		if (if_control::is_unconditional_line()) {
			if (if_control::was_unconditional_line()) {
				warning_unconditional_error_input() <<
					"An operative #error directive was input"
					<< emit();
			} else {
				warning_unconditional_error_output() <<
					 "An operative #error directive was output" << emit();
			}
		}
	}
	return LT_PLAIN;
}

template<>
line_type directive<HASH_LINE>::eval(chewer<parse_buffer> & chew)
{
	chew(greyspace);
	size_t off = size_t(chew);
	if (!chew) {
		warning_no_argument() << "#line has no argument." << emit();
	} else if (line_despatch::cur_line().reportable()) {
		string str = canonical<string>(chew);
		directive<HASH_LINE>(str).report();
	}
	expression_parser<parse_buffer> ep(chew = off);
	evaluation ev = ep.result();
	if (!ev.resolved() || int(ev.value().raw()) < 1) {
		warning_not_a_line_number() <<
			"#line expects a line-number" << emit();
	}
	return LT_PLAIN;
}

/// \endcond NO_DOXYGEN


// EOF
