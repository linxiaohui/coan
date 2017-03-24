#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'

import sys, argparse, os, string, re, atexit

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.pardir

sys.path.append(os.path.join(top_srcdir,'python'))

from coanlib import *
#if sys.version_info < (2,7):
#	sys.path.append(os.path.join(top_srcdir,'python','argparse-1.2.1'))
#import argparse

set_prog('coan_bulk_tester')

parser = argparse.ArgumentParser(
	prog=get_prog(),
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description='Exercise coan in traversing a directory tree. ' 
	'A roughly random tree of directories is created in PKGDIR/test_coan/scrap '
	'and each directory is populated with the coan source files from PKGDIR/src. '
	'The directory tree is passed into a number of coan tests',
	epilog='BREADTH and DEPTH must be > 0.\n'
		'BEWARE!: The size of the hierarchy grows exponentially with BREADTH and DEPTH. '
		'BREADTH = 20, DEPTH = 10 is likely to create in the order of a million files')
    
parser.add_argument('-v', '--verbosity', default='progress', metavar='LEVEL',
    help='Display diagnostics with severity >= LEVEL,' +
    ' where LEVEL = \'progress\', \'info\', \'warning\', \'error\' or' +
    ' \'fatal\'. Default = \'progress\'.')

parser.add_argument('-p', '--pkgdir', metavar='PKGDIR',
    help='PKGDIR is the coan package directory. '
    'Default is value of environment variable COAN_PKGDIR is defined, '
    'else \"..\"')
    
parser.add_argument('-e', '--execdir', metavar='EXECDIR',default='src',
    help='EXECDIR is the directory beneath '
		'PKGDIR from which to run coan: Default \"src\"')
				
parser.add_argument('--breadth', metavar='BREADTH',
	type=int,default=5,
    help='There will be at most BREADTH subdirectories in a directory. Default 5')

parser.add_argument('-d', '--depth', metavar='DEPTH',
	type=int,default=4,
    help='A directory will have at most DEPTH levels of subdirectories. Default 4')
    
parser.add_argument('-s', '--seed', metavar='SEED',type=int,
	default=987654321,
    help='Seed the pseudo random number generator with the number SEED.'
    ' Default 987654321')

parser.add_argument('-b', '--bail', action='store_true',default=False,
    help='Bail at the first error. Default: Keep going through errors.')

parser.add_argument('-k', '--keep', action='store_true',
	default=False,
    help='Do not delete the test directory tree or console logs at exit')
    
parser.add_argument('-t', '--tests', metavar='NTESTS', type=int,
    help='Run only the first NTESTS of the bulk tests')
    
parser.add_argument('infile', metavar='TESTFILE', nargs='*',
    help='Test files to process.')

args = vars(parser.parse_args())
set_verbosity(args['verbosity'])
pkgdir = deduce_pkgdir(args)
execdir = deduce_execdir(args)
if not windows():
	os.system('chmod -R +w ' + pkgdir)
do_metrics()

executable = os.path.join(execdir,'coan.exe') if windows() \
	else os.path.join(execdir,'coan')
scrapdir = os.path.join(pkgdir,'test_coan','scrap')
scrapdir = os.path.join(pkgdir,'test_coan','scrap')
breadth = args['breadth']
depth = args['depth']
seed = args['seed']
bailing = args['bail']
keep = args['keep']
ntests = args['tests']
if not ntests: ntests = 99
stderr_file = 'stderr.temp.txt'
stdout_file = 'stdout.temp.txt'
infiles_file = 'infiles.temp.txt'
undefs_file = 'undefs.temp.txt'
scrap_files = []
infiles_list = []
undefs_list = []
num_infiles = 0
num_sabotaged_files = 0
failures = 0
testno = 0

def exithandler():
	''' atexit() cleanup '''
	if not keep:
		del_tree(scrapdir)
		if not failures:
			file_del(stderr_file)
			file_del(stdout_file)
			file_del(infiles_file)
			file_del(undefs_file)

def gather_scrap_files(arg,dirname,names):
	''' Select all the C/C++ header and source file is the current
	directory and add their names to the list of scrap files
	'''
	for name in names:
		(base,ext) = os.path.splitext(name)
		if ext in ['.h', '.c', '.cpp']:
			scrap_files.append(os.path.join(dirname,name))

