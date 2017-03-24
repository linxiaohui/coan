#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'

import string, sys, os.path, argparse, glob, shutil

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.pardir

sys.path.append(os.path.join(top_srcdir,'python'))

from coanlib import *	
from class_TestCase import TestCase
#if sys.version_info < (2,7):
#	sys.path.append(os.path.join(top_srcdir,'python','argparse-1.2.1'))
#import argparse

set_prog('coan_case_tester')

parser = argparse.ArgumentParser(
	prog=get_prog(),
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description='Test the coan expression parser.',
    epilog="If TESTFILE... is absent then PKGDIR/test_coan/test_cases/*.c is assumed.\n" + 
		"\tTESTFILE contains sample code on which coan is to be tested. If there exists\n" +
		 "a file called TESTFILE.expect then that file contains the code that coan is\n"
		 "expected to emit from the test case. Surplus whitespace is ignored for the\n" +
		 "purpose of comparing TESTFILE with TESTFILE.expect.\n" +
		"TESTFILE has a mandatory header comment of the form:\n" +
		"\t/**ARGS:\n" +
		"\t\tARGS_BODY\n" +
		"\t*/\n" +
		"where ARGS_BODY comprises the commandline options to be passed to coan.\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t/**ALTFILES:\n" +
		"\t\tFILENAME1 [FILENAME2...]\n" +
		"\t*/\n" +
		"The listed files will be input to coan with the arguments ARGS_BODY instead of\n" +
		"TESTFILE itself. If FILEENAMEn is relative then it is assumed to be relative\n" +
		"to PKGDIR/test_coan\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t/**SYSCODE: [=] FLAGS */\n" +
		"where FLAGS is a value to be satisfied by the coan return code from running the\n" +
		"test case. If the prefix '=' is present then return code must exactly match FLAGS.\n" +
		"Otherwise the value is satisfied provided that all bits set in FLAGS are set in the\n" +
		"return code. FLAGS may be a decimal numeral or an |-combination of decimal numerals.\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t\t/**NO-OUTPUT */" +
		"In this case the test is not expected to write to stdout.\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t\t/**REDIRECT */" +
		"which may not be combined with ALTFILES. In this case the test file" +
		" is redirected to coan via stdin.\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t\t/**WORDSIZE={32|64} */\n" +
		"In this case the test is run only on a 32|64-bit system.\n" +
		"\tOptionally, TESTFILE also has a header comment of the form:\n" +
		"\t\t/**NOT-WINDOWS */n" +
		"In this case the test is not run on Windows.\n")

parser.add_argument('-v', '--verbosity', default='progress', metavar='LEVEL',
    help='Display diagnostics with severity >= LEVEL,' +
    ' where LEVEL = \'progress\', \'info\', \'warning\', \'error\' or' +
    ' \'fatal\'. Default = \'progress\'.')

parser.add_argument('-p', '--pkgdir', metavar='PKGDIR',
    help='PKGDIR is the coan package directory. '
    'Default is value of environment variable COAN_PKGDIR is defined, '
    'else \"..\"')
    
parser.add_argument('-m', '--monkey', metavar='ARGS',
    help='Pass all test files to coan to process in a single run with ' +
        'arguments ARGS. Do not attempt to verify the output.')

parser.add_argument('-r', '--rc', metavar='N', default=0,
    help='Expect return code N with --monkey. Default = 0.')

#parser.add_argument('-w', '--winexe', action='store_true',
#    help='The coan executable is a native windows executable.')

parser.add_argument('-e', '--execdir', metavar='EXECDIR',default='src',
    help='EXECDIR is the directory beneath '
		'PKGDIR from which to run coan: Default \"src\"')
		    
parser.add_argument('-b', '--bail', action='store_true',default=False,
    help='Bail at the first error. Default: Keep going through errors.')
    
parser.add_argument('-c', '--client', metavar='OTHER_SCRIPT',
    help='The script is being run for another script OTHER_SCRIPT '
		'and should record metrics as if it were OTHER_SCRIPT')
    
parser.add_argument('infile', metavar='TESTFILE', nargs='*',
    help='Test files to process.')
        
def get_wordsize():
	''' Return the wordsize of the host system '''
	prog = '.\\wordsize.exe' if windows() \
		else './wordsize'
	if not is_exe(prog):
		bail('*** Program \"' + prog +'\" not found in test_coan. '
			'Have you built wordsize.c? ***')
	wordsize = slurp_command(prog)
	return int(wordsize) * 8
	
args = vars(parser.parse_args())
client = args['client']
if (client):
	set_prog(client)
set_verbosity(args['verbosity'])
pkgdir = deduce_pkgdir(args)
execdir = deduce_execdir(args)
if not windows():
	os.system('chmod -R +w ' + pkgdir)
do_metrics()
testfiles = args['infile'];
if len(testfiles) == 0:
	pattern = os.path.join(pkgdir,'test_coan','test_cases','*.c')
	testfiles = glob.glob(pattern)

if len(testfiles) == 0:
	bail('*** No tests found! ***')
else:
	progress('*** Case tester: to process {0} files ***'.\
		format(len(testfiles)))
	update_test_size_file(len(testfiles))
	
executable = os.path.join(execdir,'coan.exe') if windows() \
	else os.path.join(execdir,'coan')
bail = args['bail']
monkey_args = args['monkey']
expected_rc = args['rc']
failures = 0        
passes = 0
num_tests = 0
archbits = get_wordsize()
	
if not archbits:
	bail('*** Cannot determine wordsize of this machine! ***')

	
if not monkey_args:
	num_tests = len(testfiles)
	for testfile in sorted(testfiles):
		test_case = TestCase(pkgdir,archbits,executable,[testfile])
		if not test_case.skip:
			failures += test_case.run() == False
			if failures and bail:
				break
else:
	for ndx,file in enumerate(testfiles):
		testfiles[ndx] = os.path.abspath(file)
	for testfile in testfiles:
			debris = glob.glob(testfile + '.coan.test.bak.*')
			for file in debris:
				file_del(file)
	num_tests = 1
	monkey_args = string.strip(monkey_args)
	test_case = TestCase(pkgdir,archbits,executable,testfiles,
							monkey_args,int(expected_rc))
	failures += test_case.run() == False

if not client and (not bail or not failures):
	msg = '{0} out of {1} tests failed'.format(failures,num_tests)
	info(msg)

finis(failures);

