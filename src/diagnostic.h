#ifndef DIAGNOSTIC_H
#define DIAGNOSTIC_H
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

#include "prohibit.h"
#include "cloner.h"
#include <sstream>
#include <list>
#include <memory>

/** \file diagnostic.h
 *   This file defines types supporting the production of diagnostics.
 */

/// Enumerated constants representing the severities of diagnostics
enum struct severity {
    /// No severity
    none = 0,
    /// A progress diagnostic
    progress = 1,
    /// An informational diagnostic
    info = 2,
    /// A warning diagnostic
    warning = 4,
    /// An error diagnostic
    error = 8,
    /// A fatal error disgnostic
    abend = 16,
    /// A summary diagnostic
    summary = 32,
    /// A progress summary
    summary_progress = summary | progress,
    /// An informational summary
    summary_info = summary | info,
    /// A warning summary
    summary_warning = summary | warning
};

/** \brief The tag class is inserted in a `diagnostic_base` to tell it to
 *   emit itself
 */
struct emit {};

/** \brief The tag class is inserted in a `diagnostic_base` to tell it to
 *   defer itself
 */
struct defer {};

/// A base class for diagnostic classes
struct diagnostic_base : private no_assign {

    /// Type of pointer to `diagnostic_base`
	using ptr = std::shared_ptr<diagnostic_base>;

	/** \brief Constructor.
     *
	 *   \param severity_name Name of the severity level of the diagnostic
	 *   \param  level   The severity level of the diagnostic
	 *   \param  id      The causal identifier of the diagnostic
	 *   \param  in_code_only True if the diagnostic is applicable only when
	 *           parsing code.
     *
	 */
	diagnostic_base(std::string const & severity_name,
	                severity level, unsigned id,
	                bool in_code_only = false);

	/// Copy constructor.
	diagnostic_base(diagnostic_base const & src)
		: _gagged(src._gagged),_text(src._text.str()) {}


	virtual ~diagnostic_base() = default;

	/// Globally enqueue a copy of this `diagnostic_base` for deferred action.
	void queue() const {
		_queue_.push_back(ptr(clone()));
	}

	/// Get the severity level of the runtime type.
	virtual severity level() const {
		return severity::none;
	}

	/** \brief Get the unique identifier of the runtime type.
     *
	 *  Each distinct cause that may be reported by a diagnostic has a
	 *  distinct identifier.
     *
	 *  \return The unique identifier of the runtime type.
	 */
	virtual unsigned id() const {
		return 0;
	}

	/** \brief Get the reason-code of the diagnostic.
     *
	 *  The reason-code encodes the id of the diagnostic and its
	 *  severity.
     *
	 *  \return The reason-code of the diagnostic.
	 */
	unsigned encode() const {
		return code(level(),id());
	}

	/// Explicitly cast the object to its reason-code.
	explicit operator unsigned () const {
		return encode();
	}

	/// Say whether this diagnostic is suppressed.
	bool gagged() const {
		return _gagged;
	}

	/// Get the text of the diagnostic
	std::string text() const {
		return _text.str();
	}

	/// Explicitly cast the object to its textual representation.
	explicit operator std::string () const {
		return text();
	}

	/** \brief Insert an arbitrary object into the diagnostic.
	 *   \tparam T Type of the object to be inserted.
	 *   \param  obj The `T` to be inserted.
	 *   \return A reference to `*this`.
     *
	 *   The member function inserts `obj` into the text
	 *   of the diagnostic.
	 */
	template<typename T>
	diagnostic_base & operator<<(T const & obj) {
		if (!_gagged) {
			_text << obj;
		}
		return *this;
	}

	/** \brief Queue the diagnostic for deferred action.
     *
	 *   Inserting a token object of type `defer` to the diagnostic
	 *   adds the diagnostic to the deferred queue.
     *
	 *   \return A reference to `*this`.
	 */
	diagnostic_base & operator<<(defer) {
		if (!_gagged) {
			queue();
		}
		return *this;
	}

	/** \brief Emit the diagnostic.
     *
	 *  Inserting a token object of type `emit` to the `diagnostic`
	 *  causes it to be emitted.
     *
	 *  \return A reference to `*this`.
	 */
	diagnostic_base & operator<<(emit) {
		if (!_gagged) {
			despatch();
		}
		return *this;
	}

