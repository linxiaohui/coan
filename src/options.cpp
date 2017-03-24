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

/** \file options.cpp
 *  This file implements `struct options`
 */

#include "options.h"
#include "symbol.h"
#include "get_options.h"
#include "contradiction.h"
#include "diagnostic.h"
#include "io.h"
#include "dataset.h"
#include "line_despatch.h"
#include "help.h"
#include "version.h"
#include <fstream>
#include <iostream>
#include <iterator>

using namespace std;

struct cmd_option * options::_command_ = nullptr;
string options::_exec_path_;
string options::_prog_name_;
string options::_backup_suffix_;
string 	options::_memfile_;
bool	options::_got_opts_ = false;
bool	options::_replace_ = false;
bool	options::_list_locate_ = false;
bool	options::_list_only_once_ = false;
bool 	options::_list_once_per_file_ = false;
bool	options::_list_only_active_ = false ;
bool	options::_list_only_inactive_ = false;
bool	options::_list_symbols_in_ifs_ = false;
bool	options::_list_symbols_in_defs_ = false;
bool	options::_list_symbols_in_undefs_ = false;
bool	options::_list_symbols_in_includes_ = false;
bool	options::_list_symbols_in_lines_ = false;
bool	options::_list_system_includes_ = false;
bool	options::_list_local_includes_ = false;
bool	options::_complement_ = false;
bool	options::_eval_wip_ = false;
bool	options::_expand_references_ = false;
bool	options::_explain_references_ = false;
bool	options::_selected_symbols_ = false;
//! \cond NO_DOXYGEN
enum discard_policy	options::_discard_policy_ = DISCARD_DROP;
//! \endcond
bool	options::_line_directives_ = false;
bool	options::_plaintext_ = false;
bool	options::_recurse_ = false;
bool	options::_keepgoing_ = false;
bool	options::_implicit_ = false;
bool	options::_no_transients_ = false;
int		options::_diagnostic_filter_ = 0;
bool    options::_parsing_file_ = false;
vector<string> options::_argfile_argv_;
int		options::_cmd_line_files_ = 0;
unsigned options::_max_expansion_ = 4096;

struct option options::long_options [] = {
	{ "file", required_argument, nullptr, OPT_FILE },
	{ "replace",no_argument, nullptr, OPT_REPLACE },
	{ "backup",required_argument,nullptr,OPT_BACKUP},
	{ "define", required_argument, nullptr, OPT_DEF },
	{ "undef", required_argument, nullptr, OPT_UNDEF },
	{ "conflict", required_argument, nullptr, OPT_CONFLICT },
	{ "gag", required_argument, nullptr, OPT_GAG },
	{ "verbose", no_argument, nullptr, OPT_VERBOSE },
	{ "complement", no_argument, nullptr, OPT_COMPLEMENT },
	{ "eval-wip", no_argument, nullptr, OPT_EVALWIP },
	{ "discard", required_argument, nullptr, OPT_DISCARD },
	{ "line", no_argument, nullptr, OPT_LINE },
	{ "pod", no_argument, nullptr, OPT_POD },
	{ "recurse", no_argument, nullptr, OPT_RECURSE },
	{ "filter", required_argument, nullptr, OPT_FILTER },
	{ "keepgoing", no_argument, nullptr, OPT_KEEPGOING },
	{ "ifs", no_argument, nullptr, OPT_IFS },
	{ "defs", no_argument, nullptr, OPT_DEFS },
	{ "undefs", no_argument, nullptr, OPT_UNDEFS },
	{ "includes", no_argument, nullptr, OPT_INCLUDES },
	{ "lns", no_argument, nullptr, OPT_LNS },
	{ "locate", no_argument, nullptr, OPT_LOCATE },
	{ "once-only", no_argument, nullptr, OPT_ONCE },
	{ "once-per-file", no_argument, nullptr, OPT_ONCE_PER_FILE },
	{ "system", no_argument, nullptr, OPT_SYSTEM },
	{ "local", no_argument, nullptr, OPT_LOCAL },
	{ "active", no_argument, nullptr, OPT_ACTIVE },
	{ "inactive", no_argument, nullptr, OPT_INACTIVE },
	{ "expand", no_argument, nullptr, OPT_EXPAND },
	{ "implicit", no_argument, nullptr, OPT_IMPLICIT },
	{ "explain", no_argument, nullptr, OPT_EXPLAIN },
	{ "no-transients", no_argument, nullptr, OPT_NO_TRANSIENTS },
	{ "dir", required_argument, nullptr, OPT_DIR },
	{ "prefix", required_argument, nullptr, OPT_PREFIX },
	{ "select", required_argument, nullptr, OPT_SELECT },
	{ "max-expansion", required_argument, nullptr, OPT_EXPAND_MAX },
	{ 0, 0, 0, 0 }
};

