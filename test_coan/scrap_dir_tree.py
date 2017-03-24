#!/usr/bin/python

# Create a random structure of randomly named directories

import sys, string, random, os.path, shutil

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.pardir

sys.path.append(os.path.join(top_srcdir,'python'))

import argparse
from coanlib import *

copyright = 'Copyright (c) 2012 Michael Kinghan'

set_prog('scrap_dir_tree')


parser = argparse.ArgumentParser(
	prog=get_prog(),
    formatter_class=argparse.RawDescriptionHelpFormatter,
    description='Create a random structure of randomly named directories',
    epilog='BREADTH and DEPTH must be greater than 0')

parser.add_argument('-w', '--wipe', action='store_true',
    help='Remove the tree if it already exists')
    
parser.add_argument('-r', '--rootname', metavar='DIRNAME',
	default='scrap',
    help='DIRNAME is the root directory of the tree. Default \"scrap\".'
		' Must not already exist unless --wipe is specified')

parser.add_argument('-b', '--breadth', metavar='BREADTH',
	required=True,type=int,
    help='There will be at most BREADTH subdirectories in a directory')

parser.add_argument('-d', '--depth', metavar='DEPTH',
	required=True,type=int,
    help='The structure will contain at most DEPTH levels')

parser.add_argument('-s', '--seed', metavar='SEED',type=int,
    help='Seed the pseudo random number generator with the number SEED.'
    ' If not specified use implementation default')

parser.add_argument('-v', '--verbosity', metavar='LEVEL',
	default='progress',
    help='Display diagnostics with severity >= LEVEL, where '  
    	'LEVEL = \'progress\', \'info\', \'warning\', \'error\' or '
    	'\'fatal\'. Default = \'progress\'')

parser.add_argument('files', metavar='FILE', nargs='+',
    help='Any specified files will be copied '
		'into each created directory')
    
parser.add_argument('--ver', '--version', action='version',
	version='%(prog)s 0.1 ' + copyright,
    help='Display version information and exit.')

args = vars(parser.parse_args())
set_verbosity(args['verbosity']);
alphabet = string.letters + string.digits + '_'
scrapfiles = args['files']

depth = args['depth']
if depth == 0:
	bail('--depth must be > 0')
progress('Make max %s levels of directories' % depth)
	
breadth = args['breadth']
if breadth == 0:
	bail('--breadth must be > 0')
progress('Make max %s directories per level' % breadth)

seed = args['seed']
if seed != None:
	random.seed(seed)
else:
	progress('No pseudo-random seed given: '
		'using implementation default');
		
wipe = args['wipe']
rootname = args['rootname']
if os.path.isdir(rootname):
	if wipe:
		del_tree(rootname)
		
	else:
		bail('*** Root directory \"' + rootname + '\" already exists');

progress("Creating scrap directories");

def gen_scrap_name(maxlen):
	''' Return a random string drawn from the characters in *alphabet*
	with a maximum length '''
	len = random.randint(0,maxlen)
	randstr = ''.join(random.choice(alphabet) for x in range(len))
	return randstr

def gen_dirs(path,breadth,maxdepth):
	'''
	Recursively generate a random directory with a given
	root and breadth and depth limits.
	'''  
	if not os.path.isabs(path):
		path= os.path.abspath(path)
	dirs_made = 0
	files_made = 0
	if breadth > 0 and maxdepth > 0:
		maxlen = 128 - len(path)
		for dir in range(0,breadth):
			next_path = path
			next_path += '/'
			if maxlen < 1:
				break
			dirname = gen_scrap_name(maxlen)
			next_path += dirname
			if not os.path.isdir(next_path):
				make_path(next_path)
				dirs_made += 1
				progress('Made directory: \"' + next_path + '\"')
				for scrapfile in scrapfiles:
					(head,tail) = os.path.split(scrapfile)
					destfile = next_path + '/' + tail
					file_copy(scrapfile,destfile)
					files_made += 1
				(new_dirs,new_files) = \
					gen_dirs(next_path,breadth - 1,maxdepth - 1)
				dirs_made += new_dirs
				files_made += new_files
	return (dirs_made,files_made);

(dirs_made,files_made) = gen_dirs(rootname,breadth,depth)
progress('Made {0} directories, {1} files'.format(dirs_made, files_made))
sys.exit(0)
