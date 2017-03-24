=pod

=begin html

<table width="100%"><tr><td align="left">COAN(1)</td><td align="center">User Commands</td><td align="right">COAN(1)</td></tr></table>

=end html

=head1 NAME

coan - C/C++ configuration analyser

=head1 SYNOPSIS

coan COMMAND [OPTION...] [I<file>...] [I<directory>...]

Where no files or directories are specified, input will be read from the standard 
input. The output of a command is written either to the standard output or to 
new files, depending upon the command and the options.

=head1 DESCRIPTION

Coan is a tool for investigating configurations of C or C++ source code. A 
I<configuration> is a (possibly empty) list of assumptions about the status of 
some preprocessor symbols when the source code is compiled. It may be assumed 
that a symbol is defined for the preprocessor, possibly with formal parameters and
possibly with a specified definition, or it may be assumed that a symbol is 
undefined. 

It is important to understand that coan shall consider a symbol to be 
I<undefined> only if there is an assumption to that effect, or if you use the 
B<--implicit> option.  By default, a symbol that is not mentioned in any 
assumption is considered to be I<undetermined>.

A configuration may be thought of as list of B<#define> and/or B<#undef> directives
that are applied to every file in the source code to observe their effects. 
The source files might themselves contain B<#define> or B<#undef>
directives that are active under the given configuration and which may influence
its effects. To capture that influence, Coan temporarily adds each active 
B<#define> or B<#undef> directive to the configuration just for the duration of 
the source file in which it is found.   

Given a configuration and some source code, coan can answer a range of questions
about how the source code would appear to the C/C++ preprocessor if that 
configuration of preprocessor symbols had been applied in advance.

The most useful of the questions that coan can answer is: 
I<What would the source code look like if re-written with all of the simplifications 
that follow from the given configuration?> Coan can produce the simplified 
re-write that answers this question. So, if the source code contains redundant 
preprocessor complexities that depend on the status of some preprocessor symbols,
coan can be given a configuration that specifies the status of those symbols, and it
will output an new version of the source code from which those redundant 
complexities have been eliminated. Or, if the source code is obscured by preprocessor
logic that differentiates several variants by reference to preprocessor symbols,
coan can be given a configuration that specifies any of these variants and will 
be able to generate a simpler version of the source code that represents only the
specifed variant.

Source code re-written by coan is I<not> pre-processed code as produced by the 
preprocessor. It still contains comments, macro-invocations, and #-directives. 
It is still source code, but simplified.

Other questions that coan can answer include ones about the preprocessor symbols
that are invoked in the source code and their properties under a given configuration. 
For example: What symbol references would appear within I<active> preprocessor directives 
under a given configuration, and could thus influence the preprocessor under that
configuration? What are the macro-expansions of symbol references? How are
those macro-expansions derived? What integer values would accrue to the symbol
references that can be evaluated under a given configuration? 

Coan can also answer a range of questions about any category of preprocessor 
directive that might appear in the source code, e.g. 
What B<#include> directives that import system header files would be active under
 a given configuration? When coan is invoked to identify the symbol references 
or directives that satisfy certain criteria, it will output a list of the 
references or directives of interest, and optionally their locations in the 
source code.

=head1 COMMANDS

=over

=item B<help>

Display a usage summary and exit.

=item B<version>

Display version information and exit.

=item B<source> [OPTION...] [I<file>...] [I<directory>...]

Read the input files and rewrite them in accordance with the options.

=item B<spin> [OPTION...] [I<file>...] [I<directory>...]

Like the B<source>, with all output files organised beneath a specified directory
mirroring their original structure.  

=item B<symbols> [OPTION...] [I<file>...] [I<directory>...]

Select references of preprocessor symbols from the input files in accordance with 
the options and report them on the standard output in accordance with the options.

=item B<includes> [OPTION...] [I<file>...] [I<directory>...]

Select B<#include> directives from the input files in accordance with the options
and report them on the standard output in accordance with the options.

=item B<defs> [OPTION...] [I<file>...] [I<directory>...]

Select B<#define> and B<#undef> directives from the input files in accordance with
the options and report them on the standard output in accordance with the options.

=item B<pragmas> [OPTION...] [I<file>...] [I<directory>...]

Select B<#pragma> directives from the input files in accordance with the options
and report them on the standard output in accordance with the options.

=item B<errors> [OPTION...] [I<file>...] [I<directory>...]

