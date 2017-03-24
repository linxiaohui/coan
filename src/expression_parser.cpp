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
#include "expression_parser.h"
#include "diagnostic.h"
#include "citable.h"
#include "lexicon.h"
#include "integer_constant.h"
#include "canonical.h"
#include "reference.h"
#include "options.h"
#include "line_despatch.h"
#include "expansion_base.h"

/** \file expression_parser.cpp
 *   This file implements `struct expression_parser`
 */

/// \cond NO_DOXYGEN

using namespace std;

template<class CharSeq>
evaluation
expression_parser<CharSeq>::op_or(
    evaluation const & lhs,
    evaluation const & rhs)
{
	if (lhs.is_true() || rhs.is_true()) {
		return evaluation(1);
	} else if (lhs.is_false() && rhs.is_false()) {
		return evaluation(0);
	}
	return evaluation();
}

template<class CharSeq>
evaluation
expression_parser<CharSeq>::op_and(
    evaluation const & lhs,
    evaluation const & rhs)
{
	if (rhs.is_true() && lhs.is_true()) {
		return evaluation(1);
	} else if (rhs.is_false() || lhs.is_false()) {
		return evaluation(0);
	}
	return evaluation();
}

template<class CharSeq>
evaluation
expression_parser<CharSeq>::op_divide(
    evaluation const & lhs,
    evaluation const & rhs)
{
	if (rhs.value().good() && rhs.value().raw() == 0) {
		warning_zero_divide() << "Divide by zero" << emit();
		return evaluation();
	}
	return evaluation(lhs.value() / rhs.value());
}

template<class CharSeq>
evaluation
expression_parser<CharSeq>::op_mod(
    evaluation const & lhs,
    evaluation const & rhs)
{
	if (rhs.value().good() && rhs.value().raw() == 0) {
		warning_zero_divide() << "Divide by zero" << emit();
		return evaluation();
	}
	return evaluation(lhs.value() % rhs.value());
}

template<class CharSeq>
evaluation expression_parser<CharSeq>::op_ternary_either_or(
    evaluation const & lhs,
    evaluation const & rhs)
{
	if (!_ternary_cond_stack.size()) {
		error_ternary_cond_incomplete() <<
			"':' of ternary conditional without preceding '?' "
			"in \"" << citable(!options::plaintext(),_seq)
			<< "\"" << defer();
		return evaluation();
	}
	else {
		evaluation ternary_cond = _ternary_cond_stack.back();
		_ternary_cond_stack.pop_back();
		if (ternary_cond.is_true()) {
			return lhs;
		}
		if (ternary_cond.is_false()) {
			return rhs;
		}
		return evaluation();
	}
}

template<class CharSeq>
evaluation expression_parser<CharSeq>::apply(
    infix_operation op,
    evaluation & lhs, evaluation &rhs)
{
	evaluation result = (this->*op)(lhs,rhs);
	if (!result.resolved()) {
		result.net_infix_ops() = 1 + lhs.net_infix_ops() + lhs.net_infix_ops();
	} else {
		result.net_infix_ops() = 0;
	}
	return result;
}