def tally_source_files(arg,dirname,names):
	''' Count all the C/C++ header and source file is the current
	directory and add the total to the total of test input files
	'''
	global num_infiles
	for name in names:
		(base,ext) = os.path.splitext(name)
		if ext in ['.h', '.c', '.cpp']:
			num_infiles += 1

def restore_backed_up_files(arg,dirname,names):
	''' Restore all backed up files in the current directory '''
	for file in names:
		match = re.match('^(.+)~$',file)
		if match:
			name = match.group(1)
			name = os.path.join(dirname,name)
			file_del(name)
			file = os.path.join(dirname,file)
			os.rename(file,name)

def append_files_to_infiles_list(arg,dirname,names):
	''' Select all the files in the current directory and
	add their names to the list of test input files '''
	global infiles_list
	infiles_list.extend([ os.path.join(dirname,file) + '\n' for file in names])
		
def check_test_result(pattern = None):
	global ntests
	global testno
	global failures
	testno += 1
	fail = False
	text = slurp_file(stderr_file)
	msg = 'info 0x02201: {0} out of {0} input files were reached'\
		.format(num_infiles)
	fail = text.find(msg) == -1
	if not fail and pattern != None:
		fail = re.search(pattern,text) == None
	if fail:
		failures += 1
		msg = '*** Bulk test {0}: Failed! See {1} ****'.\
			format(testno,stderr_file)
		if bailing:
		  bail(msg,failures)
	if testno >= ntests:
		msg = '*** Stopping by request after test {0} ****'.\
			format(testno)
		bail(msg,0)

def sabotage_source_file(file):
	''' Sabotage a source file by deleting all the #endif
	directives '''
	global num_sabotaged_files
	sabotaged = False
	(base,ext) = os.path.splitext(file)
	if ext in ['.h', '.c', '.cpp']:
		fh = fopen(file,'r')
		lines = fh.readlines()
		fh.close()
		for ndx,line in enumerate(lines):	
			match = re.match('^#endif(.*)$',line)
			if match:
				lines[ndx] = match.group(1)
				sabotaged = True
		if sabotaged:
			fh = fopen(file,'w')
			fh.writelines(lines)
			fh.close()
			num_sabotaged_files += 1
			
def sabotage_source_files(arg,dirname,names):
	''' Sabotage all eligible source files in the current directory '''
	for file in names:
		sabotage_source_file(os.path.join(dirname,file))
		
def generate_sabotaged_files():
	''' Sabotage all eligible test input files '''
	os.path.walk(scrapdir,sabotage_source_files,None)
					
def restore_all_backed_up_files():
	''' Restore all the test files backed up by the last test. '''
	os.path.walk(scrapdir,restore_backed_up_files,None)
		
def generate_test_data():
	''' Create and populate a directory structure for bulk tests ''' 
	progress('*** Generating data for bulk tests. Patience! ***')
	os.path.walk(os.path.join(pkgdir,'src'),gather_scrap_files,None)
	cmd = 'python {0} --verbosity=fatal --wipe --seed={1} --depth={2} '\
	'--rootname={3} --breadth={4} {5}'.\
		format(os.path.join(pkgdir,'test_coan','scrap_dir_tree.py'),
			seed,depth,scrapdir,breadth,' '.join(scrap_files))
	run_noerr(cmd)
	
def generate_infiles_list():
	''' Populate *infiles_list* with the names of all test input files
	'''
	os.path.walk(scrapdir,append_files_to_infiles_list,None)
	
def count_test_files():
	''' Count the test *.c. *.cpp and *.h files created '''
	os.path.walk(scrapdir,tally_source_files,None)

def generate_undefs_list():
	''' Build list *undefs_list * of -U-macros for all symbols found 
	in the test files, max 64K in length '''
	fh = fopen(stdout_file,'r')
	lines = fh.readlines()
	length = 0
	fh.close()
	for line in lines:
		length += len(line)
		line = line.replace(': unconfigured','');
		undefs_list.append(' -U' + string.strip(line))
		if length >= (1 << 16):
			break
			