Select B<#error> directives from the input files in accordance with the options 
and report them on the standard output in accordance with the options.

=item B<lines> [OPTION...] [I<file>...] [I<directory>...]

Select B<#line> directives from the input files in accordance with the options 
and report them on the standard output in accordance with the options.

=item B<directives> [OPTION...] [I<file>...] [I<directory>...]

Select preprocessor directives from the input files in accordance with the 
options and report them on the standard output in accordance with the options.

=back

=head1 OPTIONS

=head2 GENERAL OPTIONS

These options may be given with any command.

=over

=item B<-f>I<argfile>, B<--file> I<argfile>

Read (more) arguments from file I<argfile>. Arguments may be written free-form, 
separated by whitespace, in I<argfile>. These arguments will be parsed exactly 
as if they were listed on the commandline at the position of -B<f>I<argfile>.

=item B<-D>I<symbol>[B<(>I<param1>[B<,>I<param2>...]B<)>]=I<definition>, B<--define> I<symbol>[B<(>I<param1>[B<,>I<param2>...]B<)>]=I<definition>

Assume that the macro definition I<symbol>[B<(>I<param1>[B<,>I<param2>...]B<)>]=I<definition> 
is in force for processing the input file(s). If B<=>I<definition> is unspecified then I<string> defaults to the empty string.

=item B<-U>I<symbol>, B<--undef> I<symbol>

Assume that the macro I<symbol> is undefined for processing the input file(s).

=item B<-m>, B<--implicit>

Assume that any symbol that is not B<--define>-ed is implicitly B<--undef>-ed.

This option is not allowed to have the perverse effect that an initial invocation, with
arguments, of an unconfigured symbol, e.g. I<sym>B<(>I<a,b>B<)>, is expanded as
B<0(>I<a,b>B<)> to provoke a syntax error. The invocation is taken to imply
that I<sym> has a conforming but unknown definition and is passed through
unresolved. However, once I<sym> has been invoked either with arguments or
without then subsequent invocations are expected to be of the same form unless
I<sym> is redefined or undefined in the meanwhile.  

=item B<-R>, B<--recurse>

Recurse into directories to find input files. (With the B<source> command, B<--recurse>
implies B<--replace>). With B<--recurse>, the input files may include directories: 
otherwise a directory provokes a non-fatal error.

All files beneath a directory will be selected for input unless the B<--filter> 
option is given: otherwise all files will be selected that match the B<--filter>
option.

When B<--recurse> is in effect, B<coan> builds a graph of all unique input files
once and for all as it parses the filenames that are explicitly supplied and before
it processes any of them. New files that may later appear in input directories 
during execution will not be processed, and files that have disappeared from input
directories when they are due to be processed will provoke fatal errors.

=item B<-F>I<ext1>[B<,>I<ext2>...], B<--filter> I<ext1>[B<,>I<ext2>...]

Process only input files that have one of the file extensions I<ext1>,I<ext2>...
A file extension is the terminal segment of a filename that immediately follows 
the final '.'.

=item B<-g>[B<p>|B<i>|B<w>|B<e>|B<a>], B<--gag> [B<progress> | B<info> | B<warning> | B<error> | B<abend>]

Suppress diagnostics no worse than [B<progress> | B<info> | B<warning> | B<error> | B<abend>].

=item B<-gs>, B<--gag summary>.

Suppress summary diagnostics at end of input.

=item B<-V>, B<--verbose>

Output all diagnostics,

If neither B<-V> nor B<-g>I<arg> is specified B<defaults> are B<-gp -gi -gs>.

=item B<-E>, B<--evalconsts>

Constants occurring as truth-functional operands in B<#if> expressions shall be 
evaluated and eliminated. By default constants as truth-functional operands are 
treated as unknowns, i.e. like macros that are not subject to any assumptions. 
Constants are always evaluated and eliminated when they are arithmetic or 
bitwise operands.

The default behaviour preserves "work-in-progress" directives such as B<#if 1> 
and B<#if 0> along with the code they control. With B<--evalconsts> these 
directives are evaluated.

=item B<-K>, B<--keepgoing>

If a parse error is encountered in an input file, continue processing subsequent
input files. An event of severity B<abend> will terminate processing regardless
of B<--keepgoing>.

=item B<--no-transients>

By default an in-source B<#define> I<SYM> or B<#undef> I<SYM> directive is 
transiently treated as a B<-D>I<SYM> or B<-U>I<SYM> option within the source 
file where it is found. This option suppresses the default behaviour at your own
risk.