char options::opts[(sizeof(long_options)/sizeof(struct option)) * 3];


struct cmd_option options::commands [] = {
	{ "help", CMD_HELP },
	{ "-h", CMD_HELP },
	{ "--help", CMD_HELP },
	{ "version", CMD_VERSION },
	{ "-v", CMD_VERSION },
	{ "--version", CMD_VERSION },
	{ "source", CMD_SOURCE },
	{ "symbols", CMD_SYMBOLS },
	{ "includes", CMD_INCLUDES },
	{ "defs", CMD_DEFS },
	{ "pragmas", CMD_PRAGMAS },
	{ "errors", CMD_ERRORS },
	{ "lines", CMD_LINES },
	{ "directives", CMD_DIRECTIVES },
	{ "spin", CMD_SPIN },
	{ 0, 0 }
};

int const options::source_cmd_exclusions[] = {
	OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES, OPT_LOCATE,
	OPT_ONCE, OPT_SYSTEM, OPT_LOCAL, OPT_ACTIVE, OPT_INACTIVE,
	OPT_EXPAND, OPT_PREFIX, OPT_EXPLAIN, OPT_SELECT, OPT_LNS,
	OPT_ONCE_PER_FILE,0
};

int const options::symbols_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_SYSTEM,
	OPT_LOCAL, OPT_BACKUP, OPT_COMPLEMENT, OPT_DIR, OPT_PREFIX, 0
};

int const options::includes_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES, OPT_COMPLEMENT,
	OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN, OPT_SELECT,
	OPT_LNS, 0
};

int const options::directives_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES, OPT_COMPLEMENT,
	OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN, OPT_SELECT,
	OPT_LNS, 0
};

int const options::defs_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_SYSTEM, OPT_LOCAL, OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES,
	OPT_COMPLEMENT, OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN,
	OPT_SELECT, OPT_LNS, 0
};

int const options::pragmas_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_SYSTEM, OPT_LOCAL, OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES,
	OPT_COMPLEMENT, OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN,
	OPT_SELECT, OPT_LNS, 0
};

int const options::errors_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_SYSTEM, OPT_LOCAL, OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES,
	OPT_COMPLEMENT, OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN,
	OPT_SELECT, OPT_LNS, 0
};

int const options::lines_cmd_exclusions[] = {
	OPT_REPLACE, OPT_CONFLICT, OPT_DISCARD, OPT_LINE, OPT_BACKUP,
	OPT_SYSTEM, OPT_LOCAL, OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES,
	OPT_COMPLEMENT, OPT_EXPAND, OPT_DIR, OPT_PREFIX, OPT_EXPLAIN,
	OPT_SELECT, OPT_LNS, 0
};

int const options::spin_cmd_exclusions[] = {
	OPT_IFS, OPT_DEFS, OPT_UNDEFS, OPT_INCLUDES, OPT_LOCATE,
	OPT_ONCE, OPT_SYSTEM, OPT_LOCAL, OPT_ACTIVE, OPT_INACTIVE,
	OPT_BACKUP, OPT_EXPAND, OPT_EXPLAIN, OPT_SELECT, OPT_LNS,
	OPT_ONCE_PER_FILE, 0
};

struct exclusion_list const options::cmd_exclusion_lists[] = {
	/* Dummy 0th element*/
	{	0, nullptr },
	/* Exclusion list for the help command*/
	{	CMD_HELP, nullptr },
	/* Exclusion list the version command*/
	{	CMD_VERSION, nullptr },
	/* Exclusion list the code command*/
	{ 	CMD_SOURCE, source_cmd_exclusions},
	/* Exclusion list for the symbols command*/
	{ 	CMD_SYMBOLS, symbols_cmd_exclusions },
	/* Exclusion list for the includes command*/
	{ 	CMD_INCLUDES, includes_cmd_exclusions },
	/* Exclusion list for the defs command*/
	{ 	CMD_DEFS, defs_cmd_exclusions },
	/* Exclusion list for the pragmas command*/
	{ 	CMD_PRAGMAS, pragmas_cmd_exclusions },
	/* Exclusion list for the lines command*/
	{ 	CMD_LINES, lines_cmd_exclusions },
	/* Exclusion list for the pragmas command*/
	{ 	CMD_ERRORS, errors_cmd_exclusions },
	/* Exclusion list for the directives command*/
	{ 	CMD_DIRECTIVES, directives_cmd_exclusions },
	{ 0, nullptr }
};