template<class CharSeq>
template<unsigned Precedence>
pair<typename expression_parser<CharSeq>::infix_operation,bool>
expression_parser<CharSeq>::seek_rightmost_infix(
	chewer<CharSeq> & chew,
	size_t end,
	size_t & op_start,
	size_t & op_end)
{
	infix_operation rightmost_op = nullptr;
	bool short_circuitable = false;
	size_t mark = size_t(chew);
	int paren_balance = 0;
	bool have_infix = false;
	bool have_lhs = false;
	for (	;*chew && size_t(chew) < end;
				have_lhs = !have_infix,chew(greyspace)) {
		if (have_lhs && paren_balance == 0) {
			size_t op_off = size_t(chew);
			switch(*chew) {
			case '?':
				have_infix = true;
				if (Precedence == ternary_if) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_ternary_if;
				}
				continue;
			case ':':
				have_infix = true;
				short_circuitable = true;
				if (Precedence == ternary_either_or) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_ternary_either_or;
				}
				continue;
			case ',':
				have_infix = true;
				short_circuitable = true;
				if (Precedence == comma) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_comma;
				}
				continue;
			case '&':
				have_infix = true;
				if (chew(+1,continuation) && *chew == '&' &&
					Precedence == boolean_and) {
					short_circuitable = true;
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_and;
				} else if (Precedence == bit_and) {
					op_start = op_off;
					op_end = size_t(chew);
					rightmost_op = &expression_parser::op_bit_and;
				}
				continue;
			case '|':
				have_infix = true;
				if (chew(+1,continuation) && *chew == '|' &&
					Precedence == boolean_or) {
					short_circuitable = true;
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_or;
				} else if (Precedence == bit_or) {
					op_start = op_off;
					op_end = size_t(chew);
					rightmost_op = &expression_parser::op_bit_or;
				}
				continue;
			case '^':
				have_infix = true;
				if (Precedence == bit_xor) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_bit_xor;
				}
				continue;
			case '=':
				if (chew(+1,continuation) && *chew == '=') {
					have_infix = true;
					if (Precedence == eq) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_eq;
						continue;
					}
				}
				have_infix = false;
				continue;
			case '<':
				have_infix = true;
				if (chew(+1,continuation)) {
					if (*chew == '=' &&
						Precedence == le) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_le;
						continue;
					}
					if (*chew == '<') {
						if (Precedence == lshift) {
							op_start = op_off;
							op_end = size_t(++chew);
							rightmost_op = &expression_parser::op_lshift;
						}
						++chew;
						continue;
					}
				}
				if (Precedence == lt) {
					op_start = op_off;
					op_end = size_t(chew);
					rightmost_op = &expression_parser::op_lt;
				}
				continue;
			case '>':
				have_infix = true;
				if (chew(+1,continuation)) {
					if (*chew == '=' &&
						Precedence == ge) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_ge;
						continue;
					}
					if (*chew == '>') {
						if (Precedence == rshift) {
							op_start = op_off;
							op_end = size_t(++chew);
							rightmost_op = &expression_parser::op_rshift;
						}
						++chew;
						continue;
					}
				}
				if (Precedence == gt) {
					op_start = op_off;
					op_end = size_t(chew);
					rightmost_op = &expression_parser::op_gt;
				}
				continue;
			case '!':
				if (chew(+1,continuation) && *chew == '=') {
					have_infix = true;
					if (Precedence == neq) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_ne;
					}
					continue;
				}
				have_infix = false;
				continue;
			case '+':
				have_infix = true;
				if (Precedence == add) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_add;
				}
				continue;
			case '-':
				have_infix = true;
				if (Precedence == subtract) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_subtract;
				}
				continue;
			case '*':
				have_infix = true;
				if (Precedence == mult) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_mult;
				}
				continue;
			case '/':
				have_infix = true;
				if (Precedence == divide) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_divide;
				}
				continue;
			case '%':
				have_infix = true;
				if (Precedence == mod) {
					op_start = op_off;
					op_end = size_t(++chew);
					rightmost_op = &expression_parser::op_mod;
				}
				continue;
			case 'a':
				if (chew(+1,continuation) && *chew == 'n' &&
					chew(+1,continuation) && *chew == 'd' &&
					!(++chew && identifier::is_valid_char(*chew))) {
					have_infix = true;
					if (Precedence == boolean_and) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_and;
					}
					continue;
				}
				have_infix = false;
				continue;
			case 'b':
				if (chew(+1,continuation) && *chew == 'i' &&
					chew(+1,continuation) && *chew == 't' &&
					chew(+1,continuation) && *chew == '_') {
					size_t mark = size_t(chew);
					if (chew(+1,continuation) && *chew == 'a' &&
						chew(+1,continuation) && *chew == 'n' &&
						chew(+1,continuation) && *chew == 'd' &&
						!(++chew && identifier::is_valid_char(*chew))) {
						have_infix = true;
						if (Precedence == bit_and) {
							op_start = op_off;
							op_end = size_t(++chew);
							rightmost_op = &expression_parser::op_bit_and;
						}
						continue;
					}
					chew = mark;
					if (chew(+1,continuation) && *chew == 'o' &&
						chew(+1,continuation) && *chew == 'r' &&
						!(++chew && identifier::is_valid_char(*chew))) {
						have_infix = true;
						if (Precedence == bit_or) {
							op_start = op_off;
							op_end = size_t(++chew);
							rightmost_op = &expression_parser::op_bit_or;
						}
						continue;
					}
				}
				have_infix = false;
				continue;
			case 'o':
				if (chew(+1,continuation) && *chew == 'r' &&
					!(++chew && identifier::is_valid_char(*chew))) {
					have_infix = true;
					if (Precedence == boolean_or) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_or;
					}
					continue;
				}
				have_infix = false;
				continue;
			case 'n':
				if (chew(+1,continuation) && *chew == 'o' &&
					chew(+1,continuation) && *chew == 't' &&
					chew(+1,continuation) && *chew == '_' &&
					chew(+1,continuation) && *chew == 'e' &&
					chew(+1,continuation) && *chew == 'q' &&
					!(++chew && identifier::is_valid_char(*chew))) {
					have_infix = true;
					if (Precedence == neq) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_ne;
					}
					continue;
				}
				have_infix = false;
				continue;
			case 'x':
				if (chew(+1,continuation) && *chew == 'o' &&
					chew(+1,continuation) && *chew == 'r' &&
					!(++chew && identifier::is_valid_char(*chew))) {
					have_infix = true;
					if (Precedence == bit_xor) {
						op_start = op_off;
						op_end = size_t(++chew);
						rightmost_op = &expression_parser::op_bit_xor;
					}
					continue;
				}
				have_infix = false;
				continue;
			default:;
			}
		}
		have_infix = false;
		if (*chew == ')' && --paren_balance < 0) {
			break;
		}
		paren_balance += *chew == '(';
		++chew;
	}
	chew = mark;
	return make_pair(rightmost_op,short_circuitable);
}