=item B<-P>, B<--pod>

Apart from #-directives, input is to be treated as Plain Old Data. C or C++ 
comments and quotations will not be parsed. 

=back

=head2 OPTIONS FOR THE source COMMAND

=over

=item B<-r>, B<--replace>

Replace each input file with the corresponding output file. I<This option must 
be specified to process multiple input files>.

The option changes the default behaviour of the command when no input files are
specified. In that case, input is acquired from the standard input. If B<--replace> 
is I<not> specified, then a single input file is read from the standard input. 
If B<--replace> is specified then the I<names> of the input files are read from 
the standard input. Note that the B<--recurse> option implies B<--replace>.

If you wish to preserve the original files with the B<--replace> option, use the
B<--backup> option as well.

If the names of the input files are read from B<stdin>, the filenames are 
delimited by whitespace unless enclosed in double-quotes.

=item B<-b>I<suffix>, B<--backup> I<suffix>

Backup each input file before replacing it, the backup file having the same name
as the input file with I<suffix> appended to it.

=item B<-x>[B<d>|B<c>|B<e>], B<--conflict> [B<delete> | B<comment> | B<error>]

Select the action to be taken when a B<#define> or B<#undef> directive is encountered
in an input file that conflicts with one of the B<-D> or B<-U> assumptions:

B<d>, B<delete>: Delete the conflicting directive.

B<c>, B<comment>: Replace the conflicting directive with a diagnostic comment 
(B<default>).

B<e>, B<error>: Replace the conflicting directive with a diagnostic B<#error> 
directive.

=item B<-k>[B<d>|B<b>|B<c>], B<--discard> [B<drop> | B<blank> | B<comment>]

Select the policy for discarding lines from output:

B<d>, B<drop>: Drop discarded lines.

B<b>, B<blank>: Blank discarded lines.

B<c>, B<comment>: Comment out discarded lines.

=item B<--line>

Output B<#line> directives in place of discarded lines to preserve the line 
numbers of retained lines.

=item B<-c>, B<--complement>

Ouput the lines that ought to be dropped and vice versa.

=back

=head2 OPTIONS FOR THE spin COMMAND

The B<--recurse> and B<--replace> options are implied for this command.

=over

=item B<-x>[B<d>|B<c>|B<e>], B<--conflict> [B<delete> | B<comment> | B<error>]

Select the action to be taken when a B<#define> or B<#undef> directive is encountered
in an input file that conflicts with one of the B<-D> or B<-U> assumptions:

B<d>, B<delete>: Delete the conflicting directive.

B<c>, B<comment>: Replace the conflicting directive with a diagnostic comment
(B<default>).

B<e>, B<error>: Replace the conflicting directive with a diagnostic B<#error> 
directive.

=item B<-k>[B<d>|B<b>|B<c>], B<--discard> [B<drop> | B<blank> | B<comment>]

Select the policy for discarding lines from output:

B<d>, B<drop>: Drop discarded lines.

B<b>, B<blank>: Blank discarded lines.

B<c>, B<comment>: Comment out discarded lines.

=item B<--line>

Output B<#line> directives in place of discarded lines to preserve the line numbers 
of retained lines.

=item B<-c>, B<--complement>

Ouput the lines that ought to be dropped and vice versa.

=item B<--dir> I<dirname>

Generate a spin under the directory I<dirname>. The output files are generated 
relative to I<dirname>, mirroring their original structure. I<dirname> and 
subdirectories will be created as required. Pre-existing output files will be 
overwritten. A fatal error is given if I<dirname> includes, or is included by or
is identical with any input I<directory>.  

=item B<-p>I<pathame>, B<--prefix> I<pathname>

In organising output files beneath the spin directory I<dir>, I<dir> will 
equated with the path prefix I<pathname> of any input file. This enables any 
common prefix of all input files to be deleted from all the outout files under
I<dir> if desired.   

=back


=head2 OPTIONS FOR THE symbols COMMAND

=over

=item B<-i>, B<--ifs>

List symbol references that occur in B<#if>, B<#if[n]def> and B<#elif> directives.

=item B<-d>, B<--defs>

List symbol references that occur in B<#define> directives.

=item B<-u>, B<--undefs>

List symbol references that occur in B<#undef> directives.

=item B<--includes>

List symbol references that occur in B<#include> directives.

=item B<--lns>

