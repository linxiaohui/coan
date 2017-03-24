#!/usr/bin/python

# Module coanlib provides utility routines for the coan test harnesses

import sys, os, subprocess, re, shutil, string, shlex, time, atexit, \
	errno

__progress = 0
__info = 1
__warning = 2
__error = 3
__fatal = 4
__severities_by_word = {
	'progress' : __progress,
	'info' : __info,
	'warning' : __warning,
	'error' : __error,
	'fatal' : __fatal
}
__severities_by_num = {
	__progress : 'progress',
	__info : 'info',
	__warning : 'warning',
	__error : 'error',
	__fatal : 'fatal'
}

__prog = '<unknown program>'
__verbosity = __progress
__time_file = None
__test_size_file = None
 		
def get_prog():
	''' Get the current program name '''
	return __prog
	
def set_prog(prog):
	''' Set the current program name '''
	global __prog
	__prog = prog
	
	
def get_verbosity():
	''' Get the current verbosity level by keyword '''
	return __severities_by_num[__verbosity]
	
def set_verbosity(verbosity_keyword):
	''' 
	Set the verbosity level by keyword.
	Messages with a lower verbosity level will be suppressed
	'''
	global __verbosity
	__validate_verbosity(verbosity_keyword)
	__verbosity = __severities_by_word[verbosity_keyword]
	
def progress(msg):
	''' Issue a progress message '''
	__report(__progress,msg)
	
def info(msg):
	''' Issue an informational message '''
	__report(__info,msg)
	
def warn(msg):
	''' Issue a warning message '''
	__report(__warning,msg)
	
def error(msg):
	''' Issue an error message '''	
	__report(__error,msg)
	
def fatal(msg):
	''' Issue a fatal error message '''
	__report(__fatal,msg)

def finis(failures):
	''' End a test according to the number of failures'''
	if (failures):
		file_del(__get_time_file())
		file_del(__get_test_size_file())
	sys.exit(failures)	
			
def bail(msg, exitcode = 1):
	''' Optionally issue a fatal error message and exit with a given system code'''
	fatal(msg)
	file_del(__get_time_file())
	file_del(__get_test_size_file())		
	sys.exit(exitcode)
	

def __get_time_file():
	''' Set the name of the test timing file if unset'''
	global __time_file
	if not __time_file:
		prog = get_prog()
		pkgdir = deduce_pkgdir()
		__time_file = os.path.join(pkgdir,'test_coan',prog +'.time.txt') \
			if prog != '<unknown program>' else None
	return __time_file
	
def __get_test_size_file():
	''' Set the name of the test size file if unset'''
	global __test_size_file
	if not __test_size_file:
		prog = get_prog()
		pkgdir = deduce_pkgdir()
		__test_size_file = os.path.join(pkgdir,'test_coan',prog +'.size.txt') \
			if prog != '<unknown program>' else None
	return __test_size_file

def __compute_runtime(time_file):	
	''' Parse the coan timing file and add the entries to
	compute the total runtime of the process(es)
	recorded there '''
	lines = slurp_lines(time_file)
	seconds = 0.0
	for line in lines:
		try:
			seconds = seconds + float(line)
		except:
			pass
	return seconds
	
def __compute_test_size(size_file):	
	''' Parse the coan test size file and add the entries to
	compute the total size of the tests(es)
	recorded there '''
	lines = slurp_lines(size_file)
	test_files = 0
	for line in lines:
		try:
			test_files = test_files + int(line)
		except:
			pass
	return test_files

def __report(severity,msg):
	''' Issue a message with a given severity '''
	if severity >= __verbosity:
		severity_keyword = __severities_by_num[severity]
		outstr = __prog + ": " +	severity_keyword + ": " + \
		msg + '\n'
		if severity < __warning:
			sys.stdout.write(outstr)
		else:
			sys.stderr.write(outstr)

def __validate_verbosity(keyword):
	''' Validate a verbosity level keyword '''
	if keyword not in __severities_by_word:
		bail("Unknown severity keyword: \"" + keyword + "\"")
		
def windows():
	''' Say whether the host OS is Windows '''
	return os.name == 'nt'
	
def fopen(file,mode):
	if mode != 'r' and mode != 'w' and mode != 'a':
		bail('*** Unknown file open mode\'' + mode + '\' ***')   
	try:
		return open(file,mode)
	except IOError as error:
		modestr = 'reading' if mode == 'r' else 'writing'
		bail('*** Cannot open file \"' + file + '\"' + " for " + \
			modestr + ': ' + error.strerror + ' ***')
			
