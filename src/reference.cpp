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

/** \file reference.cpp
 *   This file implements `struct reference`.
 */

#include "reference.h"
#include "expansion_base.h"
#include "options.h"
#include "io.h"
#include "expression_parser.h"
#include "diagnostic.h"
#include "citable.h"
#include "options.h"
#include "line_despatch.h"
#include <iostream>

using namespace std;

bool reference::explaining() const {
	if (!_invoker) {
		return
		line_despatch::cur_line().reportable() &&
		options::explain_references() &&
		(_referee->in_progress() || _referee->invoked()) &&
		!_referee->deselected() &&
		!_referee->self_referential();
	}
	return _invoker->explaining();
}

reference::insert_result
reference::lookup() {
	reference_cache::iterator loc = lower_bound();
	if (reference_cache::at_end(loc) || loc->first != _key) {
		reference_cache::value_type v = digest();
		if (!_referee->self_referential()) {
			_referee->make_clean();
		}
		return insert_result(reference_cache::insert(v,loc),true);
	}
	if (_referee->dirty()) {
		loc->second = digest().second;
		if (!_referee->self_referential()) {
			_referee->make_clean();
		}
		return insert_result(loc,true);
	}
	return insert_result(loc,false);
}

reference_cache::value_type reference::digest()
{
	bool exp = explain();
	if (exp) {
		unsigned lineno = line_despatch::cur_line().num();
		cout << "Expanding \"" << invocation();
		if (lineno) {
			cout << "\" at " <<	io::in_file_name() << '(' << lineno << ")\n";
		} else {
			cout << " in options\n";
		}
	}
	reference_cache::entry resolved = expand(exp);
	if (!resolved.eval().insoluble() && !resolved.eval().empty()) {
		string & s = const_cast<string &>(resolved.expansion());
		chewer<string> chew(chew_mode::plaintext,s);
		chew(string_literal); // Expands as string literal?
		if (size_t(chew) == 0) {
			chew(header_name); // Expands as header name?
		}
		if (size_t(chew) == 0) {
			resolved.eval() = evaluation(expression_parser<string>(chew,this));
		} else {
			resolved.eval().set_insoluble();
		}
	}
	return reference_cache::value_type(_key,resolved);
}

reference_cache::entry reference::expand(bool explain)
{
	evaluation eval = validate();
	if (eval.insoluble()) {
		return reference_cache::entry(invocation(),eval);
	}
	if (eval.empty()) {
		return reference_cache::entry(string(),eval);
	}
	unique_ptr<expansion_base> pe = expansion_base::factory(explain,*this);
	try {
        pe->expand();
	}
	catch(expansion_base const & eb) {
	    eval.set_insoluble();
	    return reference_cache::entry(
                            pe->reference::invocation(),eval,false,false);
	}
	return reference_cache::entry(pe->value(),eval,false,true);
}


bool reference::reportable() const {
	return (options::get_command() == CMD_SYMBOLS &&
			line_despatch::cur_line().reportable() &&
			!_referee->deselected() &&
			(!_invoker || _referee->configured()));
}

void reference::do_report()
{
	insert_result result = lookup();
	bool clean = result.second;
	reference_cache::entry & entry = result.first->second;
	if (entry.reported() && options::list_at_most_once_per_file()) {
		return;
	}
	entry.set_reported();
	bool lineno = line_despatch::cur_line().num();
	char const * adverb = (clean || lineno == 0) ? "afresh " : "unchanged ";
	cout << invocation();
	switch(_referee->origin()) {
	case symbol::provenance::unconfigured:
		cout << ": unconfigured";
		break;
	case symbol::provenance::global:
		cout << ": global";
		break;
	case symbol::provenance::transient:
		cout << ": transient";
		break;
	default:
		assert(false);
	}
	if (options::expand_references()) {
		if (options::explain_references()) {
			if (_referee->defn()) {
				cout << ": def. >>";
				if (_referee->parameters()) {
					cout << _referee->signature() << '=';
				}
				cout << *_referee->defn() << "<<";
			} else {
				cout << ": undef";
			}
		}
		if(_referee->configured()) {
			if (_referee->parameters().variadic()) {
				cout << ": insoluble, because variadic macro parameters "
					"are unsupported";
			} else {
				if (_referee->self_referential()) {
					cout << ": insoluble, because of infinite regress";
				} else if (entry.complete()) {
                    cout << ": expands " << adverb
                    << "as >>" + entry.expansion() + "<<";
                    if (entry.eval().resolved()) {
                        cout << ": evaluates to "
                        << citable(entry.eval().value());
                    }
                } else {
                    cout << ": insoluble, because macro expansion too large";
                }
			}
		} else {
			cout << ": insoluble";
		}
	}
	if (lineno == 0) {
		cout << ": in options";
	} else if (options::list_location()) {
		cout << ": " << io::in_file_name()
			<< "(" << line_despatch::cur_line().num() << ")";
	}
	cout << '\n';
}

evaluation reference::validate() const
{
	evaluation eval;
	if (!_args.well_formed()) {
		error_malformed_macro() <<
			"Malformed argument list for \""
			<< invocation() << "\"" << emit();
		eval.set_insoluble();
	} else if (_referee->self_referential()) {
		warning_infinite_regress() << "Infinite regress in expansion of \""
			<< invocation()  << '\"' << emit();
		eval.set_insoluble();
	} else if (_referee->parameters().variadic()) {
		warning_variadic_macros_unsupported() <<
			"Sorry, variadic macro expansion is unsupported. "
			<< "\"" << invocation() << "\"" <<
			" will not be resolved" << emit();
		eval.set_insoluble();
	} else if (_args.size() != _referee->parameters().size()) {
		if (_referee->configured()) {
			error_mismatched_macro_args()
				<< '\"' << _referee->signature()
				<< "\" takes " << _referee->parameters().size()
				<< " arguments; " << _args.size() << " provided" << emit();
		} else if (_referee->invoked()){
			warning_inconsistent_calls()
				<< "Unconfigured symbol \"" << _referee->id()
				<< "\" has already taken " << _referee->parameters().size()
				<< " arguments; " << _args.size() << " provided" << emit();
		}
		eval.set_insoluble();
	} else if (!_referee->configured()) {
		if (options::implicit()) {
			if (_args) {
				error_macro_call_on_undefined()
				<< "Function-like invocation of undefined symbol: \""
				<< invocation() << "\" (option --implicit)" << emit();
				eval.set_insoluble();
			}
		} else {
			eval.set_insoluble();
		}
	} else if (_referee->defn() && _referee->defn()->empty()) {
		eval.set_empty();
	}
	return eval;
}

// EOF