List symbol references that occur in B<#line> directives.

=back

If none of B<-i>, B<-d>, B<-u>, B<--includes> or B<--lns> is given then they are
all B<defaults>.

=over

=item B<-o>, B<--once-only>

List only the first ocurrence of each distinct reference of a symbol.

=item B<--once-per-file>

List only the first ocurrence per input file of each distinct reference of a symbol.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all of a symbol's
references are listed.

=over 

=item B<-A>, B<--active>

List only references from operative directives.

=item B<-I>, B<--inactive>

List only references from inoperative directives.

=item B<-L>, B<--locate>

Report the source file and line number of each listed reference.

=item B<-e>, B<--expand>

Report the macro expansion of each reported symbol reference, and the integer to
which it evaluates, if any. An expansion will be reported for a reference of 
the symbol I<symbol> if I<symbol> is configured by a B<-D>option or a B<-U> option,
or transiently configured by a B<#define> or B<#undef> directive in the current file. 

In the case B<-D>I<symbol>, the expansion of I<symbol> is the empty string (since
that is the C preprocessor's default definition for defined symbols).

In the case B<-U>I<symbol>, the expansion of I<symbol> is the numeral B<0> (since
0 is the value assigned by the preprocessor to an undefined symbol).

In the case B<-D>I<symbol>[B<(>I<param1>[B<,>I<param2>...]B<)>]=I<definition>, 
the expansion of the reference is the string that results by, first, substituting
the arguments of the references, if any, for the corresponding formal parameters 
of the symbol in its definition and then recursively replacing each symbol reference
in the result with its expansion. 

If a symbol is not configured then its references are reported as B<insoluble>.
Likewise if it is configured but a reference has a circular expansion.

=item B<--explain>

Report the successive steps of macro expansion for each reported symbol reference. 
Implies B<--expand> 

=item B<--select> I<name1>[B<*>][,I<name2>[B<*>]...]

Select by name the symbols to be reported. A symbol reference will be reported 
only if its name matches one of the comma-separated names or B<*>-terminated names.
A B<*>-terminated name is interpreted as a wildcard name prefix. If not specified
then B<--select *> is the default. 

=back

=head2 OPTIONS FOR THE includes COMMAND

=over

=item B<-s>, B<--system>

List system headers, e.g. <header.h>

=item B<-l>, B<--local>

List local headers, e.g. "header.h"

=back

If neither B<-s> or B<-L> is given then both are B<defaults>.

=over

=item B<-o>, B<--once-only>

List only the first occurrence of each header.

=item B<--once-per-file>

List only the first occurrence per input file of each header.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all occurrences
of a header are listed.

=over

=item B<-A>, B<--active>

List only headers from operative directives.

=item B<-I>, B<--inactive>

List only headers from inoperative directives.

=item B<-L>, B<--locate>

Report the source file and line number of each listed occurrence

=back

=head2 OPTIONS FOR THE defs COMMAND

=over

=item B<-o>, B<--once-only>

List only the first occurrence of each distinct B<#define> or B<#undef> directive.

=item B<--once-per-file>

List only the first occurrence per input file of each distinct B<#define> or B<#undef> directive.

For the purpose of determining whether two occurrences express the same directive
or not, coan reduces the text of each occurrence to a I<canonical form>, i.e. all
comments are stripped out, all whitespace sequences are collapsed to a single space
and all tokens are separated by a single space. This applies for all commands that
accept the B<--once-only> or B<--once-per-file> option.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all occurrences
of a distinct B<#define> or B<#undef> directive are listed.

=over

=item B<-A>, B<--active>

List only operative B<#define> and B<#undef> directives.

=item B<-I>, B<--inactive>

List only inoperative B<#define> and B<#undef> directives.

=item B<-L>, B<--locate>

Report the source file and line number of each listed occurrence.

=back

=head2 OPTIONS FOR THE pragmas COMMAND

=over

=item B<-o>, B<--once-only>

List only the first occurrence of each distinct pragma.

=item B<--once-per-file>

List only the first occurrence per input file of each distinct pragma.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all occurrences
of a pragma are listed.

=over

=item B<-A>, B<--active>

List only operative pragmas.

=item B<-I>, B<--inactive>

List only inoperative pragmas.

=item B<-L>, B<--locate>

Report the source file and line number of each listed occurrence.

=back

=head2 OPTIONS FOR THE errors COMMAND

=over

=item B<-o>, B<--once-only>

List only the first occurrence of each B<#error> directive.

=item B<--once-per-file>

List only the first occurrence per input file of each B<#error> directive.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all occurrences
of a distinct B<#error> directive are listed.

=over

=item B<-A>, B<--active>

List only operative B<#error> directives.

=item B<-I>, B<--inactive>

List only inoperative B<#error> directives.

=item B<-L>, B<--locate>

Report the source file and line number of each listed occurrence.

=back

=head2 OPTIONS FOR THE directives COMMAND

=over

=item B<-o>, B<--once-only>

List only the first occurrence of each distinct directive.

=item B<--once-per-file>

List only the first occurrence per input file of each distinct directive.

=back

If neither B<--once-only> nor B<--once-per-file> is given then all occurrences
of a distinct directive are listed.

=over

=item B<-A>, B<--active>

List only operative directives.

=item B<-I>, B<--inactive>

List only inoperative directives.

=item B<-L>, B<--locate>

Report the source file and line number of each listed occurrence.

=back

=head1 DIAGNOSTICS

Diagnostics written to stderr are classified by severity. Each diagnostic includes
a distinct hexadecimal code of the form 0xXXXXX that encodes its severity. The 5
 severities are:

=over

=item Z<>

B<progress>: Progress messages (0xXXXXX & 0x00800 is true)

B<info>: Noteworthy information (0xXXXXX & 0x01000 is true)

B<warning>: Indicating problematic input (0xXXXXX & 0x02000 is true)

B<error>: Indicating invalid input (0xXXXXX & 0x04000 is true)

B<abend>: Indicating a fatal environment or internal error (0xXXXXX & 0x08000 is true)

=back

If B<--gag summary> is not in force, coan can write summary diagnostics at the 
end of processing. A summary diagnostic has a hexadecimal code B<S> that encodes
one of the severities and in addition B<S> & 0x10000 is true. Even if B<--gag summary>
is not in force, a summary will not be written if its severity is suppressed by
one of the specified or default B<--gag> options. Since all summaries have 
severity I<info> or I<warning>, and since the B<--gag> defaults are 
B<info>, B<warning> and B<summary>, you should specify  B<--gag info> to see 
only warning summaries, and to see all summaries you should specify B<--verbose>.
The summaries include:

=over

=item Z<>

B<info>: The number of input files that were reached and the number that were not
reached (due to abend).

B<info>: The number of input files reached that were abandoned (due to errors).

=back

If there was no abend or error, then additional summaries are written 
(unless suppressed) indicating each of the following outcomes that has occurred:

=over

=item Z<>

B<info>: Input lines were dropped through simplification.

B<info>: Input lines were changed through simplification. If the option 
B<--discard comment> is in effect, causing discarded lines to be commented out, 
then discarded lines are counted as changed, not dropped.

B<warning>: Input lines were changed to B<#error> directives.

B<warning>: B<#error> directives were operative (either in the input source or as
a result of simplification).

