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

#include "symbol.h"
#include "line_despatch.h"
#include "diagnostic.h"
#include "reference.h"
#include "canonical.h"
#include "contradiction.h"
#include "if_control.h"
#include <algorithm>
#include <cstring>
#include <cassert>

/** \file symbol.cpp
 *   This file implements `struct symbol`.
 */

/// \cond NO_DOXYGEN

using namespace std;

symbol::table_entry
symbol::_null_("",symbol(provenance::unconfigured));

map<string,symbol>
symbol::_sym_tab_(&symbol::_null_,&symbol::_null_ + 1);

set<string> symbol::_selected_symbols_set_;

int symbol::_current_snapshot_ = 0;
int symbol::_last_global_snapshot_ = 0;

int symbol::snapshot_max() const
{
	int max = _snapshot;
	auto first = _contributors.begin();
	auto last = _contributors.end();
	for (	;first != last; ++first) {
		int submax = (*first)->snapshot_max();
		if (max < submax) {
			max = submax;
		}
	}
	return max;
}

void symbol::set_definition(string const & defn)
{
	_defn.reset(new string(defn));
}

void symbol::define(string const & defn, formal_parameter_list const & params)
{
	reference_cache::erase_symbol(id());
	make_dirty(pseudo_snapshot::define_in_progress);
	unsubscribe();
	set_definition(defn);
	set_parameters(params);
	subscribe();
	_line = line_despatch::cur_line().num();
}

void symbol::undef()
{
	reference_cache::erase_symbol(id());
	make_dirty(pseudo_snapshot::undef_in_progress);
	unsubscribe();
	_defn.reset();
	_params = formal_parameter_list();
	_format.reset();
	_line = line_despatch::cur_line().num();
}

bool symbol::selected(string const & name)
{
	set<string>::const_iterator lb =
		_selected_symbols_set_.lower_bound(name);
	if (lb != _selected_symbols_set_.end() &&
			(name == *lb || wildcard_match(*lb,name))) {
		return true;
	}
	return lb != _selected_symbols_set_.begin() && wildcard_match(*--lb,name);
}

bool symbol::deselected(string const & id)
{
	return options::selected_symbols() && !selected(id);
}