	/// Count the diagnostics enqueued for deferred action.
	static size_t deferred() {
		return _queue_.size();
	}

	/** \brief Discard any queued diagnostics that match a
	 *  reason-code, returning the number discarded.
	 *  \param  reason The reason-code for selecting
	 *		the queued diagnostics to discard.
	 *  \return The number of discarded diagnostics.
     */
	static size_t discard(unsigned reason) {
		return volley(reason,&diagnostic_base::encode);
	}

	/** \brief Discard any queued diagnostics that match a
	 *   severity, returning the number discarded.
	 *   \param  level The severityfor selecting the queued diagnostics to
	 *      discard.
	 *   \return The number of discarded diagnostics.
	 */
	static size_t discard(severity level) {
		return volley(level,&diagnostic_base::level);
	}

	/// Discard all queued diagnostics.
	static void discard_all() {
		_queue_.resize(0);
	}

	/// Emit all queued diagnostics that match a reason-code
	static size_t flush(unsigned reason) {
		return volley(
					reason,
					&diagnostic_base::encode,
					&diagnostic_base::despatch);
	}

	/// Emit all queued diagnostics that match a severity
	static size_t flush(severity level) {
		return volley(
					level,
					&diagnostic_base::level,
					&diagnostic_base::despatch);
	}

	/// Emit any queued diagnostics
	static void flush_all();


	/** \brief Write summary diagnostics on `cerr` at exit.
     *
	 *   The summary diagnostics will be output unless `--gag summary`
	 *   is in force, which it is by default.
     */
	static void epilogue();

	/** \brief Convert the internal exit status to an exit code for `exit()`.
     *
	 *  \return RC such that:
     *
	 *  - RC = 0:   severity::none: No diagnostics accrued.
     *
	 *  - RC & 0x2:	severity::info: 	At worst info diagnostics accrued.
     *
	 *  - RC & 0x4:	severity::warning: At worst warnings accrued.
     *
	 *  - RC & 0x8:	severity::error:	At worst errors accrued.
     *
	 *  - RC & 0x16: severity::abend:	Execution was aborted.
     *
     *  - RC & 0x12: Input lines were dropped on output (info).
     *
	 *  - RC & 0x22: Input lines were changed on output (info)
     *
	 *  - RC & 0x44: Lines were changed to `#error`s on output (warning).
     *
	 *  - RC & 0x84: Unconditional `#error`s were  output.
	 */
	static int exitcode();

protected:

	/// Increment global counts appropriately for the runtime type.
	virtual void count() const {}

	/// Clone the runtime object on the heap.
	virtual diagnostic_base * clone() const {
	    return new diagnostic_base(*this);
	}

	/// Emit the diagnostic
	void despatch() const throw (unsigned);

	/// Is the diagnostic suppressed?
	bool _gagged;

	/// The text of the diagnostic.
	std::stringstream _text;

	/** \brief Get the reason-code that encodes a causal identifier and
     *  severity
	 *
	 *   \param  level The severity to be encoded.
	 *   \param  id  The causal identifier to be encoded.
	 *   \return The reason-code that encodes a causal identifier and severity
	 *   level.
	 */
	static unsigned code(severity level, unsigned id) {
		return (int(level) << 8) | id;
	}

	/** \brief Queue of deferred diagnostics.
     *
     *   A diagnostic may have to be constructed before we know whether it
	 *   should be issued. In the meantime it can be queued.
	 */
	static std::list<ptr> _queue_;

	/// Global count of informational diagnostics.
	static unsigned _infos_;
	/// Global count of warning diagnostics.
	static unsigned _warnings_;
	/// Global count of error diagnostics.
	static unsigned _errors_;
	/// Global count of fatal error diagnostics.
	static unsigned _abends_;
	/// Global count of `#error` directives output.
	static unsigned _error_directives_generated_;
	/// Global count of operative `#error` directives output.
	static unsigned _error_directives_operative_;

private:

