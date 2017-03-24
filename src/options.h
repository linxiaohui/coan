#ifndef OPTIONS_H
#define OPTIONS_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2004 - 2006 Symbian Software Ltd.                       *
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

#include <string>
#include <vector>

/** \file options.h
 *   This file defines `struct options`
 */

/// Symbolic constants denoting policies for discarding lines
enum discard_policy {
    DISCARD_DROP,	///< Drop discarded lines
    DISCARD_BLANK,	///< Blank discarded lines
    DISCARD_COMMENT ///< Comment discarded lines
};

/** \brief Symbolic constants denoting policies for handling contradictions.
 *
 *   Symbolic constants denoting policies for handling contradictions
 *   between commandline `--define` and `--undef` options and
 *   in-source `#define` and `#undef` directives.
 */
enum contradiction_policy {
    /// Delete a contradicted directive
    CONTRADICTION_DELETE,
    /// Comment out a contradicted directive
    CONTRADICTION_COMMENT,
    /// Replace a contradicted directive with an `#error`
    CONTRADICTION_ERROR
};

/// Sequential symbolic constants for coan commands
enum command_code {
    CMD_HELP = 1,	///< The help command
    CMD_VERSION,	///< The version command
    CMD_SOURCE,	///< The source command
    CMD_SYMBOLS,	///< The symbols command
    CMD_INCLUDES,	///< The includes command
    CMD_DEFS,	///< The defs command
    CMD_PRAGMAS,	//!< The pragmas command
    CMD_ERRORS,	///< The errors command
    CMD_LINES,	///< The line command
    CMD_DIRECTIVES,	///< The directives command
    CMD_SPIN	///< the spin command
};

/// Manages coan's commandline arguments
struct  options {

	/// Get the operative coan command code.
	static command_code get_command();

	/// Get our executable's full pathname, `argv[0]`
	static std::string const & exec_path() {
		return _exec_path_;
	}
	/// Get the program's name.
	static std::string const & prog_name() {
		return _prog_name_;
	}
	/// Get the file backup name suffix.
	static std::string const & backup_suffix() {
		return _backup_suffix_;
	}
	/// Have we got all the options?
	static bool got_opts() {
		return _got_opts_;
	}
	/// Do we replace input files with output files?
	static bool replace() {
		return _replace_;
	}
	/// Do we report file and line numbers for listed items?
	static bool list_location() {
		return _list_locate_;
	}
	/// Do we report only the first occurrence of listed items?
	static bool list_only_once() {
		return _list_only_once_;
	}
	/// Do we report the listed items just once per input file?
	static bool list_once_per_file() {
		return _list_once_per_file_;
	}
	/// Do we list items only from operative directives?
	static bool list_only_active() {
		return _list_only_active_;
	}
	/// Do we list items only from inoperative directives?
	static bool list_only_inactive() {
		return _list_only_inactive_;
	}
	/// Do we list items only from inoperative directives?
	static bool list_symbols_in_ifs() {
		return _list_symbols_in_ifs_;
	}
	/// Do we list symbols in `#define` directives?
	static bool list_symbols_in_defs() {
		return _list_symbols_in_defs_;
	}
	/// Do we list symbols in `#undef` directives?
	static bool list_symbols_in_undefs() {
		return _list_symbols_in_undefs_;
	}
	/// Do we list symbols in `#include` directives?
	static bool list_symbols_in_includes() {
		return _list_symbols_in_includes_;
	}
	/// Do we list symbols in `#line` directives?
	static bool list_symbols_in_lines() {
		return _list_symbols_in_lines_;
	}
	/// Do we list system `#include` directives?
	static bool list_system_includes() {
		return _list_system_includes_;
	}
	/// Do we list local `#include` directives?
	static bool list_local_includes() {
		return _list_local_includes_;
	}
	/// Are we to output lines instead of dropping them and vice versa?
	static bool complement() {
		return _complement_;
	}
	/** \brief Do we evaluate constants in truth-functional contexts or treat
	 *   them as unknowns.
	 */
	static bool eval_wip() {
		return _eval_wip_;
	}
	/// Do we report the expansions reported symbols?
	static bool expand_references() {
		return _expand_references_;
	}
	/// Get the policy for discarding lines.
	static discard_policy & get_discard_policy() {
		return _discard_policy_;
	}
	/// Do we output `#line` directives?
	static bool line_directives() {
		return _line_directives_;
	}
	/// Are we to omit parsing for C/C++ comments?
	static bool plaintext() {
		return _plaintext_;
	}
	/// Do we recurse into directories?
	static bool recurse() {
		return _recurse_;
	}
	/// Shall we continue to process input files after errors?
	static bool keep_going() {
		return	_keepgoing_;
	}
	/// Do we implicitly `--undef` all unconfigured symbols?
	static bool implicit() {
		return	_implicit_;
	}
	/** Do we suppress transient symbol configurations for in-source
	 *	`#define` and `#undef` directives?
	 */
	static bool no_transients() {
		return	_no_transients_;
	}
	/// Do we report the progressive expansion of symbol references?
	static bool explain_references() {
		return	_explain_references_;
	}
	/// Is symbol reporting restricted to a selected set?
	static bool selected_symbols() {
		return	_selected_symbols_;
	}
	/// Is symbol reporting restricted to a selected set?
	static unsigned & max_expansion() {
		return	_max_expansion_;
	}