command_code options::get_command()
{
	return command_code(_command_->cmd_code);
}

void options::make_opts_list()
{
	struct option * longopt = long_options;
	char *opts_str = opts;
	for (	; longopt->name; ++longopt, ++opts_str) {
		*opts_str = longopt->val;
		switch(longopt->has_arg) {
		case no_argument:
			break;
		case optional_argument:
			++opts_str;
			*opts_str = ':';
			/* Yes, fall through */
		case required_argument:
			++opts_str;
			*opts_str = ':';
			break;
		default:
			assert(0);
		}
	}
}

void options::config_diagnostics(string const & arg)
{
	severity mask = severity::none;
	if (_diagnostic_filter_ < 0 && arg != "verbose") {
		warning_verbose_only warn;
		warn << "Can't mix --verbose with --gag.'--gag " << arg << " ignored";
		cerr << warn.text() << '\n';
		return;
	}
	if (arg == "progress") {
		mask = severity::progress;
	} else if (arg == "info") {
		mask = severity::info;
	} else if (arg == "warning") {
		mask = severity::warning;
	} else if (arg == "error") {
		mask = severity::error;
	} else if (arg == "abort") {
		mask = severity::abend;
	} else if (arg == "summary") {
		mask = severity::summary;
	} else if (arg == "verbose") {
		if (_diagnostic_filter_ > 0) {
			warning_verbose_only() <<
			   "Can't mix --verbose with --gag. '--verbose' ignored" << emit();
			return;
		} else if (_diagnostic_filter_ < 0) {
			info_duplicate_mask() << "'--verbose' already seen" << emit();
			return;
		}
		_diagnostic_filter_ = -1;
		return;
	} else {
		error_usage() << "Invalid argument for --gag: \""
		              << arg << '\"' << emit();
	}
	if (int(mask) & _diagnostic_filter_) {
		info_duplicate_mask() << "'--gag " << arg << "' already seen" << emit();
	}
	_diagnostic_filter_ |=  int(mask);
	if (mask == severity::info) {
		config_diagnostics("progress");
	} else if (mask == severity::warning) {
		config_diagnostics("info");
	} else if (mask == severity::error) {
		config_diagnostics("warning");
	} else if (mask == severity::abend) {
		config_diagnostics("error");
	}
}

void options::finalise_diagnostics()
{
	if (!_diagnostic_filter_) {
		/* Default diagnostic masking to no progress
			messages, no infos, no summaries*/
		config_diagnostics("info");
		config_diagnostics("summary");
	} else if (_diagnostic_filter_ == -1) {
		/* --verbose was temporarily set as -1 to block later
			--gag options. Can now reset as 0*/
		_diagnostic_filter_ = 0;
	}
}

void options::error_invalid_opt(cmd_option const *cmd, int bad_opt)
{
	char const *cmdname = cmd->name;
	char const *optname = get_long_opt_name(long_options,bad_opt);
	assert(optname);
	error_usage() << '\"' << optname << '\"'
		<< " is an invalid or ambiguous option for command \""
		<< cmdname << '\"' << emit();
}

void options::add_files(string const & path)
{
	fs::obj_type_t obj_type = fs::obj_type(path);
	if (obj_type == fs::OBJ_NONE) {
		abend_no_file() <<
			"No such file or directory as \"" << path << '\"' << emit();
	} else if (fs::is_file(obj_type)) {
		dataset::add(path);
	} else if (fs::is_dir(obj_type)) {
		if (!_recurse_ && !io::spin()) {
			warning_dir_ignored() <<
				  "--recurse not specified. Ignoring directory \"" << path <<
				  '\"' << emit();
		} else {
			if (io::spin()) {
				static path_t spin_dir_path(io::spin_dir());
				path_t new_path(path);
				path_t prefix = path_t::common_prefix(new_path,spin_dir_path);
				if (prefix == spin_dir_path || prefix == new_path) {
					abend_invalid_spin_dir() << "The spin directory "
						 "cannot include or be included by or identical with "
						 "any input directory" << emit();
				}
			}
			dataset::add(path);
		}
	} else if (fs::is_slink(obj_type)) {
		warning_broken_symlink() <<	"Broken symbolic link \"" << path <<
		                         "\" ignored" << emit();
	}
}