	/** \brief Call a member function on all queued diagnostics for which
	 *	a given member function returns a given result.
	 *	\tparam T Type returned by the selector member function.
	 *   \param  match  The T that shall be returned by the selector function
	 *		to select the diagnostic.
	 *   \param  selector The member method that selects queued diagnostics for
	 *      action.
	 *   \param  action The member method, if any,
	 *		to be called on selected diagnostics, else null.
	 *   \return The number of diagnostics selected.
	 */
	template<class T>
	static size_t
	volley(
		T match,
		T (diagnostic_base::*selector)() const,
		void (diagnostic_base::*action)() const = nullptr)
	{
		if (match == T(0)) {
			return 0;
		}
		size_t shots = 0;
		std::list<ptr>::iterator qit(_queue_.begin());
		while ( qit != _queue_.end()) {
			if ((qit->get()->*selector)() == match) {
				if (action) {
					(qit->get()->*action)();
				}
				qit = _queue_.erase(qit);
				++shots;
			} else {
				++qit;
			}
		}
		return shots;
	}

};

/** \brief `template struct diagnostic<Level,Id>
 *  generically encapsulates a type of diagnostic.
 *
 *  \tparam  Level  The severity of the diagnostic type.
 *   \tparam Id The causal identifier of diagnostic type.
*/
template<severity Level, unsigned Id>
struct diagnostic : cloner<diagnostic_base,diagnostic<Level,Id>>
{

    /// Base class
    using base = cloner<diagnostic_base,diagnostic<Level,Id>>;

	/// The severity level of this diagnostic type.
	static const severity _severity_ = Level;
	/// The causal identifier of this diagnostic type.
	static const unsigned _id_ = Id;

	/** \brief Explicitly construct given a severity name.
	 *   \param  severity_name Severity name of the diagnostic to be
	 *       constructed.
	 */
	explicit diagnostic(std::string const & severity_name)
		: base(severity_name,Level,Id) {}

	/// Get the severity level of this diagnostic.
	severity level() const {
		return Level;
	}

	/// Get the causal identifier of this diagnostic.
	unsigned id() const {
		return Id;
	}

	/// Get the reason-code of this diagnostic's type
	static unsigned code() {
		return diagnostic_base::code(Level,Id);
	}

	/// Forget all queued diagnostics of this type
	static size_t discard() {
		return diagnostic_base::discard(code());
	}

	/// Emit all deferred diagnostics of this type.
	static size_t flush() {
		return diagnostic_base::flush(code());
	}
};

/** \brief template struct progress_msg<Id>` generically
 *  encapsulates a progress diagnostic.
 *   \tparam Id The causal identifier of the progress diagnostic
 */
template<unsigned Id>
struct progress_msg : cloner<diagnostic<severity::progress,Id>,progress_msg<Id>>
{
    /// Base class
    using base = cloner<diagnostic<severity::progress,Id>,progress_msg<Id>>;

	/// Constructor
	progress_msg()
		: base("progress") {}
};

/** \brief `template struct info_msg` generically
 *  encapsulates an informational diagnostic.
 *   \tparam Id The causal identifier of the informational diagnostic
 */
template<unsigned Id>
struct info_msg : cloner<diagnostic<severity::info,Id>,info_msg<Id>>
{
    /// Base class
    using base = cloner<diagnostic<severity::info,Id>,info_msg<Id>>;

	/// Constructor
	info_msg()
		: base("info") {}

private:

	/// Increment the global count of informational diagnostics
	void count() const override {
		++base::_infos_;
	};

};

/** \brief `template struct warning_msg<Id>` generically
 *   encapsulates a warning diagnostic.
 *   \tparam Id The causal identifier of the warning diagnostic
*/
template<unsigned Id>
struct warning_msg : cloner<diagnostic<severity::warning,Id>,warning_msg<Id>>
{
    /// Base class
    using base = cloner<diagnostic<severity::warning,Id>,warning_msg<Id>>;

	/// Constructor
	warning_msg()
		: base("warning") {}

private:
	/// Increment the global count of warning diagnostics
	virtual void count() const override {
		++base::_warnings_;
	};

};

/** \brief `template struct error_msg<Id>` generically encapsulates an error
 *  diagnostic.
 *   \tparam Id The causal identifier of the error diagnostic
*/
template<unsigned Id>
struct error_msg : cloner<diagnostic<severity::error,Id>,error_msg<Id>>
{
    /// Base class
    using base = cloner<diagnostic<severity::error,Id>,error_msg<Id>>;

