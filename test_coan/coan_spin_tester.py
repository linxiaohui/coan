#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'

import sys, os, argparse, atexit, re, string

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.pardir

sys.path.append(os.path.join(top_srcdir,'python'))

from coanlib import *
#if sys.version_info < (2,7):
#	sys.path.append(os.path.join(top_srcdir,'python','argparse-1.2.1'))
#import argparse


set_prog('coan_spin_tester')

parser = argparse.ArgumentParser(
	prog=get_prog(),
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description='Exercise coan in traversing a directory tree '
		'containing softlinks to verify that coan follows '
		'them correctly')

parser.add_argument('-v', '--verbosity', metavar='LEVEL',
	default='progress',
    help='Display diagnostics with severity >= LEVEL, where '  
    	'LEVEL = \'progress\', \'info\', \'warning\', \'error\' or '
    	'\'fatal\'. Default = \'progress\'')

parser.add_argument('-p', '--pkgdir', metavar='PKGDIR',
    help='PKGDIR is the coan package directory. '
    'Default is value of environment variable COAN_PKGDIR is defined, '
    'else \"..\"')

parser.add_argument('-e', '--execdir', metavar='EXECDIR',
	default='src',
    help='EXECDIR is the directory beneath '
		'PKGDIR from which to run coan: Default \"src\"')
		
parser.add_argument('-k', '--keep', action='store_true',default=False,
    help='Do not delete the directory tree or console logs at exit')

parser.add_argument('-b', '--bail', action='store_true',default=False,
    help='Quit at the first test failure')

#parser.add_argument('-w', '--winexe', action='store_true',
#    help='The coan executable is a native windows executable')
        
parser.add_argument('--ver', '--version', action='version',
	version='%(prog)s 0.1 ' + copyright,
    help='Display version information and exit.')

args = vars(parser.parse_args())
set_verbosity(args['verbosity'])
pkgdir = args['pkgdir']
pkgdir = deduce_pkgdir(args)
execdir = deduce_execdir(args)
if not windows():
	os.system('chmod -R +w ' + pkgdir)
do_metrics()

failures = 0
keep = args['keep']
bail = args['bail']
spindir = os.path.join(pkgdir,'test_coan','spin')
extensions = ['.h'] if windows() else ['.c','.cpp','.h']
filters = ['h'] if windows() else ['c','cpp','h']
in_files = []
out_files = []
error_files = []
stderr_file = 'stderr.temp.txt'
stdout_file = 'stdout.temp.txt'
in_file_list = 'spin-test-infiles.txt'
out_file_list = 'spin-test-outfiles.txt'
error_file_list = 'spin-test-errorfiles.txt'
testno = 1
exe = 'coan.exe' if windows() else 'coan'
gripe = 'abend 0x0100e: The spin directory cannot include or be ' \
	'included by or identical with any input directory'
epitaph = 'progress 0x02101: Completed ABNORMALLY, exit code 0x08'
stderr_text = None
error_regex = re.compile('coan:\s+(\S+):\s+line\s+\d+:\s+error\s+0x\S{5}:')
	
def get_stderr_text():
	global stderr_text
	try:
		if not stderr_text:
			stderr_text = slurp_file(stderr_file);
	except:
		pass
		
def get_error_files():
	global stderr_text
	global error_files
	get_stderr_text()
	error_files = error_regex.findall(stderr_text)
	
def exithandler():
	''' atexit() cleanup '''
	if not keep:
		del_tree(spindir)
		if not failures:
			file_del(stderr_file)
			file_del(stdout_file)
			file_del(in_file_list)
			file_del(out_file_list)

def gather_files(list,dirname,names):
	'''
	Select all files in *dirname* with extensions in
	*extensions* and append them to *list*
	''' 
	for name in names:
		(base,ext) = os.path.splitext(name)
		if ext in extensions:
			list.append(os.path.join(dirname,name))
		
def gather_in_files(arg,dirname,names):
	'''
	Select all files in *dirname* with extensions in
	*extensions* and append them to *in_files*
	''' 
	gather_files(in_files,dirname,names)
			
def gather_out_files(arg,dirname,names):
	'''
	Select all files in *dirname* with extensions in
	*extensions* and append them to *out_files*
	'''
	gather_files(out_files,dirname,names)
	