	/// Say whether the current comment generates source code
	static bool have_source_output() {
		static bool yes =
				get_command() == CMD_SOURCE || get_command() == CMD_SPIN;
		return yes;
	}

	/// Say whether items are reportable at most once per file
	static bool list_at_most_once_per_file() {
		static bool yes =
				_list_only_once_ || _list_once_per_file_;
		return yes;
	}

	/** \brief Parse the full and short names of the executable.
     *
	 *   Element 0 of `argv` is parsed to store the
	 *   pathname and filename of our executable.
     *
	 *   \param argv		The array of commandline arguments.
	 */
	static void parse_executable(char **argv);

	/** \brief Parse the commandline.
     *
	 *  \param argc		The number of commandline arguments.
	 *  \param	argv	Array of commandline arguments.
	 */
	static void parse(int argc, char *argv[]);

	/// Say whether a diagnostic reason code is gagged.
	static bool diagnostic_gagged(unsigned reason);

	/** \brief Analyse the class global state after parsing the commandline.
     *
	 *   Diagnose any errors and draw final conclusions.
	 */
	static void finish();

private:

	/// Symbolic constants for the options of all coan commands
	enum option_codes {
		OPT_FILE = 'f', 		///< The `--file` option
		OPT_REPLACE = 'r', 		///< The `--replace` option
		OPT_BACKUP = 'b', 		///< The `--backup` option
		OPT_DEF = 'D', 			///< The `--define` option
		OPT_UNDEF = 'U', 		///< The `--undefine` option
		OPT_CONFLICT = 'x', 	///< The `--conflict` option
		OPT_GAG = 'g', 			///< The `--gag` option
		OPT_VERBOSE = 'V', 		///< The `--verbose` option
		OPT_COMPLEMENT = 'c', 	///< The `--complement` option
		OPT_EVALWIP = 'E', 	///< The `--eval-wip` option
		OPT_DISCARD = 'k', 		///< The `--discard` option
		OPT_LINE = 1,			///< The `--line` option
		OPT_POD = 'P', 			///< The `--pod` option
		OPT_RECURSE = 'R', 		///< The `--recurse` option
		OPT_FILTER = 'F', 		///< The `--filter` option
		OPT_KEEPGOING = 'K',	///< The `--keepgoing` option
		OPT_IFS = 'i',			///< The `--ifs` option
		OPT_DEFS = 'd',			///< The `--defs` option
		OPT_UNDEFS = 'u',		///< The `--undefs` option
		OPT_INCLUDES = 2,       ///< The `--includes` option
		OPT_LOCATE = 'L',		///< The `--locate` option
		OPT_ONCE = 'o',			///< The `--once-only` option
		OPT_SYSTEM = 's',		///< The `--system` option
		OPT_LOCAL = 'l',		///< The `--local` option
		OPT_ACTIVE = 'A',		///< The `--active` option
		OPT_INACTIVE = 'I',		///< The `--inactive` option
		OPT_EXPAND = 3,			///< The `--expand` option
		OPT_IMPLICIT = 'm',		///< The `--implicit` option
		OPT_DIR = 4,			///< The `--dir` option
		OPT_PREFIX = 'p',		///< The `--prefix` option
		OPT_NO_TRANSIENTS = 5,	///< The `--no-transients` option
		OPT_EXPLAIN = 6,		///< The `--explain` option
		OPT_SELECT = 7,			///< The `--select` option
		OPT_LNS = 8,			///< The `--lns` option
		OPT_EXPAND_MAX = 9,		///< The `--max-expansion` option
		OPT_ONCE_PER_FILE = 10	///< The `--once-per-file` option
	};