	/// Constructor
	error_msg()
		: base("error") {}

private:
	/// Increment the global count of error diagnostics
	void count() const override {
		++base::_errors_;
	};

};

/** \brief `template struct abend_msg` generically
 * encapsulates a fatal error diagnostic.
 *
 *   \tparam Id The causal identifier of the fatal error diagnostic
*/
template<unsigned Id>
struct abend_msg : cloner<diagnostic<severity::abend,Id>,abend_msg<Id>>
{
    /// Base class
    using base = cloner<diagnostic<severity::abend,Id>,abend_msg<Id>>;

	/// Constructor
	abend_msg()
		: base("abend") {}

private:
	/// Increment the global count of abend diagnostics
	void count() const override {
		++base::_abends_;
	};

};

/** \brief `template struct progress_summary_msg<Id>`
 * generically encapsulates a progress summary diagnostic.
 *
 * \tparam Id The causal identifier of progress summary diagnostic.
 */
template<unsigned Id>
struct progress_summary_msg :
		diagnostic<severity::summary_progress,Id> {
	/// Default constructor
	progress_summary_msg()
		: diagnostic<severity::summary_progress,Id>("progress") {}
};

/** \brief `template struct info_summary_msg<Id>`
 * generically encapsulates an informational summary diagnostic.
 *
 *  \tparam Id The causal identifier of informational summary diagnostic.
*/
template<unsigned Id>
struct info_summary_msg : diagnostic<severity::summary_info,Id> {
	/// Default constructor
	info_summary_msg()
		: diagnostic<severity::summary_info,Id>("info") {}
};

/** \brief `template struct warning_summary_msg<Id>` generically
 *  encapsulates a warning summary diagnostic.
 *
 *  \tparam Id The causal identifier of the warning summary diagnostic.
 */
template<unsigned Id>
struct warning_summary_msg :
		diagnostic<severity::summary_warning,Id> {
	/// Default constructor
	warning_summary_msg()
		: diagnostic<severity::summary_warning,Id>("warning") {}
};

/// Report file being processed.
using progress_processing_file = progress_msg<1> ;
/// Peport building the input tree.
using progress_building_tree = progress_msg<2>;
/// Report added a file to input.
using  progress_added_file = progress_msg<3>;
/// Report total input files found.
using  progress_file_tracker = progress_msg<4>;
/// Report a commandline argument parsed.
using progress_got_options = progress_msg<5>;
/// Report a duplicate diagnostic selection option
using  info_duplicate_mask = info_msg<1>;
/// Report that input file or directory is symbolic link.
using  info_symlink = info_msg<2>;
/** \brief Report a `#define` or `#undef` that retrospectively affects
 *	the meaning of another symbol.
 */
using  info_retrospective_redefinition = info_msg<3>;

/** \brief Report that same argument occurs for multiple `--define` or
 *	`--undefine` options
 */
using  warning_duplicate_args = warning_msg<1>;

/** \brief Report that an input `#define` or `#undef` directive was deleted.
 *
 *  Report that an input `#define` or `#undef` directive was deleted on
 *  output because it contradicts a `--define` or `--undefine`
 *  option together with `--contradict delete`
 */
using  warning_deleted_contradiction = warning_msg<2>;
/** \brief Report that an input `#define` or `#undef` directive was
 *   commented out.
 *
 *  Report that an input `#define` or `#undef` directive was commented out
 *  on output because it contradicts a `--define` or `--undefine`
 *	option togther with `--contradict comment`
 */
using  warning_commented_contradiction = warning_msg<3>;
/** \brief Report that an input `#define` or `#undef` directive was
 *  converted to an `#error`.
 *
 *  Report that an input `#define` or `#undef` directive was converted to a
 *	`#error` on output because it contradicts a `--define` or `--undefine`
 *	option together with `--contradict error`
 */
using warning_errored_contradiction = warning_msg<4>;
/** \brief Report that an in-source `#define` defines a symbol differently
 *	from a prior one.
 */
using  warning_differing_redef = warning_msg<5>;

/** \brief Report that an in-source `#undef` undefines a symbol previously
 *	defined by an in-source `#define`.
 */
using  warning_undefing_defined = warning_msg<6>;

/** \brief `struct warning_unconditional_error_input'
 * encapsulates a diagnostic for an `#error` directive input unconditionally,
 *   i.e. not in the scope of `#if`
 */
