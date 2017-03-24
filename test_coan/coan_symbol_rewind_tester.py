#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'
import sys, os, argparse

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.path.abspath(os.pardir)
	

sys.path.append(os.path.join(top_srcdir,'python'))

import coanlib
from coanlib import *

parser = argparse.ArgumentParser(
	prog=get_prog(),
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description='Test that coan correctly purges transient symbols '\
		'after every input file')
		
parser.add_argument('-p', '--pkgdir', metavar='PKGDIR',
    help='PKGDIR is the coan package directory. '
    'Default is value of environment variable COAN_PKGDIR is defined, '
    'else \"..\"')
	
parser.add_argument('-e', '--execdir', metavar='EXECDIR',default='src',
    help='EXECDIR is the directory beneath '
		'PKGDIR from which to run coan: Default \"src\"')
		
parser.add_argument('-b', '--bail', action='store_true',default=False,
    help='Bail at the first error. Default: Keep going through errors.')
    
    
output_files = []
replaced_files = []
backed_up_files = []
testno = 1
failures = 0
args = vars(parser.parse_args())
bail = args['bail']
pkgdir = deduce_pkgdir(args)
execdir = deduce_execdir(args)
	
def reference_run(testfiles):
	''' Do a reference run, passing each input file individually
	to coan via coan_case_tester.py to get reference results
	in the .output files '''
	rc = run('python ' 
	+ os.path.join(top_srcdir,'test_coan','coan_case_tester.py') \
	+ ' --pkgdir=' + pkgdir
	+ ' --execdir=' + execdir 
	+ ' --client=\'coan_symbol_rewind_tester\' '
	+ ' ' + ' '.join(testfiles), timing = False)
	if rc:
		finis(rc)
	for file in testfiles:
		output = file + '.output'
		if os.path.exists(output):
			output_files.append(output)

def test_run(testfiles,monkey_args,expected_rc):
	''' Do a the run, passing all the input files at once to coan
	coan via coan_case_tester.py to get test results in the overwritten
	input files (which are backed up) '''
	rc = run('python ' 
	+ os.path.join(top_srcdir,'test_coan','coan_case_tester.py') \
	+ ' --pkgdir=' + pkgdir
	+ ' --execdir=' + execdir
	+ ' --client=coan_symbol_rewind_tester'
	+ ' --monkey=\"' + monkey_args + '\" --rc=' + str(expected_rc) + ' '
	+ ' ' + ' '.join(testfiles), timing = False)
	for file in testfiles:
		backed_up_file = file + '.coan.test.bak'
		if os.path.exists(backed_up_file):
			backed_up_files.append(backed_up_file)
	return rc;
			
def compare_results(testfiles):
	''' Compare each overwritten input file from the test run
	with the corresponding .output file from the reference run.
	They should be identical.'''
	errors = 0
	for testfile in testfiles:
		backed_up_file = testfile + '.coan.test.bak'
		if not backed_up_file in backed_up_files:
			orig_text = slurp_file(testfile)
			if orig_text.find('NO-OUTPUT') != -1:
				continue
			else:
				errors = errors + 1
				error('Expected some output from \"' + testfile +
					'\" but got none')
				continue
		reference = testfile + '.output'
		reference_text = slurp_file(reference)
		test_text = slurp_file(testfile)
		if test_text != reference_text:
			errors = errors + 1
			error('Test output \"' + testfile + '\" differs from '
				+ 'reference output \"' + reference + '\"')
	return errors 

def restore_backups():
	''' Restore the backed up input files '''
	for backup in backed_up_files:
		pos = backup.find('.coan.test.bak')
		original = backup[0:pos]
		file_copy(backup,original)
		file_del(backup)

def do_test(testfiles,monkey_args,expected_rc):
	''' Do a test '''
	global testno
	global failures
	for i in range(len(testfiles)):
		testfiles[i] = os.path.join(top_srcdir,'test_coan',testfiles[i])
	msg = '*** Symbol-rewind test {0}: ' \
		'to process {1}  files ***'.format(testno, len(testfiles))
	progress(msg)
	mismatches = 0
	reference_run(testfiles)
	rc = test_run(testfiles,monkey_args,expected_rc)
	if rc == 0:
		mismatches = compare_results(testfiles)
	restore_backups()
	del backed_up_files[:]
	testno = testno + 1
	failed = rc != 0 or mismatches != 0
	failures = failures + int(rc)
	if failures and bail:
		finis(failures)

set_prog('coan_symbol_rewind_tester')	
testfiles = ['test_cases\\test0286.c','test_cases\\test0287.c'] \
	if windows() else ['test_cases/test0286.c','test_cases/test0287.c']
#monkey_args = 'source --no-inc -DFOO --replace --backup .coan.test.bak'
monkey_args = 'source -DFOO --replace --backup .coan.test.bak'
expected_rc = 19

do_test(testfiles,monkey_args,expected_rc)

testfiles = ['test_cases\\test0393.c','test_cases\\test0394.c'] \
	if windows() else ['test_cases/test0393.c','test_cases/test0394.c']
monkey_args = 'source -DFOO --replace --backup .coan.test.bak'
expected_rc = 5

do_test(testfiles,monkey_args,expected_rc)

msg = '{0} out of {1} tests failed'.format(failures,testno - 1)
info(msg)

finis(failures)
			