def make_path(path):
	''' 
	Try to create the directory specified by a path,
	bailing on failiure
	'''
	try:
		os.makedirs(path)
	except OSError as error:
		bail('*** Failed to create directory \"' + path + '\": ' + \
			error.strerror)

def del_tree(rootdir):
	''' Try to delete a directory tree, if it exists, 
	bailing on failure '''
	try:
		if os.path.isdir(rootdir):
			shutil.rmtree(rootdir)
	except OSError as error:
		bail('*** Failed to delete directory \"' + rootdir + '\": ' + 
			error.strerror)
		
def file_copy(src,dest):
	''' Try to copy a file to another, bailing on failure '''
	try:
		shutil.copyfile(src,dest)
	except IOError:
		bail('*** Failed to copy file \"' +\
		 src + '\" as \"' + dest + '\"')
		 
def file_copy_to(src,dest):
	''' Try to copy a file, bailing on failure '''
	try:
		shutil.copy(src,dest)
	except IOError:
		bail('*** Failed to copy file \"' +\
		 src + '\" -> \"' + dest + '\"')

def file_del(filename):
	''' Try to delete a file if it exists, bailing on failure '''
	try:
		os.remove(filename)
	except OSError, e:
		if e.errno != errno.ENOENT: # errno.ENOENT = no such file or directory
			bail('*** Failed to delete file \"' + filename + '\"')
				
def slurp_command(cmd, with_stderr = False):
	''' Return the output of a command as a string '''
	words = cmd.split()
	output = subprocess.check_output(words) if not with_stderr \
		else subprocess.check_output(words, stderr=subprocess.STDOUT)
	return output
	
def slurp_file(file):
	''' Return the contents of as a string '''
	fh = fopen(file,'r')
	data = fh.read()
	fh.close()
	return data
	
def slurp_lines(file):
	''' Return the contents of as a list of lines '''
	fh = fopen(file,'r')
	lines = fh.readlines()
	fh.close()
	return lines
	
def __timing_metrics_enabled():
	return os.getenv('TIMING_METRICS') == '1'

def __do_timing_metrics():
	if not __timing_metrics_enabled():
		return False
	time_version = slurp_command('/usr/bin/time --version', True)
	return time_version.find('GNU') != -1 

DO_TIMING_METRICS = __do_timing_metrics()
	
def run(cmd,
	stdout_file = None,stderr_file = None,stdin_file = None, timing = DO_TIMING_METRICS):
	''' 
	Run a command optionally specifying
	files to capture stdout and stderr and whether timing is required
	Return the exit code of the command.
	'''
	time_file = __get_time_file() if timing else None
	if windows():
		cmd = re.sub(r'\\',r'\\\\',cmd)
	elif time_file:
		cmd = "/usr/bin/time -f \"%e\" --quiet -a -o " \
			+ time_file  + ' ' + cmd 
	args = []
	try:
		args = shlex.split(cmd)
	except ValueError:
		args = cmd.split()
	stdout_fh = None
	stderr_fh = None
	stdin_fh = None
	if stdout_file:
		stdout_fh = fopen(stdout_file,'w')
	if stderr_file:
		stderr_fh = fopen(stderr_file,'w')
	if stdin_file:
		stdin_fh = fopen(stdin_file,'r')
		stdin_fh = fopen(stdin_file,'r')
	progress('*** Running: ' + cmd)
	syscode = subprocess.call(args,
		stdout=stdout_fh,stderr=stderr_fh,stdin=stdin_fh)
	if stdout_fh:
		stdout_fh.close()
	if stderr_fh:
		stderr_fh.close()
	if stdin_fh:
		stdin_fh.close()
	return syscode
	
def run_noerr(cmd):
	retcode = run(cmd)
	if retcode:
		bail('*** Command failed: \"' + cmd + '\": ' + \
			os.strerror(retcode))
	
def is_exe(path):
	''' Say whether a path is an executable file '''
	return os.path.isfile(path) and os.access(path, os.X_OK)
	
def deduce_pkgdir(args = {}):
	''' Deduce the actual coan package directory given the commandline args '''
	pkgdir = None
	try:
		pkgdir = args['pkgdir']
	except:
		pass
	if not pkgdir:
		pkgdir = os.getenv('COAN_PKGDIR')
		if not pkgdir:
			pkgdir = os.pardir
	return os.path.abspath(pkgdir)