struct warning_unconditional_error_input : warning_msg<7> {
private:
	/// Increment global counts appropriately.
	void count() const override {
		++_warnings_;
		++_error_directives_operative_;
	}
};
/** \brief `template struct warning_error_generated_input'
 *  encapsulates a diagnostic for `#error` directive output
 *  conditionally, i.e. within the scope of an `#if`
 */
struct warning_error_generated : warning_msg<8> {
private:
	/// Increment global counts appropriately.
	void count() const override {
		++_warnings_;
		++_error_directives_generated_;
	}
};
/** \brief `struct warning_unconditional_error_output`
 *  encapsulates a diagnostic for an `#error` directive output
 *  unconditionally, i.e. not in the scope of any `#if`
*/
struct warning_unconditional_error_output : warning_msg<9> {
private:
	/// Increment global counts appropriately.
	void count() const override {
		++_warnings_;
		++_error_directives_operative_;
	}
};
/// Report garbage text was input following a directive
using  warning_garbage_after_directive = warning_msg<10>;
/// Report a newline is missing at end of input
using  warning_missing_eof_newline = warning_msg<11>;
/// Report a symbol has a circular definition
using  warning_infinite_regress = warning_msg<12>;
/// Report the `--verbose` option is mixed with the `--gag` option
using  warning_verbose_only = warning_msg<13>;
/// Report a divide by zero was found in an expression
using warning_zero_divide =  warning_msg<14>;
/** \brief Report directory name ignored on input when `--recurse` not
 *	not specified
 */
using  warning_dir_ignored = warning_msg<15>;
/// Report an unknown preprocessor directive
using  warning_unknown_directive = warning_msg<16>;
/// Report a problematic argument to an \c \#include directive
using  warning_invalid_include = warning_msg<17>;
/// Report that an integer constant evaluates > \c INT_MAX
using warning_int_overflow = warning_msg<18>;
/// Report missing terminator quotation
using  warning_missing_terminator = warning_msg<19>;
/// Report negative shift count
using warning_negative_shift = warning_msg<20>;
/// Report a huge integer constant forced to be unsigned
using warning_forced_unsigned = warning_msg<21>;
/// Report integer sign changed by promotion
using warning_sign_changed = warning_msg<22>;
/// Report a that shift count is >= the width of the shifted quantity
using warning_shift_overflow = warning_msg<23>;
/// Report a character constant too long for current locale
using warning_char_constant_too_long = warning_msg<24>;
/// Report that character constant goes multi-byte
using  warning_mulitbyte_char_constant = warning_msg<25>;
/// Apologize that variadic macro expansion is unsupported.
using warning_variadic_macros_unsupported = warning_msg<26>;
/** Report that a symbol has been transiently defined or undefined for the
 *   duration of the current source file
 */
using warning_transient_symbol_added = warning_msg<27>;
/// Report that the commandline does not specify any `--define` or `--undef`
using warning_no_syms = warning_msg<28>;
/// Report that input file is a broken symbolic link
using warning_broken_symlink = warning_msg<29>;
/// Report that the unsafe `--no-transients` option is used.
using warning_no_transients_used = warning_msg<30>;
/// Report macro ref on unconfigured symbol
using warning_calling_unconfigured = warning_msg<31>;
/// Report a directive that lacks a required argument
using warning_no_argument = warning_msg<32>;
/// Report a `#line` directive with argument not a positive integer.
using warning_not_a_line_number = warning_msg<33>;
/// Report inconsistent argument lists for unconfigured symbol references.
using warning_inconsistent_calls = warning_msg<34>;
/// Report a macro reference not fully expanded.
using warning_incomplete_expansion = warning_msg<35>;

/// Report an orphan `#elif` was found in input
using error_orphan_elif = error_msg<1>;
/// Report an orphan `#else` was found in input
using error_orphan_else = error_msg<2>;
/// Report an orphan `#endif` was found in input
using error_orphan_endif = error_msg<3>;
/// Report unexpected end of input
using  error_eof_too_soon = error_msg<4>;
/// Report that the commandline does not specify anything to do
using  error_nothing_to_do = error_msg<6>;
/** Report that the commandline invalidly specifies multiple input files without
 *	the `--replace` option
 */
