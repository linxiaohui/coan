#ifndef GET_OPTIONS_H
#define GET_OPTIONS_H
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

#include "platform.h"

/*! \file get_options.h
 * This file defines the `get_options` API
 */


extern int optind;
///< Index of option parsed by `getopt_long()`
extern int optopt;
///< Unrecogised short option parsed by `getopt_long()`
extern char *optarg;
///< Argument to an option parsed by `getopt_long()`

/// Info structure for an option for `getopt_long()`.
struct option {
	/// The name of the option.
	const char *name;
	/** \brief Can the option take an argument?
	 *  One of:
     *  - `no_argument`
	 *	- `required_argument`
	 *	- `optional argument`
	 */
	int has_arg;

	/** \brief Mode flag.
     *
	 *   `nullptr` here makes `getopt_long()` return the field `val` if
	 *   this option is matched. Non-null makes `getopt_long()` return
	 *   0 if any option is matched, and set the `int` addressed by
	 *   `flag` = `val` if this option is matched, otherwise
	 *   leaving it unchanged.
	 */
	int *flag;
	/**	\brief Value to be returned or stored by `getopt_long()`.
     *
	 *   Value to be returned or stored by `getopt_long()` if this
	 *   option is matched (depending on `flag`).
	 */
	int val;
};

/// Possible values of the `has_arg` field in an `option` structure
enum {
    no_argument, ///< The option cannot have an argument
    required_argument, ///< The option must have an argument
    optional_argument ///< The option may or may not have an argument
};


/// Info structure for a command option for `get_command()`
struct cmd_option {
	/// The name of the option.
	const char *name;
	///	Value to be returned if the command is matched.
	int cmd_code;
};

/** \brief Structure representing a set of options that are excluded
 *	by another option.
 */
struct exclusion_list {
	/// This option code excludes the others.
	int excluder;
	/// This 0-terminated list of option codes is excluded by `excluder`.
	int const * excluded;
};

/** \brief Look for a command option at the start of commandline arguments.
 *	\param	argc	The number of commandline arguments.
 *	\param	argv	The array of commandline arguments.
 *	\param	commands	An array of valid command options.
 *	\return If `argc` < 2 return `nullptr`. Else if `argv[1]` matches the
 *		`name` field in any of the command options listed in `commands`,
 *		return the address of that command option, else return `nullptr`.
 */
extern cmd_option const *
get_command_option(int argc, char *argv[], cmd_option const * commands);

/** \brief Say whether options are compatible with respect to specified
 *	incompatibilities.
 *	\param	opt_excluder	To be determined if this option
 *			excludes another.
 *	\param	opt_excluded	To be determined if this option
 *			is excluded by `opt_excluder`.
 *	\param	exclusions	Addresses an array of exclusions lists,terminated
 *			by an exclusion list in which the `excluder` field is 0,
 *			in which a list belonging to `opt_excluder` will be sought.
 *	\param	indexed  If true then the function may assume that `opt_excluder`
 *			is an index to its exclusion list in `exclusions`.
 *	\return	False if `exclusions` is found to contain an exclusion list
 *			whose `excluder` field is `opt_excluder` and whose
 *			`excluded` field is an array containing `opt_excluded`.
 *			Otherwise true.
 *
 */
extern bool
opts_are_compatible(	int opt_excluder, int opt_excluded,
                        struct exclusion_list const *exclusions, bool indexed);

#ifdef GNUWIN
/* Avoid linkage clash with mingw function */
#define getopt_long getoptions_long
#endif
/** \brief Parse commandline options.
 *
 *  We need the standard `getopt_long()` function, but the host implementation
 *	cannot be relied on to be re-entrant. Therefore we roll our own
 *	re-entrant version. The interface is like that of its standard namesake.
 *
 *	\param argc The number of commandline arguments
 *	\param  argv Array of pointers to the commandline arguments.
 *	\param optstr A string  containing the legitimate option characters.
 *	\param longopts  A pointer to the first element of an array of
 *      `struct option`.
 *   \param longind A pointer at which is stored on return the index of
 *       any long option parsed.
 *   \return The character code of the option detected, if any; '?' if an
 *       unrecognized option is detected; -1 if no more options are detected.
 */
extern int
getopt_long(int argc, char * argv[], const char *optstr,
            struct option const *longopts, int *longind);

/** \brief Look up the long name of the an option in an array of
 *   `struct option`.
 *	\param	longopts	The array of option info to be searched, terminated
 *				by a `struct option` in which the `name` field is `nullptr`.
 *	\param	opt	The option code to be sought.
 *	\return The address of the long name of the option with code `opt`, if
 *		found, else `nullptr`.
 *
 */
extern const char *
get_long_opt_name(struct option const * longopts, int opt);

#endif /* EOF*/