	/** \brief Array of structures specifying the valid options for all coan
	 *   commands.
	 */
	static struct option long_options [];

	/** \brief Array in which we will assemble the `shortopts` arguments for
	 *   `getopts_long()`
     *
	 *   We allow 3 bytes per short option, i,e, the short option code
	 *   possibly followed by up to 2 colons.
	 */
	static char opts [];

	/// Array of structures specifying the valid coan commands.
	static struct cmd_option commands [];

	/// Excluded options for the `source` command.
	static int const source_cmd_exclusions [];

	/// Excluded options for the `symbols` command.
	static int const symbols_cmd_exclusions [];

	/// Excluded options for the `includes` command.
	static int const includes_cmd_exclusions[];

	/// Excluded options for the `directives` command.
	static int const directives_cmd_exclusions [];

	/// Excluded options for the `defs` command.
	static int const defs_cmd_exclusions [];

	/// Excluded options for the `pragmas` command.
	static int const pragmas_cmd_exclusions [];

	/// Excluded options for the `errors` command.
	static int const errors_cmd_exclusions[];

	/// Excluded options for the `lines` command.
	static int const lines_cmd_exclusions[];

	/// Excluded options for the `spin` command.
	static int const spin_cmd_exclusions[];

	/** \brief Array of exclusion lists for the coan commands,
	 *   indexed by command code.
	 */
	static struct exclusion_list const cmd_exclusion_lists [];

	/** \brief Assemble the `shortopts` arguments for `getopts_long()`.
     *
	 *  The string is constructed by scanning the `long_options`
	 *  array to avoid the need for manually maintaining agreement
	 *  between that structure and the `shortopts` string..
	 */
	static void make_opts_list();

	/**	\brief Configure diagnostic output.
     *
	 *   Configure diagnostic outputs in response to a `--gag` or `--verbose`
	 *   option.
     *
	 *   \param	arg	Either "verbose" or an argument to the `--gag` option
	 */
	static void
	config_diagnostics(std::string const & arg);

	/** \brief Configure the final state of diagnostic output filtering.
     *
	 *   If no `--gag` or `--verbose` option has been seen,
	 *   default to `--gag info --gag summary`.
	 */
	static void finalise_diagnostics();

	/** \brief Raise a usage error when an option is invalid for the
	 *   active command.
     *
	 *   \param	cmd	 Pointer to details of the active coan command.
	 *   \param	bad_opt The code of the invalid option.
	 */
	static void
	error_invalid_opt(struct cmd_option const *cmd, int bad_opt);


	/** \brief Add files to the input dataset.
     *
	 *  \param path	File or directory to be included in the
	 *      input dataset.
     *
	 *  If `path` is a file that satisfies any `--filter` option
	 *  it is added to the input dataset.
     *
	 *  If `path` is a directory and `--recurse` is in force then
	 *  files recursively beneath it that satisfy any `--filter`
	 *  option are added to the input dataset.
     *
	 *  If `path` is a directory and `--recurse` is not in force
	 *  then the directory is ignored.
	 */
	static void add_files(std::string const & path);