using error_one_file_only = error_msg<7>;
/// Report that the commandline options are not a valid combination
using error_invalid_args = error_msg<8>;
/// Report that the commandline is syntactically invalid
using error_usage = error_msg<9>;
/// Report that `#if` nesting exceeds `maxdepth` in input
using error_too_deep = error_msg<10>;
/// Report that the `--file` option occurs more than once
using error_multiple_argfiles =error_msg<11>;
/** Report that the argument to a `--define` or `--undefine` option
 *	contains mysterious characters.
 */
using error_garbage_arg = error_msg<12>;
/// Report that a valid preprocessor identifier was not found where required
using error_not_identifier =  error_msg<13>;
/// Report unbalanced parenthesis
using error_unbalanced_paren = error_msg<14>;
/// Report `#if` or `#elif` has no argument
using error_if_without_cond = error_msg<15>;
/// Report failure from `realpath()`
using error_cant_get_realpath = error_msg<16>;
/// Report a malformed macro parameter list
using error_malformed_macro = error_msg<17>;
/// Report wrong number of macro arguments for symbol
using error_mismatched_macro_args = error_msg<18>;
/// Report attempting macro-ref on undefined symbol
using error_macro_call_on_undefined =  error_msg<19>;
/// Report an incomplete ternary conditional operator
using error_ternary_cond_incomplete = error_msg<20>;
/// Report an ill-formed expression
using error_ill_formed_expression = error_msg<21>;
/** \brief Report a symbol that evaluates to an empty string is an
 *	operand in an expression
 */
using error_empty_symbol = error_msg<22>;
/// Report an infinite regress in the global configuration
using error_infinite_regress= error_msg<23>;
/// Report a `##` operator at start or end of definition.
using error_misplaced_token_paste = error_msg<24>;
/// Report a `#` operator not preceding a parameter for function-like macro
using error_stringify_non_param = error_msg<25>;
/// Report a non-expression in subexpression context
using error_non_term = error_msg<26>;
/// Report that token-pasting does not yield a token
using error_bad_token_paste = error_msg<27>;


/// Report cannot open an output file
using abend_cant_open_output = abend_msg<1>;
/// Report cannot open an input file
using abend_cant_open_input = abend_msg<2>;
/** \brief Report end of input encountered while reading a quoted filename.
 *	a closing quotation was not found
 */
using abend_eof_in_filename = abend_msg<3>;
/// Report an internal logic abend
using abend_confused= abend_msg<4>;
/// Report read error on input
using abend_cant_read_input = abend_msg<5>;
/// Report write error on output
using abend_cant_write_file= abend_msg<6>;
/// Report can't identify file or directory
using abend_no_file = abend_msg<7>;
/// Report invalid filename input
using abend_illegal_filename = abend_msg<8>;
/// Report failure to delete a file
using abend_cant_delete_file = abend_msg<9>;
/// Report failure to rename a file
using abend_cant_rename_file = abend_msg<10>;
/// Report cannot create a temporary file
using abend_no_tempfile = abend_msg<11>;
/// Report cannot open directory
using abend_cant_open_dir = abend_msg<12>;
/// Report read error on directory
using abend_cant_read_dir = abend_msg<13>;
/** \brief Report that a spin directory includes, is included by or is same as
 *   an input directory
 */
using abend_invalid_spin_dir = abend_msg<14>;
/// Report can't get the current working directory
using abend_cant_get_cwd = abend_msg<15>;
/// Report can't create directory
using abend_cant_create_dir = abend_msg<16>;


//! Report processing complete
using progress_summary_all_done = progress_summary_msg<1>;
//! Report total files reached
using info_summary_files_reached = info_summary_msg<1>;
//! Report total files abandoned due to errors
using info_summary_files_dropped = info_summary_msg<2>;
//! Report that input lines were dropped
using info_summary_summary_dropped_lines = info_summary_msg<3>;
//! Report that input lines were changed
using info_summary_summary_changed_lines = info_summary_msg<4>;
//! Report that input lines were converted to `#errors`
using warning_summary_summary_errored_lines = warning_summary_msg<1>;
//! Report that unconditional `#error` lines were output
using warning_summary_summary_error_output = warning_summary_msg<2>;

#endif // EOF