template<class CharSeq>
void expression_parser<CharSeq>::defer_diagnostic(diagnostic_base & gripe)
{
	if (_ref) {
		gripe << ", in expansion of \"" << _ref->invocation() << '\"';
	}
	gripe << ", in \"" << line_despatch::pretty() << '\"'<< defer();
}

template<class CharSeq>
template<unsigned Precedence>
evaluation
expression_parser<CharSeq>::infix_op(chewer<sequence_type> & chew, size_t end)
{
	pair<infix_operation,bool> op(nullptr,false);
	evaluation lhs;
	size_t start_lhs = size_t(chew);
	size_t end_lhs = start_lhs;
	size_t start_rhs = end_lhs;
	chew(greyspace);
	for (; (op = seek_rightmost_infix<Precedence>(
					chew,end,start_rhs,end_lhs)),op.first != nullptr; ) {
		lhs = next_evaluator<Precedence>(chew,start_rhs);
		if (size_t(chew(greyspace)) != start_rhs) {
			return lhs;
		}
		chew = end_lhs;
		/* Evaluate rhs...*/
		evaluation rhs = next_evaluator<Precedence - 1>(chew,end);
		size_t end_rhs = size_t(chew);
		evaluation result = apply(op.first,lhs,rhs);
		if (lhs.resolved() && !rhs.resolved()) {
			if (op.second || op.first == &expression_parser::op_ternary_if) {
				result.net_infix_ops() = rhs.net_infix_ops();
				result.set_parens_off(rhs.lparen_off(),rhs.rparen_off());
				cut(start_lhs,end_lhs);
			} else if (rhs.net_infix_ops() > 0) {
				restore_paren(rhs.lparen_off(),rhs.rparen_off());
			}
		}
		else if (rhs.resolved() && !lhs.resolved()) {
			if (op.second) {
				result.net_infix_ops() = lhs.net_infix_ops();
				result.set_parens_off(lhs.lparen_off(),lhs.rparen_off());
				cut(start_rhs,end_rhs);
			} else if (lhs.net_infix_ops() > 0) {
				restore_paren(lhs.lparen_off(),lhs.rparen_off());
			}
		} else if (!rhs.resolved() && !lhs.resolved()) {
			if (rhs.net_infix_ops() > 0) {
				restore_paren(rhs.lparen_off(),rhs.rparen_off());
			}
			if (lhs.net_infix_ops() > 0) {
				restore_paren(lhs.lparen_off(),lhs.rparen_off());
			}
		}
		return result;
	}
	return next_evaluator<Precedence - 1>(chew,end);
}

