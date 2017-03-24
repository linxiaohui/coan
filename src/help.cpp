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

#include "help.h"
#include "options.h"
#include <iostream>
#include <cstdlib>

/** \file help.cpp
 *	This file implements free functions:-
 *	- `help`
 *	- `usage`
 */

using namespace std;

void help()
{
	cout << usage() << '\n';
	exit(EXIT_SUCCESS);
}

string usage()
{
	return  string("usage:\n")
	        + options::prog_name() + " COMMAND [OPTION... [FILE...]]\n"
	        "1. " + options::prog_name() + " { help | --help | -h }\n"
	        "\tDisplay this message and exit.\n"
	        "2. " + options::prog_name() + " { version | --version | -v }\n"
	        "\tDisplay version information and exit.\n"
	        "3. " + options::prog_name() + " source OPTION... [FILE...]\n"
	        "\tSimplify source FILEs in accordance with OPTIONs, "
	        "parsing in the manner of the C preprocessor.\n"
	        "\tInput lines may be dropped from output. Truth functional "
	        "directives may be simplified on output.\n"
	        "4. " + options::prog_name() + " spin OPTION... [FILE...]\n"
	        "\tLike the source command, with all simplified source files "
	        "output beneath a specified directory, mirroring their original "
	        "structure.\n"
	        "5. " + options::prog_name() + " symbols OPTION... [FILE...]\n"
	        "\tList preprocessor symbols from FILEs in accordance with "
	        "OPTIONs, parsing in the manner of the C preprocessor.\n"
	        "6. " + options::prog_name() + " includes OPTION... [FILE...]\n"
	        "\tList #includes fom FILEs in accordance with OPTIONs, parsing in "
	        "the manner of the C preprocessor\n"
	        "7. " + options::prog_name() + " defs OPTION... [FILE...]\n"
	        "\tList #defines and #undefs fom FILEs in accordance with OPTIONs, "
	        "parsing in the manner of the C preprocessor.\n"
	        "8. " + options::prog_name() + " pragmas OPTION... [FILE...]\n"
	        "\tList #pragmas fom FILEs in accordance with OPTIONs, parsing in "
	        "the manner of the C preprocessor\n"
	        "9. " + options::prog_name() + " lines OPTION... [FILE...]\n"
	        "\tList #lines fom FILEs in accordance with OPTIONs, parsing in "
	        "the manner of the C preprocessor\n"
	        "10. " + options::prog_name() + " directives OPTION... [FILE...]\n"
	        "\tList #-directives fom FILEs in accordance with OPTIONs, "
	        "parsing in the manner of the C preprocessor. "
	        "(Directives are #includes, #defines, #undefs, "
	        "#pragmas, #errors, #lines).\n"

	        "General OPTIONS:\n"
	        "\t-fARGFILE, --file ARGFILE\n"
	        "\t\tRead other arguments from ARGFILE.\n"
	        "\t-R, --recurse\n"
	        "\t\tRecurse into directories to find input files.\n"
	        "\t-FEXT1[,EXT2...]\n"
	        "\t--filter EXT1[,EXT2...]\n"
	        "\t\tProcess only input files that have one of the file extensions "
	        "EXT1,EXT2...\n"
	        "\t-DSYM[=VAL], --define SYM[=VAL]\n"
	        "\t\tAssume symbol SYM to be defined [=VAL]\n"
	        "\t-USYM, --undef SYM\n"
	        "\t\tAssume symbol SYM to be undefined.\n"
	        "\t-m, --implicit\n"
	        "\t\tAssume that any symbol that is not --define-ed is implicitly "
	        "--undef-ed.\n"
	        "\t-g{p|i|w|e|a},\n"
	        "\t--gag { progress| info | warning | error | abend }\n"
	        "\t\tSuppress diagnostics no worse than "
	        "{progress | info | warning | error | abend}.\n"
	        "\t-gs, --gag summary\n"
	        "\t\tSuppress summary diagnostics at exit.\n"
	        "\t\t(Default: -gp -gi -gs)\n"
	        "\t-V, --verbose\n"
	        "\t\tOutput all diagnostics.\n"
	        "\t-K, --keepgoing\n"
	        "\t\tIf a parse error is encountered in an input file, continue "
	        "processing subsequent input files.\n"
			"\t--no-transients\n"
			"\t\tBy default an in-source #define SYM or #undef SYM directive "
			"is transiently treated as a -DSYM or -USYM option within the "
			"source file where it is found. This option suppresses the "
			"default behaviour at your own risk.\n"
	        "\t-P, --pod\n"
	        "\t\tApart from #-directives, input is Plain Old Data.\n"
	        "\t\tComments and quotations will not be parsed.\n"
	        "\t-E, --eval-wip\n"
	        "\t\tEvaluate constants in the \"work-in-progress\" directives "
			"\"#if 0\",\"#if 1\". (Default: do not evaluate.)\n"
			"\t--max-expansion N[K]\n"
			"\t\tLimit the length of reported macro-expansions "
			"to N[kilobytes]\n"

	        "source OPTIONs:-\n"
	        "\t-r, --replace\n"
	        "\t\tOverwrite input file with output file. Implied by --recurse\n"
	        "\t\tWith -r, stdin supplies the input *filenames*.\n"
	        "\t\tOtherwise stdin supplies an input *file*; "
	        "the output file is cout.\n"
	        "\t-bSUFFIX, --backup SUFFIX\n"
	        "\t\tBackup each input file by appending SUFFIX to the name.\n"
	        "\t\tApplies only with -r.\n"
	        "\t-kd, --discard drop\n"
	        "\t\tDrop discarded lines from output.\n"
	        "\t-kb, --discard blank\n"
	        "\t\tBlank discarded lines on output.\n"
	        "\t-kc, --discard comment\n"
	        "\t\tComment out discarded lines on output.\n"
	        "\t-xd, --conflict delete\n"
	        "\t\tDelete #defines and #undefs that contradict -D or "
	        "-U options.\n"
	        "\t-xc, --conflict comment\n"
	        "\t\tInsert diagnostic comments on contradictions as per -xd. "
	        "(Default.)\n"
	        "\t-xe, --conflict error\n"
	        "\t\tInsert diagnostic #errors on contradictions as per -xd.\n"
	        "\t--line\n"
	        "\t\tGenerate #line directives to make CPP line-numbering of output"
	        " agree with input even of lines are dropped.\n"
	        "\t-c, --complement\n"
	        "\t\tComplement. Retain the lines that ought to be dropped and "
	        "vice versa.\n"
	        "\t\t(Retained #-directives will still be simplified where "
	        "possible.)\n"

	        "spin OPTIONs:-\n"
	        "\tThe --recurse and --replace options are implied for this "
	        "command.\n"
	        "\t-kd, --discard drop\n"
	        "\t\tDrop discarded lines from output.\n"
	        "\t-kb, --discard blank\n"
	        "\t\tBlank discarded lines on output.\n"
	        "\t-kc, --discard comment\n"
	        "\t\tComment out discarded lines on output.\n"
	        "\t-xd, --conflict delete\n"
	        "\t\tDelete #defines and #undefs that contradict -D or "
	        "-U options.\n"
	        "\t-xc, --conflict comment\n"
	        "\t\tInsert diagnostic comments on contradictions as per -xd. "
	        "(Default.)\n"
	        "\t-xe, --conflict error\n"
	        "\t\tInsert diagnostic #errors on contradictions as per -xd.\n"
	        "\t--line\n"
	        "\t\tGenerate #line directives to make CPP line-numbering of output"
	        " agree with input even of lines are dropped.\n"
	        "\t-c, --complement\n"
	        "\t\tComplement. Retain the lines that ought to be dropped and "
	        "vice versa.\n"
	        "\t\t(Retained #-directives will still be simplified where "
	        "possible.)\n"
	        "\t--dir DIRNAME\n"
	        "\t\tGenerate a spin under directory DIRNAME, which will be "
	        " created if required. Every simplified source file will be output "
	        "to a file with the same name, relative to DIRNAME.\n"
	        "\t-pPATHNAME, --prefix PATHNAME\n"
	        "\t\tIn organizing output files beneath the directory DIRNAME, "
	        "DIRNAME will be equated to the path prefix PATHNAME of any "
	        "input file.\n"

	        "symbols OPTIONs:-\n"
	        "\t-i, --ifs\n"
	        "\t\tList symbols that occur #if[[n]def]/#elif directives.\n"
	        "\t-d, --defs\n"
	        "\t\tList symbols that occur in #define directives.\n"
	        "\t-u, --undefs\n"
	        "\t\tList symbols that occur in #undef directives.\n"
	        "\t--includes\n"
	        "\t\tList symbols that occur in #include directives.\n"
	        "\t--lns\n"
	        "\t\tList symbols that occur in #lines directives.\n"
	        "\t(Default: -i -d -u --includes --lns)\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of of each distinct "
			"reference a symbol\n"
	        "\t--once-per_file\n"
	        "\t\tList only the first occurrence per file of of each distinct "
			"reference of a symbol\n"
	        "\t-A, --active\n"
	        "\t\tList only symbols from operative directives\n"
	        "\t-I, --inactive\n"
	        "\t\tList only symbols from inoperative directives\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"
	        "\t-e, --expand\n"
	        "\t\tReport the expansion of each symbol reference and "
			 "its resolved value, if any\n"
	        "\t--explain\n"
	        "\t\tReport the progressive expansion of each symbol "
			"reference and its resolved value, if any. "
			"Implies --expand.\n"
	        "\t--select=NAME1[*][,NAME2[*]...]\n"
	        "\t\tReport only on symbols that match one of the optionally "
			"wildcard-terminated names\n"

	        "includes OPTIONs:-\n"
	        "\t-s, --system\n"
	        "\t\tList system headers, i.e. <header.h>\n"
	        "\t-L, --local\n"
	        "\t\tList local headers, i.e. \"header.h\"\n"
	        "\t(Default: -s -L)\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each header. "
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each header\n"
	        "\t-A --active\n"
	        "\t\tList only headers from operative directives.\n"
	        "\t-I --inactive\n"
	        "\t\tList only headers from inoperative directives.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"

	        "defs OPTIONs:-\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each distinct #define or "
	        "#undef directive\n"
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each distinct "
			"#define or #undef directive\n"
	        "\t-A --active\n"
	        "\t\tList only operative #define and #undef directives.\n"
	        "\t-I --inactive\n"
	        "\t\tList only inoperative #define and #undef directives.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"

	        "pragmas OPTIONs:-\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each distinct pragma\n"
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each distinct "
			"pragma\n"
	        "\t-A, --active\n"
	        "\t\tList only pragmas from lines that are retained.\n"
	        "\t-I, --inactive\n"
	        "\t\tList only pragmas from lines that are dropped.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"

	        "errors OPTIONs:-\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each #error directive\n"
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each "
			"#error directive\n"
	        "\t-A, --active\n"
	        "\t\tList only operative #error directives.\n"
	        "\t-I, --inactive\n"
	        "\t\tList only inoperative #error directives.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"

	        "lines OPTIONs:-\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each #line directive\n"
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each "
			"#line directive\n"
	        "\t-A, --active\n"
	        "\t\tList only operative #line directives.\n"
	        "\t-I, --inactive\n"
	        "\t\tList only inoperative #line directives.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.\n"

	        "directives OPTIONs:-\n"
	        "\t-o, --once-only\n"
	        "\t\tList only the first occurrence of each distinct directive\n"
	        "\t--once-per-file\n"
	        "\t\tList only the first occurrence per file of each distinct "
			"directive\n"
	        "\t-A, --active\n"
	        "\t\tList only operative directives.\n"
	        "\t-I, --inactive\n"
	        "\t\tList only inoperative directives.\n"
	        "\t-L, --locate\n"
	        "\t\tReport the source file and line number of each listed "
	        "occurrence.";
}

// EOF