void options::parse_command_args(int argc, char *argv[])
{
	int options = argc;
	int opt, save_ind, long_index;
	if (!opts[0]) {
		make_opts_list();
	}
	for ( optind = 0;
	      (opt = getopt_long(argc,argv,opts,long_options,&long_index))
	      != -1; ) {
		if (!opts_are_compatible(_command_->cmd_code,
		                         opt,cmd_exclusion_lists,true)) {
			error_invalid_opt(_command_,opt);
		}
		switch (opt) {
		case OPT_FILE:	/* Read further options from file*/
			save_ind = optind;
			/* Remember where we have parsed up to*/
			_parsing_file_ = true;
			parse_file(optarg); /* Parse file*/
			_parsing_file_ = false;
			optind = save_ind; /* Restore position*/
			break;
		case OPT_CONFLICT: {	/* Policy for contradictions*/
			contradiction_policy conflict_policy = CONTRADICTION_COMMENT;
			string option_arg(optarg);
			if (option_arg.length() > 1) {
				if (option_arg == "delete") {
					conflict_policy = CONTRADICTION_DELETE;
				} else if (option_arg == "comment") {
					conflict_policy = CONTRADICTION_COMMENT;
				} else if (option_arg == "error") {
					conflict_policy = CONTRADICTION_ERROR;
				} else {
					error_usage() << "Invalid argument for --conflict: \""
					              << option_arg << '\"' << emit();
				}
			} else {
				switch(option_arg[0]) {
				case 'd':
					conflict_policy = CONTRADICTION_DELETE;
					break;
				case 'c':
					conflict_policy = CONTRADICTION_COMMENT;
					break;
				case 'e':
					conflict_policy = CONTRADICTION_ERROR;
					break;
				default:
					error_usage() << "Invalid argument for -x: \""
					              << option_arg[0] << '\"' << emit();
				}
			}
			contradiction::set_contradiction_policy(conflict_policy);
		}
		break;
		case OPT_GAG: {
			string option_arg(optarg);
			if (option_arg.length() == 1) {
				switch(option_arg[0]) {
				case 'p':
					option_arg = "progress";
					break;
				case 'i':
					option_arg = "info";
					break;
				case 'w':
					option_arg = "warning";
					break;
				case 'e':
					option_arg = "error";
					break;
				case 'a':
					option_arg = "abend";
					break;
				case 's':
					option_arg = "summary";
					break;
				default:
					assert(false);
				}
			}
			config_diagnostics(option_arg);
		}
		break;
		case OPT_VERBOSE:
			config_diagnostics("verbose");
			break;
		case OPT_DEF: { /* define a symbol*/
            string s(optarg);
			chewer<string> chew(false,s);
			symbol::locator sloc(chew);
			sloc->digest_global_define(chew);
		}
		break;
		case OPT_UNDEF: { /* undef a symbol*/
            string s(optarg);
			chewer<string> chew(false,s);
			symbol::locator sloc(chew);
			sloc->digest_global_undef(chew);
		}
		break;
		case OPT_COMPLEMENT: /* treat -D as -U and vice versa*/
			_complement_ = true;
			break;
		case OPT_REPLACE:
			_replace_ = true;
			break;
		case OPT_BACKUP:
			_backup_suffix_ = optarg;
			break;
		case OPT_EVALWIP:
			_eval_wip_ = true;
			break;
		case OPT_DISCARD: { /* policy for discarding lines on output*/
			string option_arg(optarg);
			if (option_arg.length() > 1) {
				if (option_arg == "drop") {
					_discard_policy_ = DISCARD_DROP;
				} else if (option_arg == "blank") {
					_discard_policy_ = DISCARD_BLANK;
				} else if (option_arg == "comment") {
					_discard_policy_ = DISCARD_COMMENT;
				} else {
					error_usage() << "Invalid argument for --discard: \""
					              << optarg << '\"' << emit();
				}
			} else {
				switch(option_arg[0]) {
				case 'd':
					_discard_policy_ = DISCARD_DROP;
					break;
				case 'b':
					_discard_policy_ = DISCARD_BLANK;
					break;
				case 'c':
					_discard_policy_ = DISCARD_COMMENT;
					break;
				default:
					error_usage() << "Invalid argument for -k: \""
					              << *optarg << '\"' << emit();
				}
			}
		}
		break;
		case OPT_LINE:
			_line_directives_ = true;
			break;
		case OPT_LOCATE:
			_list_locate_ = true;
			break;
		case OPT_ACTIVE:
			_list_only_active_ = true;
			break;
		case OPT_INACTIVE:
			_list_only_inactive_ = true;
			break;
		case OPT_EXPAND:
			_expand_references_ = true;
			break;
		case OPT_ONCE:
			_list_only_once_ = true;
			break;
		case OPT_ONCE_PER_FILE:
			_list_once_per_file_ = true;
			break;
		case OPT_IFS:
			_list_symbols_in_ifs_ = true;
			break;
		case OPT_DEFS:
			_list_symbols_in_defs_ = true;
			break;
		case OPT_UNDEFS:
			_list_symbols_in_undefs_ = true;
			break;
		case OPT_INCLUDES:
			_list_symbols_in_includes_ = true;
			break;
		case OPT_LNS:
			_list_symbols_in_lines_ = true;
			break;
		case OPT_SYSTEM:
			_list_system_includes_ = true;
			break;
		case OPT_LOCAL:
			_list_local_includes_ = true;
			break;
		case OPT_POD: /* don't parse quotes or comments*/
			_plaintext_ = true;
			break;
		case OPT_RECURSE: /* recurse into directories*/
			_recurse_ = true;
			if (_command_->cmd_code == CMD_SOURCE ||
			    _command_->cmd_code == CMD_SPIN) {
				_replace_ = true;
			}
			break;
		case OPT_FILTER: /* Filter input by file extensions*/
			dataset::set_filter(optarg);
			break;
		case OPT_KEEPGOING: /* Continue to process subsequent in
								put files after errors*/
			_keepgoing_ = true;
			break;
		case OPT_IMPLICIT: /* Implicitly --undef any unconfigured symbol */
			_implicit_ = true;
			break;
		case OPT_NO_TRANSIENTS: /* Disallow transient symbol configurations
					for in-source #defines and #undefs */
			_no_transients_ = true;
			warning_no_transients_used() << "The --no-transients option "
				<< "prohibits coan from taking account of the effects of "
				<< "in-source #define and #undef directives. "
				<< "Use at your own risk." << emit();
			break;
		case OPT_DIR: /* Specify the spin directory */
			io::set_spin_dir(optarg);
			break;
		case OPT_PREFIX:
			/* Specify the path prefix assumed to match the spin directory */
			io::set_spin_prefix(optarg);
			break;
		case OPT_EXPLAIN:
			/* Specify whether the progressive expansion of symbol
				references is to be reported. Implies --expand
			*/
			_expand_references_ = _explain_references_ = true;
			break;
		case OPT_SELECT:
			/* Specify a list of symbols to be reported.
			*/
			symbol::set_selection(optarg);
			_selected_symbols_ = true;
			break;
		case OPT_EXPAND_MAX: {
			/* Specify the limit size for reported macro expansions
			*/
			char *endp;
			_max_expansion_ = strtoul(optarg,&endp,10);
			if (*endp && (*endp == 'K' || *endp == 'k')) {
				_max_expansion_ *= 1024;
				++endp;
			}
			if (*endp) {
				error_usage() << "Invalid argument for --max-expansion: \""
					<< optarg << '\"' << emit();
			}
		}
		break;
		default:
			error_usage() <<
				"Invalid or ambiguous option: \"" << argv[optind - 1] << '\"'
			              << emit();
		}
	}
	if (!_parsing_file_) {
		_got_opts_ = true;
		finalise_diagnostics();
		if (!progress_gagged()) {
			progress_got_options msg;
			msg << "Args: ";
			for (int i = 0; i < options; ++i) {
				msg << argv[i] << ' ';
			}
			msg << emit();
		}
		if (_command_->cmd_code == CMD_SOURCE ||
			_command_->cmd_code == CMD_SPIN) {
			if (! options::implicit() &&
					symbol::count(symbol::provenance::global) == 0) {
				warning_no_syms() <<
					"You have not --define-ed or --undef-ed any symbols. "
					"Only in-source #define/#undef directives will have "
					"any effects" << emit();
			}
		}
	}
	if (argc) {
		progress_building_tree() << "Building input tree" << emit();
	}
	argc -= optind;
	argv += optind;
	for (	; argc; --argc,++argv,++_cmd_line_files_) {
		add_files(*argv);
	}
}

