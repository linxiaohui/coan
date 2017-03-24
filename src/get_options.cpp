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

#include "get_options.h"
#include <cassert>
#include <cstring>

/** \file get_options.cpp
 * This file implements the `get_options` API
 */

int optind;
int optopt;
char *optarg;

cmd_option const *
get_command_option(int argc, char *argv[], cmd_option const * commands)
{
	if (argc > 1) {
		cmd_option const * command = commands;
		assert(commands);
		for (	; command->name; ++command) {
			if (!strcmp(argv[1],command->name)) {
				return command;
			}
		}
	}
	return nullptr;
}

bool
opts_are_compatible(	int opt_excluder, int opt_excluded,
                        struct exclusion_list const *exclusions, bool indexed)
{
	struct exclusion_list const * which;
	bool verdict = true;
	assert(exclusions);
	if (indexed) {
		which = exclusions + opt_excluder;
	} else {
		which = exclusions;
		for (	; which->excluder; ++which) {
			if (which->excluder == opt_excluder) {
				break;
			}
		}
	}
	if (which->excluder) {
		int const *excluded = which->excluded;
		if (excluded) {
			for (	; *excluded; ++excluded ) {
				if (opt_excluded == *excluded) {
					verdict = false;
				}
			}
		}
	}
	return verdict;
}

int
getopt_long(int argc, char * argv[], const char *optstr,
            const struct option *longopts, int *longind)
{
	char *opt;
	optarg = nullptr;
	if (longind) {
		*longind = -1;
	}
	if (optind < 0) {
		return -1;
	}
	if (optind == 0) {	/* Initial or re-initialising ref*/
		++optind; /* Skip argv[0]*/
	}
	if (optind >= argc) {
		return -1;
	}
	opt = argv[optind];
	if (*opt == '-') {	/* argv[optind] is an option*/
		++optind;
		if (*++opt == ':') {
			optopt = ':';
			return '?';
		}
		if (*opt != '-') { /* Short option*/
			char const *where = strchr(optstr,*opt);
			if (!where) {
				optopt = *opt;
				return '?';
			}
			if (where[1] != ':') { 	/* No argument*/
				if (opt[1]) {
					optopt = *opt;
					return '?';
				}
				optarg = nullptr;
				return *opt;
			} else if (where[2] != ':') { /* Optional argument*/
				if (opt[1] != 0) {	/* arg abuts opt*/
					optarg = opt + 1;
					return *opt;
				} else if (optind >= argc || *argv[optind] == '-') {
					/* No arg*/
					optarg = nullptr;
					return *opt;
				} else {	/* Arg is next argv*/
					optarg = argv[optind++];
					return *opt;
				}
			} else {	/* Required argument*/
				if (opt[1] != 0) {	/* arg abuts opt*/
					optarg = opt + 1;
					return *opt;
				} else if (optind >= argc || *argv[optind] == '-') {
					/* Required arg missing*/
					optopt = *opt;
					return '?';
				} else {	/* arg is next argv*/
					optarg = argv[optind++];
					return *opt;
				}
			}
		} else { /* Long option*/
			int i, match = -1;
			size_t optlen;
			/* Test for `opt=arg' and get length of opt*/
			char *eq = strchr(++opt,'=');
			if (eq) {
				optarg = eq + 1;
				optlen = eq - opt;
			} else {
				optlen = strlen(opt);
			}
			/* Test for exactly one match of the option*/
			for (i = 0; longopts[i].name != nullptr; ++i) {
				if (!strncmp(opt,longopts[i].name,optlen)) {
					if (match != -1) {
						match = -1;
						break;
					}
					match = i;
				}
			}
			if (longind != nullptr) {
				*longind = match;
			}
			if (match == -1) { /* Not found*/
				return '?';
			}
			switch(longopts[match].has_arg) {
			case no_argument:
				if (optarg) {	/* Illegal arg*/
					return '?';
				}
				break;
			case required_argument:
				if (optarg) {	/* Arg after `='*/
					break;
				}
				if (optind >= argc || *argv[optind] == '-') {
					/* Required arg missing*/
					return '?';
				}
				/* Arg is next argv*/
				optarg = argv[optind++];
				break;
			case optional_argument:
				if (optarg) {	/* Arg after `='*/
					break;
				}
				if (optarg || optind >= argc || *argv[optind] == '-') {
					/* No argument*/
					break;
				}
				optarg = argv[optind++];
				break;
			default:
				assert(false);
			}
			/* Store or return value as required*/
			if (longopts[match].flag != nullptr) {
				*(longopts[match].flag) = longopts[i].val;
				return 0;
			} else {
				return longopts[match].val;
			}
		}
	} else {
		/* argv[optind] is a non-option.
				We want to shuffle it to the end*/
		bool done = true;	/* Posit no more options to the right*/
		int i;
		for (i = optind; done && i < argc; ++i) {
			/* Now see if this is true*/
			done = *argv[i] != '-';
		}
		if (done) { /* There are no more options to the right*/
			optarg = argv[optind]; /* optind at start of non-options*/
			return -1; /* All done*/
		} else { /* There are more options to the right*/
			/* Swap the current non-opt to the end of argv*/
			char *temp = argv[optind];
			memmove(argv + optind,argv + optind + 1,
			        (argc - optind - 1) * sizeof(char *));
			argv[argc - 1] = temp;
			/* Try for another option*/
			return getopt_long(argc,argv,optstr,longopts,longind);
		}
	}
}

const char *
get_long_opt_name(struct option const * longopts, int opt)
{
	struct option const * longopt = longopts;
	for (	;	longopt->name; ++longopt) {
		if (longopt->val == opt) {
			return longopt->name;
		}
	}
	return nullptr;
}


/* EOF*/