=back

Coan returns a system code B<SC> of which the low order half of the low order 
byte is always meaningful:

=over

=item Z<>

B<SC> & 1: Informational diagnostics accrued.

B<SC> & 2: Warnings diagnostics accrued.

B<SC> & 4: Error diagnostics accrued. (Input files provoking errors will be 
unchanged notwithstanding the B<--replace> option.)

B<SC> & 8: An abend occurred. Some input files may not have been reached.

=back

If no error or abend is indicated, then the high order half of the low order 
byte is also meaningful:

=over

=item Z<>

B<SC> & 16: Input lines were dropped through simplification.

B<SC> & 32: Input lines were changed through simplification.

B<SC> & 64: Input lines were changed to B<#error> directives.

B<SC> & 128: B<#error> directives were operative.

=back

The system code reflects diagnostics that were provoked even if they were not 
actually output due to B<--gag> options.

=head1 BUGS

Trigraphs are not parsed.

Files that are B<#include>-ed by input files are not parsed, hence any 
B<#define> or B<#undef> directives they contain will not be factored into the 
evaluation of the file.

Please report bugs to bugs dot coan at burroingroingjoing dot com

=head1 AUTHOR

Mike Kinghan imk at burroingroingjoing dot com

=begin html

<p><table width="100%"><tr><td align="left">burroingroingjoing.com</td><td align="center">{{REL_MONTH}}</td><td align="right">COAN(1)</td></tr></table>

=end html

=cut 