def verify_spin_files(prefix):
	'''
	Verify that the files under *spindir* are the expected ones.
	'''
	global in_files
	global out_files
	global error_files	
	global failures
	get_error_files()
	num_outfiles = 0
	num_error_files = 0;
	out_files = []
	if prefix == None:
		prefix = spindir
	os.path.walk(spindir,gather_out_files,None)
	for in_file in in_files:
		if in_file in error_files:
			num_error_files += 1
			continue
		(result,subs) = re.subn('\"','',in_file)
		quote = subs != 0
		if prefix:
			in_file = string.replace(in_file,prefix,'',1)
		if windows():
			(drive,in_file) = os.path.splitdrive(in_file)
			in_file = string.replace(in_file,'\\','',1)
		else:
			in_file = string.replace(in_file,'/','',1)
		in_file = os.path.join(spindir,in_file)
		if quote:
			in_file = '\"' + infile + '\"' 
		num_outfiles += (in_file in out_files)
	if num_outfiles + num_error_files != len(in_files):
		fh = fopen(error_file_list,'w')
		for error_file in error_files:
			fh.write('{0}\n'.format(error_file))
		fh.close()		
		fh = fopen(in_file_list,'w')
		for in_file in in_files:
			fh.write('{0}\n'.format(in_file))
		fh.close()
		fh = fopen(out_file_list,'w')
		for out_file in out_files:
			fh.write('{0}\n'.format(out_file))
		fh.close()
		msg = '*** Failed to name-match output files and abandoned '\
			'files with input files: '\
			'See {0}, {1}. {2} ****'.format(in_file_list,\
				out_file_list,error_file_list)
		error(msg)
		failures += 1

def check_test_result(testno,gripe = None, epitaph = None):
	'''
	Check that the stderr output of a test is as expected
	'''
	global failures
	global stderr_text
	fail = 0
	num_infiles = len(in_files)
	summary = 'info 0x02201: {0} out of {1} ' \
		'input files were reached'.format(num_infiles,num_infiles)
	get_stderr_text();
	if gripe and epitaph:
		if stderr_text.find(gripe) == -1 or \
			stderr_text.find(epitaph) == -1: 
			fail = 1
	elif stderr_text.find(summary) == -1:
		fail = 1;
	if fail:
		failures += 1
	if failures:
		err_msg = '*** Spin test {0}: ' \
			'Failed! See {1} ****'.format(testno,stderr_file)
		error(err_msg)
		if bail:
		  sys.exit(failures)

def do_test(args,verify = True, prefix = None, 
			gripe = None, epitaph = None):
	'''
	Run a test.
	
	Arguments:
	args -- The variable arguments to be passed to coan
	verify -- Verify the files generated in the sin directory
	prefix -- The --prefix path passed to coan, if any
	gripe -- The expected fatal diagnostic if coan should fail
	epitaph -- The expected summary message if coan should fail
	'''
	global testno
	global stderr_text
	stderr_text = None
	if verify:
		del_tree(spindir)
	msg = '*** Spin Test {0}: ' \
		'to process {1}  files ***'.format(testno, len(in_files))
	update_test_size_file(len(in_files))
	progress(msg)
	cmd = os.path.join(execdir,exe) + ' spin ' + args  
	run(cmd,stdout_file,stderr_file)
	msg = '*** Done Test {0} ***'.format(testno)
	progress(msg);
	if verify:
		verify_spin_files(prefix)
	check_test_result(testno,gripe,epitaph)
	testno += 1
	
atexit.register(exithandler)

file_del(in_file_list)
file_del(out_file_list)
file_del(error_file_list)

# Get list of the .c, .h and .cpp files in the package dir
in_dirs = [ os.path.join(pkgdir,'src'), \
	os.path.join(pkgdir,'test_coan','test_cases') ]
for dir in in_dirs:
	os.path.walk(dir,gather_in_files,None)

if len(in_files) == 0:
	bail('*** No test data found! Bailing out confused ***')

# Testing input of several directory structures containing eligible files
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --filter ' +
do_test('--keepgoing -DWINDOWS --verbose --filter ' + 
	','.join(filters) + ' --dir ' + spindir + ' ' +
	' '.join(in_dirs))


# Testing input of the same eligible files as a flat list.
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --filter ' +
do_test('--keepgoing -DWINDOWS --verbose --filter ' + 
	','.join(filters) + ' --dir ' + spindir  +  ' ' + ' '.join(in_files))


# Repeat test 1 with a --prefix
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --prefix ' + pkgdir +
do_test('--keepgoing -DWINDOWS --verbose --prefix ' + pkgdir + 
	' --filter ' +	','.join(filters) + ' --dir ' + spindir + ' ' +
	' '.join(in_dirs),True,pkgdir)

# Testing detection of spin dir same as an input dir
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --filter ' +
do_test('--keepgoing -DWINDOWS --verbose --filter ' + 
	','.join(filters) +	' --dir ' + spindir + ' ' + spindir,
	False,None,gripe,epitaph)

bad_dir = os.path.join(spindir,'src')

# Testing detection of spin dir parent of an input dir
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --filter ' +
do_test('--keepgoing -DWINDOWS --verbose --filter ' + 
	','.join(filters) +	' --dir ' + spindir + ' ' + bad_dir,
	False,None,gripe,epitaph)

# Testing detection of spin dir child of an input dir
#do_test('--keepgoing -DWINDOWS --no-inc --verbose --filter ' +
do_test('--keepgoing -DWINDOWS --verbose --filter ' + 
	','.join(filters) + ' --dir ' + bad_dir + ' ' + spindir,
	False,None,gripe,epitaph)

finis(failures)