void options::parse_file(string const & argsfile)
{
	std::vector<char *> arg_addrs;
	if (_argfile_argv_.size()) {
		error_multiple_argfiles() << "--file can only be used once" << emit();
	}
	ifstream in(argsfile.c_str());
	if (!in.good()) {
		abend_cant_open_input() << "Can't open " <<
		                        argsfile << " for reading" << emit();

	}
	_argfile_argv_.push_back(_prog_name_);
	copy(istream_iterator<string>(in),
	     istream_iterator<string>(),
	     back_inserter<vector<string> >(_argfile_argv_));
	in.close();
	vector<string>::iterator iter = _argfile_argv_.begin();
	vector<string>::iterator end = _argfile_argv_.end();
	for(    ; iter != end; ++iter ) {
		arg_addrs.push_back(const_cast<char *>(iter->c_str()));
	}
	parse_command_args(int(arg_addrs.size()),&arg_addrs[0]);
}

void options::parse_executable(char **argv)
{
	_exec_path_ = string(*argv);
	size_t last_slash = _exec_path_.find_last_of(PATH_DELIM);
	if (last_slash == string::npos) {
		_prog_name_ = _exec_path_;
	} else {
		_prog_name_ = _exec_path_.substr(last_slash + 1);
	}
}

void options::parse(int argc, char *argv[])
{
	line_despatch::top();
	cmd_option const *cmd = get_command_option(argc,argv,commands);
	if (cmd) {
		switch(cmd->cmd_code) {
		case CMD_HELP:
			help();
			break;
		case CMD_VERSION:
			version();
			break;
		default:
			_command_ = const_cast<cmd_option *>(cmd);
			argc -= 1;
			argv += 1;
			parse_command_args(argc,argv);
		}
	} else if (argc < 2) {
		error_usage() << "No coan command given" << emit();
	} else {
		error_usage() << '\"' << argv[1]
		              << "\" is not a coan command" << emit();
	}
}