def write_list_to_file(list,file):
	''' Write a list of filenames to a file '''
	fh = fopen(file,'w')
	fh.writelines(list)
	fh.close()
	
	
def generate_infiles_file():
	''' List all test input files to file *infiles_file *'''
	write_list_to_file(infiles_list,infiles_file)
	
def generate_undefs_file():
	''' List all test input files to file *infiles_file *'''
	write_list_to_file(undefs_list,undefs_file)

atexit.register(exithandler)
generate_test_data()
count_test_files()
set_verbosity(args['verbosity'])

progress('*** Bulk Test 1: to process {0} files ***'.\
		format(num_infiles))
update_test_size_file(num_infiles)
# Run coan with --recurse to list all preprocessor
# symbols in test .cpp and .h files, passing the scrap tree *twice*
# Test that the number of files processed is the number
# of .cpp and .h files

#cmd = '{0} symbols --once-only --no-inc --verbose --recurse --filter cpp,h {1} {1}'.\
cmd = '{0} symbols --once-only --verbose --recurse --filter cpp,h {1} {1}'.\
		format(executable,scrapdir,scrapdir) 
run(cmd,stdout_file,stderr_file)
progress("*** Done ***");
check_test_result();

progress('*** Bulk Test 2: to process {0} files ***'.\
	format(num_infiles))
update_test_size_file(num_infiles)
# Run coan with --recurse on the scrap tree again, to
# apply --undef SYM to all .cpp and .h files, for symbol SYM
# that was found by the first test.
generate_undefs_list()
#cmd = '{0} source {1} --no-inc --verbose --recurse '\
cmd = '{0} source {1} --verbose --recurse '\
	'--filter cpp,h --backup \"~\" {2}'.\
	format(executable,' '.join(undefs_list),scrapdir)
run(cmd,None,stderr_file)
progress("*** Done ***")
check_test_result();

progress('*** Bulk Test 3: to process {0} files ***'.\
	format(num_infiles))
update_test_size_file(num_infiles)
# Run coan as per the 2nd test, but this time read all the
# file and directory names in the scrap tree from stdin.
restore_all_backed_up_files()
generate_infiles_list()
generate_infiles_file()
#cmd = '{0} source {1} --no-inc --verbose --recurse --filter cpp,h --backup \"~\"'.\
cmd = '{0} source {1} --verbose --recurse --filter cpp,h --backup \"~\"'.\
	format(executable,' '.join(undefs_list))
run(cmd,None,stderr_file,infiles_file)
progress("*** Done ***");
check_test_result();

progress('*** Bulk Test 4: to process {0} files ***'.\
	format(num_infiles))
update_test_size_file(num_infiles)
# Run coan with --recurse on the scrap tree again, this time
# reading all the --undef options of test 2 from a file.
restore_all_backed_up_files()
generate_undefs_file()
#cmd = '{0} source --file {1} --no-inc --verbose --recurse '\
cmd = '{0} source --file {1} --verbose --recurse '\
	'--filter cpp,h --backup \"~\" {2}'.\
	format(executable,undefs_file,scrapdir)
run(cmd,None,stderr_file);
progress("*** Done ***");
check_test_result();

progress('*** Bulk Test 5: to process {0} files ***'.\
	format(num_infiles))
update_test_size_file(num_infiles)
# Sabotage many source files in the scrap tree by deleting
# all occurrences of the #endif directive. Then run coan
# to traverse the scrap tree and determine that it drops all
# the sabotaged files.
#
restore_all_backed_up_files()
generate_sabotaged_files()
progress('*** Generated {0} invalid input files. This is intended! ***'.\
	format(num_sabotaged_files))
#cmd = '{0} symbols --once-only --no-inc --verbose --keepgoing '\
cmd = '{0} symbols --once-only --verbose --keepgoing '\
	'--recurse --filter cpp,h {1}'.	format(executable,scrapdir)
run(cmd,None,stderr_file);
progress("*** Done ***");
check_test_result('{0} were abandoned due to parse errors'.\
	format(num_sabotaged_files))

finis(failures)