template<class CharSeq>
evaluation
expression_parser<CharSeq>::unary_op(chewer<CharSeq> & chew, size_t end)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	evaluation result;
	chew(greyspace);
	do {
		if (!chew) {
			break;
		}
		if (*chew == '!') {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
				break;
			}
			result.set_value(!result.value());
			break;
		}
		if (*chew == '~') {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
				break;
			}
			result.set_value(~result.value());
			break;
		}
		if (*chew == '(') {
			size_t start = size_t(chew);
			chew(+1,greyspace);
			result = infix_op<max>(chew,end);
			chew(greyspace);
			if (*chew != ')') {
				/* Missing ')'*/
				error_unbalanced_paren() << "Expected \")\" after \""
				<< citable(!options::plaintext(),_seq.substr(0,size_t(chew)))
                    << "\" in \""
				<< citable(!options::plaintext(),_seq) << '\"' << defer();
				result.set_insoluble();
			} else {
				result.set_parens_off(start,size_t(chew));
				if (result.net_infix_ops() < 2) {
					delete_paren(start,size_t(chew));
				}
			}
			chew(+1,greyspace);
			break;
		}
		if (*chew == '+') {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
			}
			break;
		}
		if (*chew == '-') {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
				break;
			}
			result.set_value(-result.value());
			break;
		}
		if (isdigit(*chew)) {
			size_t mark = size_t(chew);
			integer val = integer_constant::read_numeral(chew);
			if (val.type() != INT_UNDEF) {
				if (identifier::is_start_char(*chew)) {
					// Disallow UD-suffix in directive
					chew = mark;
				} else {
					result.set_value(val);
				}
			}
			break;
		}
		size_t mark = size_t(chew);
		string word = canonical<symbol>(chew);
		if (word == TOK_DEFINED) {
			bool paren;
			chew(greyspace);
			paren = *chew == '(';
			if (paren) {
				chew(+1,greyspace);
			}
			symbol::locator sloc(chew);
			chew(greyspace);
			if (paren) {
				if (*chew == ')') {
					chew(+1,greyspace);
				} else {
				/* Missing ')'*/
					error_unbalanced_paren() << "Expected \")\" after \""
					<< citable(!options::plaintext(),_seq.substr(0,size_t(chew)))
                        << "\" in \""
					<< citable(!options::plaintext(),_seq) << '\"' << defer();
					result.set_insoluble();
				}
			}
			sloc->report();
			if (!sloc->configured()) {
				if (options::implicit()) {
					result.set_value(0);
				}
				break;
			}
			result.set_value(sloc->defined());
			break;
		} else if (word == TOK_ALT_BOOLEAN_NOT) {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
				break;
			}
			result.set_value(!result.value());
			break;
		} else if (word == TOK_ALT_BIT_NOT) {
			chew(+1,greyspace);
			result = unary_op(chew,end);
			if (result.net_infix_ops() > 0) {
				restore_paren(result.lparen_off(),result.rparen_off());
				break;
			}
			result.set_value(~result.value());
			break;
		}
		else {
			chew = mark;
		}
		if (*chew == '\'') {
			/* Possible character constant */
			size_t mark = size_t(chew);
			integer val = integer_constant::read_char(chew);
			if (val.type() != INT_UNDEF) {
				if (identifier::is_start_char(*chew)) {
					// Disallow UD-suffix in directive
					chew = mark;
				} else {
					/* Got a character constant */
					result.set_value(val);
				}
				break;
			}
		} else if (identifier::is_start_char(*chew)) {
			size_t mark = size_t(chew);
			if (*chew == 'L' || *chew == 'u' || *chew == 'U') {
				/* Possible prefixed character constant*/
				integer val = integer_constant::read_char(chew);
				if (val.type() != INT_UNDEF) {
					if (identifier::is_start_char(*chew)) {
						// Disallow UD-suffix in directive
						chew = mark;
					} else {
						/* Got a character constant */
						result.set_value(val);
					}
					break;
				}
			}
			if (identifier::is_start_char(*chew)) {
				string id = identifier::read(chew);
				symbol::locator sloc = symbol::lookup(id);
				if (!sloc->configured()) {
					sloc = symbol::locator(id);
					argument_list args(chew);
					if (!sloc->invoked() && args) {
						sloc->set_parameters(args.size());
					}
					reference ref(sloc,args,_ref);
					if (!ref.eval().insoluble() && !ref.args() &&
							options::implicit()) {
						result.set_value(0);
					}
					ref.report();
				} else {
					reference ref(sloc,chew,_ref);
					ref.report();
					result = ref.eval();
					if (sloc->self_referential() || sloc->variadic()) {
						break;
					}
					if (sloc->defined() && sloc->defn()->empty()) {
						error_empty_symbol gripe;
						gripe << '\"' << ref.invocation()
							<< "\" expands to nothing within expression";
						defer_diagnostic(gripe);
					} else if (result.insoluble()) {
					    if (ref.complete()) {
                            error_non_term gripe;
                            gripe << '\"' << ref.invocation()
                                << "\" expands to non-expression >>"
                                << ref.expansion() << "<< within expression";
                            defer_diagnostic(gripe);
					    } else {
					        warning_incomplete_expansion gripe;
                            gripe << "Macro expansion of \""
                            << ref.invocation()
                            << "\" abandoned. "
                            "Will exceed max expansion size "
                            << expansion_base::max_expansion_size()
                            << " bytes";
                            defer_diagnostic(gripe);
					    }
					}
				}
				break;
			}
		}
		else {
			error_ill_formed_expression gripe;
			string good =
                citable(!options::plaintext(),_seq.substr(0,size_t(chew)));
			string bad =
                citable(!options::plaintext(),_seq.substr(size_t(chew)));
			gripe << "Ill-formed \"" << bad;
			if (!good.empty()) {
				gripe << "\" after \"" << good << '\"';
			}
			defer_diagnostic(gripe);
		}

	} while(false);
	return result;
}
template<class CharSeq>
void expression_parser<CharSeq>::parse(chewer<CharSeq> & chew)
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");
	_eval = infix_op<max>(chew,size_t(-1));
	bool orphan_if = false;
	if (_ternary_cond_stack.size()) {
		orphan_if = true;
	}
	chew(greyspace);
	if ((chew || orphan_if)) {
		_eval.set_insoluble();
		std::shared_ptr<diagnostic_base> gripe;
		if (orphan_if) {
			gripe.reset(new error_ternary_cond_incomplete);
			*gripe
				<< "'?' of ternary conditional without following ':' "
				"in \"" << citable(!options::plaintext(),_seq) << '\"';
		} else  {
			string good =
                citable(!options::plaintext(),_seq.substr(0,size_t(chew)));
			string bad =
                citable(!options::plaintext(),_seq.substr(size_t(chew)));
			gripe.reset(new error_ill_formed_expression);
			*gripe << "Ill-formed \"" << bad << "\" after \""
			 << good << '\"';
		}

		if (!_ref) {
			*gripe << " in \"" + line_despatch::pretty() + '\"';
		}
		*gripe << defer();
	}
	if (_cuts == 0) {
		restore_paren();
	}
	if (!_ref) {
		diagnostic_base::flush(severity::error);
	}
}