void options::finish()
{
	int cmd_code = _command_->cmd_code;
	bool input_is_stdin = false;

	if (_list_only_active_ && _list_only_inactive_) {
		error_usage() << "--active is inconsistent with --inactive" << emit();
	}
	if (_list_only_once_ && _list_once_per_file_) {
		error_usage()
			<< "--once-only is inconsistent with --once-per-file" << emit();
	}
	if (_line_directives_ && _discard_policy_ != DISCARD_DROP) {
		error_usage() << "--line is inconsistent with --discard blank|comment"
			<< emit();
	}
	if (_cmd_line_files_ == 0) {
		/* No input files on command line*/
		if (!_replace_) {
			/* Without --replace, stdin is the input file*/
			input_is_stdin = true;
		} else {
			/* With --replace, stdin supplies input filenames*/
			string infile(io::read_filename());
			for (   ; infile.length(); infile = io::read_filename()) {
				(void)add_files(infile);
			}
		}
	}
	if (dataset::files() == 0 && !input_is_stdin) {
		error_nothing_to_do() <<
			  "Nothing to do. No input files selected." << emit();
	}
	if (cmd_code == CMD_SOURCE && !input_is_stdin &&
	    dataset::files() > 1 && !_replace_) {
		error_one_file_only() <<
			  "The \"source\" command needs --replace to process multiple files"
			  << emit();
	}
	if (cmd_code == CMD_SYMBOLS) {
		if (!_list_symbols_in_ifs_ &&
		    !_list_symbols_in_defs_ &&
		    !_list_symbols_in_undefs_ &&
		    !_list_symbols_in_includes_) {
			/* No restriction on listed symbols implies list all*/
			_list_symbols_in_ifs_ = _list_symbols_in_defs_ =
				_list_symbols_in_undefs_ = _list_symbols_in_includes_ = true;
		}
	}
	if (cmd_code == CMD_INCLUDES) {
		if (!_list_system_includes_ && !_list_local_includes_) {
			/* No restriction on listed #includes implies list all*/
			_list_system_includes_ = _list_local_includes_ = true;
		}
	}
	progress_file_tracker() <<
		dataset::files() << " files to process" << emit();

}

bool options::progress_gagged()
{
	return (_diagnostic_filter_ & int(severity::progress)) != 0;
}

bool options::diagnostic_gagged(unsigned reason)
{
	return options::_got_opts_ && reason &&
	       options::_diagnostic_filter_ &&
	       (((reason >> 8) & options::_diagnostic_filter_) != 0);
}

/* EOF*/
