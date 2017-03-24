#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'

import sys, os, atexit, argparse

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.pardir

sys.path.append(os.path.join(top_srcdir,'python'))

from coanlib import *
#if sys.version_info < (2,7):
#	sys.path.append(os.path.join(top_srcdir,'python','argparse-1.2.1'))
#import argparse

set_prog('coan_softlink_tester')

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

parser.add_argument('-r', '--rootname', metavar='ROOTDIR',
	default='scrap',
    help='ROOTDIR is the root directory of the test directory tree, '
		'relative to the current directory. Default \"scrap\"')
    
parser.add_argument('-k', '--keep', action='store_true',
	default=False,
    help='Do not delete the test directory tree or console logs at exit')
            
parser.add_argument('--ver', '--version', action='version',
	version='%(prog)s 0.1 ' + copyright,
    help='Display version information and exit.')

if windows():
    info('This test is not applicable on Windows')
    sys.exit(0)

args = vars(parser.parse_args())
pkgdir = deduce_pkgdir(args)
execdir = deduce_execdir(args)
if not windows():
	os.system('chmod -R +w ' + pkgdir)
do_metrics()

rootname = os.path.abspath(args['rootname'])
failures = 0
infiles = 0
stderr_file = 'stderr.temp.txt'
stdout_file = 'stdout.temp.txt'
set_verbosity(args['verbosity']);
keep = args['keep']

def exithandler():
	''' atexit() cleanup '''
	if not keep:
		file_del('link2options.h');
		file_del('link2scrap');
		del_tree(rootname);
		if not failures:
			file_del(stderr_file)
			file_del(stdout_file)
		
def make_softlink(target,linkname):
	''' Create a softlink *linkname* to target *target* '''
	cmd = 'ln -s {0} {1}'.format(target,linkname)
	ret = os.system(cmd) >> 8
	if ret:
		bail('*** Failed to softlink \"' +\
		 target + '\" as \"' + linkname + '\"')
		 
def copy_test_files_to_scrap(arg, dirname, names):
	''' A callback for os.path.walk.
		Copy all files in the current directory into
		the scrap directory. '''
	global infiles
	for name in names:
		path = os.path.join(dirname,name)
		if os.path.isfile(path):
			(root,ext) = os.path.splitext(path)
			if ext == '.c' or ext == '.cpp' or ext == '.h':
				file_copy_to(path,rootname);
				infiles += 1
				
def populate_scrap():
	''' Copy all files under *pkgdir*/src into
	the scrap directory. '''
	srcdir = pkgdir + '/src'
	os.path.walk(srcdir,copy_test_files_to_scrap,None)
				
def softlink_testfiles(arg,dirname,names):
	''' A callback for os.path.walk. 
	Softlink each file in the current directory to a link
	within subdir1 in the scrap directory '''
	(head,tail) = os.path.split(dirname)
	if tail == 'subdir1':
		return
	for name in names:
		path = os.path.join(dirname,name)
		if os.path.isfile(path):
			(root,ext) = os.path.splitext(name)
			if (ext == '.c' or ext == '.cpp' or ext == '.h'):
				linkname = rootname + '/subdir1/link2' + name
				make_softlink(path,linkname)
				
def make_softlinks_in_scrap():
	''' Softlink each file in the scrap directory to a link
	within subdir1 in the scrap directory '''
	global rootname
	os.path.walk(rootname,softlink_testfiles,None)
	
	
def check_test_result():
	''' Check the result of a test by parsing its stderr output '''
	global failures
	print "infiles = " + str(infiles)
	seekstr = 'info 0x02201: {0} out of {1} input files were reached'.\
		format(infiles,infiles)
	stderr_text = slurp_file(stderr_file)
	if stderr_text.find(seekstr) == -1:
		failures += 1
		error('*** Test Failed! See ' + stderr_file + ' ****')


atexit.register(exithandler)
	
file_del('link2options.h')
file_del('link2scrap')
del_tree(rootname)
os.makedirs(rootname + '/subdir1')
make_softlink(rootname,'link2scrap')

populate_scrap()
if infiles == 0:
	bail('*** No test data found! Bailing out confused ***')

make_softlinks_in_scrap()
make_softlink(rootname + '/subdir1',rootname +'/link2subdir1')
make_softlink(rootname + '/subdir1',rootname + 
	'/subdir1/self_including_link')
make_softlink(rootname +'/options.h','link2options.h')

progress('*** Symlink Test: to process {0} files ***'.format(infiles));
update_test_size_file(infiles)
# Run coan recursively on a dataset in which numerous
# members are symbolic links to other members, to ensure
# that symbolic links are resolved.

#run(execdir + '/coan symbols --once-only --no-inc --verbose --recurse '
run(execdir + '/coan symbols --once-only --verbose --recurse '
	'--filter c,cpp,h link2scrap link2options.h',None,stderr_file)
progress('*** Done ***')
check_test_result()
finis(failures)