template
void expression_parser<string>::parse(chewer<string> &);
template
void expression_parser<parse_buffer>::parse(chewer<parse_buffer> &);

template<class CharSeq>
string expression_parser<CharSeq>::simplified() const
{
	string const & in = _seq.str();
	if (is_simplified()) {
		string out;
		size_t i = 0;
		for (	; i < _start; ++i) {
			out += in[i];
		}
		if (i > 0 && _deletions[i] && isgraph(in[i - 1])) {
			out += ' ';
			++i;
		}
		for (	; ptrdiff_t(i) <= _last_deletion; ++i) {
			if (!_deletions[i]) {
				out += in[i];
			}
		}
		if (i + 1 < _seq.size()) {
			out += _seq.substr(i);
		}
		return out;
	}
	return in;
}

template
string expression_parser<parse_buffer>::simplified() const;

template<class CharSeq>
expression_parser<CharSeq>::expression_parser(
	CharSeq & seq,
	reference const * ref,
	size_t start)
: 	_ternary_cond_stack(0),_seq(seq),_start(start),
	_cuts(0),_last_deletion(-1),_ref(ref)
{
	chewer<CharSeq> chew(!options::plaintext(),_seq,start);
	parse(chew);
}

/// \endcond NO_DOXYGEN

/* EOF*/