bool symbol::wildcard_match(string const & wildcard, string const & name)
{
#if CXX11_HAVE_STRING_BACK
	if (wildcard.back() != '*') {
#else
	if (*(--wildcard.end()) != '*') {
#endif
		return false;
	}
	size_t min_len = wildcard.length() - 1;
	if (name.length() < min_len) {
		return false;
	}
	return memcmp(name.data(),wildcard.data(),min_len) == 0;
}

void symbol::set_selection(char const *optarg)
{
	string arg(optarg);
	size_t follower = 0;
	size_t leader = arg.find(',');
	for (	;leader != string::npos;
			follower = leader + 1,leader = arg.find(',',follower)) {
		string pattern = arg.substr(follower,leader - follower);
		if (pattern.length()) {
			add_pattern(pattern);
		}
	}
	if (follower < arg.length()) {
		add_pattern(arg.substr(follower));
	}
}


void symbol::report_premiere() {
	auto first = _contributors.begin();
	auto const last = _contributors.end();
	for (	;first != last; ++first) {
		(*first)->report_premiere();

	}
	if (origin() == provenance::global) {
		reference ref(_loc);
		if (!ref.reported()) {
			ref.report();
		}
	}
}

size_t symbol::count(provenance source)
{
	auto i = _sym_tab_.begin();
	size_t nsyms = 0;
	for (   ; i != _sym_tab_.end(); ++i) {
		nsyms += i->second._provenance == source;
	}
	return nsyms;
}

void symbol::report_global_config()
{
	static bool done = false;
	if (done) {
		return;
	}
	done = true;
	line_despatch::cur_line().set_directive_type(COMMANDLINE);
	auto i = _sym_tab_.begin();
	for (	i = _sym_tab_.begin(); i != _sym_tab_.end(); ++i) {
		if (i->second.origin() == provenance::global) {
			i->second.report_premiere();
		}
	}
}

void symbol::subscribe_to(locator other)
{
	if (which_parameter(other.id()) != string::npos) {
		return;
	}
	if (_loc == other) {
		make_self_referential();
		return;
	}
	if (other->subscribes_to(_loc)) {
		make_self_referential();
		other->make_self_referential();
		return;
	}
	if (find(_contributors.begin(),_contributors.end(),other) ==
			_contributors.end()) {
		_contributors.push_back(other);
		other->_subscribers.push_back(_loc);
	}
	for (auto i = other->_contributors.begin();
			i != other->_contributors.end(); ++i) {
		subscribe_to(*i);
	}
}

bool symbol::subscribes_to(symbol::locator other) const {
	return find(_contributors.begin(),_contributors.end(),other) !=
			_contributors.end();
}

void symbol::subscribe()
{
	if (!_defn || _defn->empty()) {
		return;
	}
	assert(_contributors.empty());
	chewer<string> chew(chew_mode::plaintext,*_defn);
	string id;
	size_t off = 0;
	while(((id = identifier::find_any_in(chew,off)),!id.empty())) {
		locator loc(id);
		subscribe_to(loc);
		argument_list args(chew);
		if (args && _loc != loc && !loc->configured() && !loc->invoked()) {
			loc->set_parameters(args.size());
		}
	}
}

void symbol::unsubscribe() {
	auto i = _contributors.begin();
	for (	;i != _contributors.end(); ++i) {
		auto & contributes_to = (*i)->_subscribers;
		auto j = find(contributes_to.begin(),contributes_to.end(),_loc);
		if (j != contributes_to.end()) {
			contributes_to.erase(j);
		}
	}
	_contributors.clear();
}

void symbol::per_file_init()
{
	if (count() < size_t(_last_global_snapshot_) + 1) {
		return;
	}
	// Skip the null symbol
	auto i = ++_sym_tab_.begin();
	// Unsubscribe all symbols
	for ( ;i != _sym_tab_.end(); ++i) {
		i->second.unsubscribe();
	}

	// 	Delete all transients
	for (i = ++_sym_tab_.begin(); i != _sym_tab_.end();) {
		if (i->second.origin() == provenance::transient) {
			i = _sym_tab_.erase(i);
		} else {
			++i;
		}
	}

	// Prep remaining symbols
	for (i = ++_sym_tab_.begin(); i != _sym_tab_.end(); ++i) {
		if (options::list_once_per_file()) {
			reference_cache::erase_symbol(i->first);
		}
		if (i->second.origin() == provenance::global) {
			i->second.subscribe();
		} else {
			i->second.clear_parameters();
			i->second.set_invoked(false);
		}
	}
	_current_snapshot_ = count();
	if (options::get_command() == CMD_SYMBOLS && options::expand_references()) {
		report_global_config();
	}
}

void symbol::report() const
{
	if (!line_despatch::cur_line().reportable() ||
			options::get_command() != CMD_SYMBOLS) {
		return;
	}
	reference ref(_loc);
	if (!clean() && !_contributors.empty()) {
		assert(_defn);
		for (auto i = _contributors.begin(); i != _contributors.end(); ++i) {
			size_t off = 0;
			chewer<string> chew(chew_mode::plaintext,*_defn);
			string const & id = (*i)->id();
			vector<argument_list> args_seen;
			while ((off = identifier::find_first_in(id,chew)) != string::npos) {
				argument_list args(chew);
				if (find(args_seen.begin(),args_seen.end(),args)
						== args_seen.end()) {
					args_seen.push_back(args);
					reference r(*i,args,&ref);
					r.report();
				}
			}
		}
	}
	ref.report();
}

void symbol::make_dirty(pseudo_snapshot n) {
	char const * action = nullptr;
	switch(n) {
	case pseudo_snapshot::define_in_progress:
		action = "Defining";
		break;
	case pseudo_snapshot::undef_in_progress:
		action = "Undefining";
		break;
	case pseudo_snapshot::infinite:
		break;
	default:
		assert(false);
	}
	for (auto i = _subscribers.begin(); i != _subscribers.end(); ++i) {
		if (!(*i)->dirty()) {
			(*i)->make_dirty(n);
		}
		if (action) {
			info_retrospective_redefinition()
			<< action << " \"" << id()
			<< "\" retrospectively affects the meaning of \""
			<< (*i)->signature() << '\"' << emit();
		}
	}
	_snapshot = int(n);
}

void symbol::digest_global_define(chewer<string> & chew)
{
	string definition;
	size_t off = size_t(chew);
	formal_parameter_list params(chew);
	if (!params.well_formed()) {
		error_malformed_macro() <<
			"Malformed macro parameter list for symbol \""
			<< id() << "\"" << emit();
		return;
	}
	if (*chew == '=') {
		++chew;
	} else if (chew) {
		error_garbage_arg()
		        << "Garbage in \"-D" << id()
				<< chew.buf().substr(off) << '\"' << emit();
		return;
	}
	definition = canonical<string>(chew);
	if (configured()) {
		/* -D option defining prior global  */
		if (_defn) { /* symbol is already defined */
			if (definition != *_defn ||
			    params.size() != parameters().size()) {
				/* Definitions differ */
				error_invalid_args() << "\"-D" << id() << params.str()
					 << '=' << definition
					 << "\" contradicts prior \"-D" << id()
					 << parameters().str()
					 << '=' << *_defn << '\"' << emit();
				return;
			} else { /* Definitions the same */
				warning_duplicate_args()
					<<  "Duplicated \"-D" << id() << parameters().str()
					<< '=' << definition << "\" ignored" << emit();
				return;
			}
		} else  {
			/* -D contradicts prior -U option */
			error_invalid_args() << "\"-D" << id() << params.str()
					 << '=' << definition << "\" contradicts prior \"-U"
					 << id() << '\"' << emit();
			return;
		}
	}
	set_pseudo_snapshot(pseudo_snapshot::define_in_progress);
	set_definition(definition);
	set_parameters(params);
	_provenance = provenance::global;
}

line_type
symbol::digest_transient_define(formal_parameter_list const & params,
                                string const & definition)
{
	if (configured()) {
		bool is_global = _provenance == provenance::global;
		/* This is a re-configuration of an already configured symbol */
		if (_defn) { /* symbol is already defined */
			if (definition != *_defn ||
			    params.size() != parameters().size()) {
				if (!is_global) {
					/* Differing #define contradicts prior #define */
					warning_differing_redef() <<  "Differently redefining \""
						  << id() << params.str()
						  << '=' << definition << "\", after \""
						  << id() << parameters().str()
						  << '=' << *_defn << "\" at line "
						  << _line << emit();
				} else {
					/* Differing #define contradicts -D option */
					contradiction::flush();
					contradiction::insert(
                            contradiction::cause::DIFFERENTLY_REDEFING_D,id());
					return LT_DIRECTIVE_DROP;
				}
			}
			/* Definitions the same */
			else if (!is_global) {
				/* #define duplicating #define */
				return LT_DIRECTIVE_KEEP;
			} else { /* #define duplicating -D option */
				if (id() == contradiction::last_conflicted_symbol_id()) {
					contradiction::forget();
				}
				return LT_DIRECTIVE_DROP;
			}
		}
		/* symbol is already undefined. */
		else if (is_global) {
			/* #define contradicts -U option */
			contradiction::insert(contradiction::cause::DEFINING_U,id());
			return LT_DIRECTIVE_DROP;
		}
		//* Else #define countervails #undef */
	}
	if (if_control::is_unconditional_line() && !options::no_transients()) {
		if (!configured()) {
			warning_transient_symbol_added()
				<< "\"-D" << id() << params.str()
				<< '=' << definition
				<< "\" has been assumed for the current file" << emit();
		}
		define(definition,params);
		_provenance = provenance::transient;
	} else if (!options::no_transients()) {
		/* 	If we are here the symbol must be unconfigured.
			If it were configured global then we have already returned.
			Hence it could only be configured transient, which is not allowed.
		*/
		set_parameters(params);
		_provenance = provenance::unconfigured;
	}
	return LT_DIRECTIVE_KEEP;
}

void symbol::digest_global_undef(chewer<string> & chew)
{
	if (chew) {
		error_garbage_arg() << "Garbage in \"#undef " << id() << '\"' << emit();
		return;
	}
	if (configured()) {
		if (_defn) { /* symbol is already defined */
			error_invalid_args() << "\"-U" << id() << parameters().str()
				<< "\" contradicts prior \"-D" << id() << '='
				<< *_defn << '\"' << emit();
			return;
		}
		/* symbol is already undefined. */
		else {
			warning_duplicate_args() << "Duplicated \"-U"
			                         << id() << "\" ignored" << emit();
			return;
		}
	}
	set_pseudo_snapshot(pseudo_snapshot::undef_in_progress);
	undef();
	_provenance = provenance::global;
}

line_type
symbol::digest_transient_undef()
{
	if (configured()) {
		bool is_global = _provenance == provenance::global;
		if (_defn) { /* symbol is already defined */
			if (!is_global) {
				/* #undef contradicting prior #define */
				warning_undefing_defined()
				        << "undefining " << id() << ", after defining "
				        << id() << parameters().str()
				        << '=' << *_defn << " at line "
				        << _line << emit();
			} else  { /* #undef contradicting -D option */
				if (contradiction::last_conflicted_symbol_id() != id()) {
					contradiction::flush();
					contradiction::save(contradiction::cause::UNDEFING_D,id());
				}
				return LT_DIRECTIVE_DROP;
			}
		}
		/* symbol is already undefined. */
		else if (is_global) {
			/* #undef echoes -U */
			return LT_DIRECTIVE_DROP;
		} else {
			/* #undef echoes #undef */
			return LT_DIRECTIVE_KEEP;
		}
	}
	if (if_control::is_unconditional_line() && !options::no_transients()) {
		if (!configured()) {
			warning_transient_symbol_added() << "\"-U" << id() <<
					 "\" has been assumed for the current file" << emit();
		}
		undef();
		_provenance = provenance::transient;
	} else if (!options::no_transients()) {
		/* 	If we are here the symbol must be unconfigured.
			If it were configured global then we have already returned.
			Hence it could only be configured transient, which is not allowed.
		*/
		_provenance = provenance::unconfigured;
	}
	return LT_DIRECTIVE_KEEP;
}

template<class CharSeq>
symbol::locator symbol::find_any_in(chewer<CharSeq> & chew, size_t & off)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	string id;
	while((id = identifier::find_any_in(chew,off)),!id.empty()) {
		locator sloc = lookup(id);
		if (sloc) {
			return sloc;
		}
	}
	return locator();
}

template
symbol::locator symbol::find_any_in(chewer<string> &, size_t &);
template
symbol::locator symbol::find_any_in(chewer<parse_buffer> &, size_t &);

/// \endcond NO_DOXYGEN

// EOF