	/**	\brief Parse commandline options from a file.
     *
	 *  Parse commandline options specified with the `--file`
	 *  option.
     *
	 *  \param	argsfile	Name of the file from which to read options.
	 *
	 *  Options are separated by whitespace in the file.
	 */
	static void parse_file(std::string const & argsfile);

	/**	\brief Parse the options to the active coan command.
     *
	 *  \param	argc	The number of arguments to be parsed.
	 *  \param	argv	The array of arguments to be parsed.
	 */
	static void parse_command_args(int argc, char *argv[]);

	/// Say whether progress messages are suppressed.
	static bool progress_gagged();

	/// Pointer to the details of the operative coan command
	static struct cmd_option * _command_;
	/// `argv[0]`
	static std::string _exec_path_;
	/// Filename element of `exec_path`
	static std::string _prog_name_;
	/// Suffix for backup files
	static std::string _backup_suffix_;
	/// Have we got all commandline options?
	static bool	_got_opts_;
	/// Do we replace input files with output files?
	static bool	_replace_;
	/// Do we report file and line numbers for listed items?
	static bool	_list_locate_;
	/// Do we report only the first occurrence of listed items?
	static bool	_list_only_once_;
	/// Do we report only the first occurrence per file of listed items?
	static bool	_list_once_per_file_;
	/// Do we list items only from kept lines?
	static bool	_list_only_active_;
	/// Do list items only from dropped lines?
	static bool	_list_only_inactive_;
	/// Do we list symbols in `#if/else/endif directives?
	static bool	_list_symbols_in_ifs_;
	/// Do we list symbols in `#define` directives?
	static bool	_list_symbols_in_defs_;
	/// Do we list symbols in `#undef` directives?
	static bool	_list_symbols_in_undefs_;
	/// Do we list symbols in `#include` directives?
	static bool	_list_symbols_in_includes_;
	/// Do we list symbols in `#line` directives?
	static bool	_list_symbols_in_lines_;
	/// Do we list system `#include` directives?
	static bool	_list_system_includes_;
	/// Do we list local `#include` directives?
	static bool	_list_local_includes_;
	/// Are to output lines instead of dropping tem and vice versa?
	static bool	_complement_;
	/** \brief Do we evaluate constants in truth-functional contexts or treat
	 *   them as unknowns?
	 */
	static bool	_eval_wip_;
	/// Do we report the expansions of symbol references?
	static bool	_expand_references_;
	/// Policy for discarding lines
	static discard_policy	_discard_policy_;
	/// Do we output `#line` directives?
	static bool	_line_directives_;
	/// Are we to omit parsing for comments?
	static bool	_plaintext_;
	/// Recurse into directories?
	static bool	_recurse_;
	/// Continue to process input files after errors
	static bool	_keepgoing_;
	/// Do we implicitly `--undef` all unconfigured symbols?
	static bool	_implicit_;
	/** Do we suppress transient symbol configurations for in-source
	 *	`#define` and `#undef` directives?
	 */
	static bool _no_transients_;
	/// Do we report the derivation of symbol resolutions?
	static bool _explain_references_;
	/// Is symbol reporting restricted to a selected set?
	static bool _selected_symbols_;
	/// Limit size for reported macro expansions
	static unsigned _max_expansion_;
	/// Bitmask of diagnostic filters
	static int	_diagnostic_filter_;

	/// Array of options read from `--file ARGFILE`
	static std::vector<std::string > _argfile_argv_;
	/// Read whole `ARGFILE` into this storage
	static std::string 	_memfile_;
	/// The number of input files/dirs specified on the commandline.
	static int _cmd_line_files_;

	/// Are we parsing an argsfile ?
	static bool _parsing_file_;
};

#endif // EOF