def deduce_execdir(args = {}):
	''' Deduce the actual directory containing the coan
	executable given the commandline args '''
	execdir = None
	try:
		execdir = args['execdir']
	except:
		pass
	if not execdir:
		execdir = 'src'
	builddir = os.getenv('COAN_BUILDDIR')
	if not builddir:
		builddir = deduce_pkgdir(args)
	execdir = os.path.join(builddir,execdir)
	return os.path.abspath(execdir)

def compute_runtime(time_files = [__get_time_file()]):
	''' Parse the coan timing files and add the entries to
	compute the total runtime of the process(es)
	recorded there. If the list of timing files includes any
	but the the program's own timing file then the current
	program's timing file is removed from the list.
	'''
	time_file = __get_time_file()
	if time_files != [time_file]:
		try:
			posn = time_files.index(time_file)
			del time_files[posn]
		except:
			pass
	seconds = 0.0
	for time_file in time_files:
		if os.path.exists(time_file):
			seconds = seconds + __compute_runtime(time_file)
	return seconds
	
def update_test_size_file(nfiles):
	''' Append the number of files a test is to process to the
	coan test size file '''
	size_file = __get_test_size_file();
	if size_file:
		fh = fopen(size_file,'a')
		fh.write(str(nfiles) + '\n')
		fh.close()	
	
def compute_test_size(size_files = [__get_test_size_file()]):
	''' Parse the coan test size files and add the entries to
	compute the total size of the test(es)
	recorded there. If the list of size files includes any
	but the the program's own size file then the current
	program's size file is removed from the list.
	'''
	size_file = __get_test_size_file()
	if size_files != [size_file]:
		try:
			posn = size_files.index(size_file)
			del size_files[posn]
		except:
			pass
	test_files = 0
	for size_file in size_files:
		if os.path.exists(size_file):
			test_files = test_files + __compute_test_size(size_file)
	return test_files
	
def report_runtime(time_files = [__get_time_file()]):
	''' Display the total runtime recorded in the 
	coan timing files. If the list of timing files
	includes any but the current program's timing file
	then the current program's timing file is removed from
	the list and the report is also written to the current 
	program's timing file.
	
	Return the total runtime computed.
	'''
	tot_time = compute_runtime(time_files)
	info('runtime in coan: ' + str(tot_time) + ' secs.')
	time_file = __get_time_file()
	if time_file not in time_files:
		fh = fopen(time_file,'w')
		fh.write(str(tot_time) +'\n')
		fh.close()
	return tot_time
		
def report_test_size(size_files = [__get_test_size_file()]):
	''' Display the total test size recorded in the 
	coan test size files. If the list of size files
	includes any but the current program's size file
	then the current program's size file is removed from
	the list and the report is also written to the current 
	program's size file.
	
	Return the total test size computed.
	'''
	tot_size = compute_test_size(size_files)
	if tot_size != 0:			
		info('Coan processed ' + str(tot_size) + ' input_files.')
		size_file = __get_test_size_file()
		if size_file not in size_files:
			fh = fopen(size_file,'w')
			fh.write(str(tot_size) + '\n')
			fh.close()
	return tot_size
	
def report_metrics(	time_files = [__get_time_file()], 
					size_files = [__get_test_size_file()]):
	tot_size = report_test_size(size_files)
	if tot_size == 0:
		return
	tot_time = report_runtime(time_files);
	av_time = float(tot_time) / float(tot_size)
	info('Average processing time per input file: {:2.6f} secs.'\
		.format(av_time))
		
def measure_runtime():
	''' Initialize coan runtime measuring '''
	time_file = __get_time_file()
	if time_file:
		file_del(time_file)
		atexit.register(report_runtime,[__get_time_file()])
		
def measure_test_size():
	''' Initialize coan test_size measuring '''
	size_file = __get_test_size_file()
	if size_file:
		file_del(size_file)
		atexit.register(report_test_size,[__get_test_size_file()])
		
def do_metrics():
	''' Initailize coan test metrics '''
	time_file = __get_time_file()
	size_file = __get_test_size_file()
	if time_file and size_file:
		file_del(time_file)
		file_del(size_file)
		atexit.register(
			report_metrics,
			[__get_time_file()],
			[__get_test_size_file()])

if __name__ == "__main__":
	print slurp_command('/usr/bin/time --version', True)

	
