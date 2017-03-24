#!/usr/bin/python

# class_TestCase.py: Provides class TestCase
# Copyright (c) 2012 Michael Kinghan

import re, sys, os, string

sys.path.append("../python")

import coanlib
from coanlib import progress, info, error, bail, set_prog, get_prog, \
	set_verbosity, fopen, windows, is_exe, slurp_command, slurp_file, \
	slurp_lines, file_copy, file_del

 
class TestCase:
	''' class TestCase encapulates a coan test case '''
	def __init__(self,pkgdir,wordsize,
				executable,testfiles,monkey_args = '', coan_rc = 0):
		self.__pkgdir = pkgdir
		self.__wordsize = wordsize
		self.__executable = executable
		self.__test_files = testfiles
		self.__monkey_args = monkey_args;
		self.__test_args = ''
		self.__test_syscode = coan_rc
		self.__test_exact = True
		self.__tally_lines = False
		self.__test_altfiles = []
		self.__no_output = False
		self.__redirect_op = ''
		self.__test_cmd = ''
		self.__stdout_file = ''
		self.__stderr_file = ''
		self.__skip = False
		self.__testdir = os.path.join(self.__pkgdir,'test_coan')
		if self.__monkey_args:
			self.__test_args = self.__monkey_args + ' --keepgoing'
		if len(self.__test_files) and not self.__monkey_args:
			self.__parse_test_file()
		
	@property
	def skip(self):
		''' Say whether a test is to be skipped '''
		return self.__skip
		
	def set_pkgdir(self,pkgdir):
		''' Set the coan package directory '''
		self.__pkgdir = pkgdir
		
	def set_testfiles(self,testfiles):
		''' Set the list of test files '''
		self.__test_files = testfiles
		
	def set_wordsize(self,wordsize):
		''' Set the host wordsize '''
		self.__wordsize = wordsize
		
	def set_executable(self,executable):
		''' Set the coan exacutable '''
		self.__executable = executable
		
	def set_args(self,args):
		''' Set the coan arguments '''
		self.__test_args = args
		
	def set_expected_syscode(self,syscode):
		''' Set the expected system code for the test '''
		self.__test_syscode = syscode
		
	def set_syscode_exact(self,exact):
		''' Specify whether the expected system code is to match
		actual system code exactly or merely or-wise '''
		self.__exact = exact
		
	def set_altfiles(self,altfiles):
		''' Set the list of alternate test files,
		(when the nominal test file is not itself the actual test file)
		'''
		self.__test__altfiles = altfiles
		
	def set_tally_lines(self,tally):
		''' Specify that the number of output lines from the test
		must match the number of input lines '''
		self.__tally_lines = tally
		
	def set_redirect(self,redirect):
		''' Specify that an input file is to be redirected to coan
		via stdin.
		'''
		self.__redirect = redirect		
								
	def run(self):
		''' Run the test, returning True = success, False = failure '''
		self.__compose_test_command()
		syscode_correct = False
		output_correct = True
		diagnostics_correct = True
		testfiles_str = ' '.join(self.__test_files)
		msg = '*** Running test file(s) '\
			'\"{0}\"'.format(testfiles_str)
		progress(msg)
		msg = '*** ARGS: {0}'.format(self.__test_args)
		progress(msg);
		if len(self.__test_altfiles):
			progress('*** ALTFILES: ' + ' '.join(self.__test_altfiles))
		if self.__test_exact:
			msg = '*** SYSCODE: = {0:#4x}'.format(self.__test_syscode)
			progress(msg);
		else:
			msg = '*** SYSCODE: match '\
				'{0:#04x}'.format(self.__test_syscode)
			progress(msg)
		if self.__no_output:
			progress("*** NO-OUTPUT")
		if self.__redirect_op == '<':
			progress("*** REDIRECT")
		coanlib.run(self.__test_cmd,self.__stdout_file,self.__stderr_file)
		actual_syscode = self.__parse_syscode_from_stderr()
		if self.__test_exact:
			syscode_correct = (actual_syscode == self.__test_syscode)
		else:
			syscode_correct = \
				((actual_syscode & self.__test_syscode) == \
					actual_syscode)
		if not self.__monkey_args:
			output_correct = True if self.__no_output \
				else self.__verify_output(
					self.__test_files[0] + '.expect',
					self.__test_files[0] +'.output')
			if output_correct and \
				os.path.exists(self.__test_files[0] + '.stderr.expect'):
				diagnostics_correct = self.__verify_output(
					self.__test_files[0] + '.stderr.expect',
					self.__test_files[0] +'.stderr')
		if not syscode_correct:
			msg = '*** FAILED test file(s) \"{0}\". '\
				'Unexpected system code {1}'.\
				format(' '.join(self.__test_files),actual_syscode)
			error(msg)
		if not output_correct:
			msg = '*** FAILED test file(s) \"{0}\". '\
				'Unexpected output'.format(' '.join(self.__test_files))
			error(msg)
		if not diagnostics_correct:
			msg = '*** FAILED test file(s) \"{0}\". '\
				'Unexpected stderr'.format(' '.join(self.__test_files))
			error(msg)
		if syscode_correct and output_correct and diagnostics_correct:
			if not self.__monkey_args:
				progress("*** PASSED");
			return True;
		return False;
	
	def __parse_test_file(self):
		''' Parse a test file for the parameters of a test case '''
		text = slurp_file(self.__test_files[0])
		match = self.__re_args.search(text)
		if match:
			self.__test_args = match.group(1)
			if self.__re_discard.search(self.__test_args):
				self.__tally_lines = True
		else:
			self.__skip = True
		match = self.__re_syscode_parm.search(text)
		if match:
			self.__test_syscode = match.group(1)
			match = self.__re_syscode.match(self.__test_syscode)
			if match:
				self.__test_exact = True
				self.__test_syscode = eval(match.group(1))
		if self.__re_no_output.search(text):
			self.__no_output = True
		if windows():
			if self.__re_not_windows.search(text):
				progress('*** Skipping non-Windows-compatible '
					'test file \"' + self.__test_files[0] + '\"')
				self.__skip = True
				return
		match = self.__re_wordsize.search(text)
		if match:
			wordsize = int(match.group(1))
			if wordsize != self.__wordsize:
				msg = '*** Skipping test file \"{0}\" because '\
					'WORDSIZE={1} is not '\
					'satisfied'.format(self.__test_files[0],wordsize)
				progress(msg)
				self.__skip = True
				return
		match = self.__re_altfiles.search(text)
		if match:
			self.__test_altfiles = match.group(1).split()
		else:
			match = self.__re_altfiles.search(text)
			if match:
				self.__redirect_op = '<'
					
	def __verify_output(self,expected_out_file,actual_out_file):
		''' Verify that the actual output file from a test matches
		the expected output file '''
		expected_lines = slurp_lines(expected_out_file)
		actual_lines = slurp_lines(actual_out_file)
		if self.__tally_lines:
			if len(expected_lines) != len(actual_lines):
				return False
		expected_words = []
		for line in expected_lines:
			expected_words += line.split()
		actual_words = []
		for line in actual_lines:
			actual_words += line.split()
		if len(actual_words) != len(expected_words):
			return False
		for i in range(len(actual_words)):
			if not self.__match_words(actual_words[i],expected_words[i]):
				return False
		return True
	
	def __compose_test_command(self):
		''' Compose the coan command for the test '''
		test_files = []
		if len(self.__test_altfiles):
			for file in self.__test_altfiles:
				if not os.path.isabs(file):
					file = re.sub('^\./','',file)
					test_files.append(
						os.path.join(self.__pkgdir,'test_coan',file))
		else:
			test_files = self.__test_files;
		if windows():
			for ndx, file in enumerate(test_files):
				test_files[ndx] = \
					string.replace(file,'/','\\\\')
		if self.__test_args.find('--replace') != -1:
			self.__stdout_file = 'coan.stdout.tmp'
			self.__stderr_file = 'coan.stderr.tmp'
		else:
			self.__stdout_file = self.__test_files[0] + '.output'
			self.__stderr_file = self.__test_files[0] + '.stderr'
		self.__test_cmd = '{0} {1} --verbose {2}{3}'\
			.format(self.__executable,self.__test_args,
				self.__redirect_op,' '.join(test_files))
			
	def __parse_syscode_from_stderr(self):
		''' Parse the system code return from a test from its
		stderr output '''
		text = slurp_file(self.__stderr_file)
		match = self.__re_exitcode.search(text)
		if match:
			syscode = int(match.group(1),16)
			return syscode
		bail('*** Could not parse system code from ' +
			self.__stderr_file + '***')

	def __match_words(self,actual_word,expected_word):
		if actual_word == expected_word:
			return True
		expected_word = expected_word.replace('{CWD}',self.__testdir)
		if windows():
			actual_word = actual_word.replace('\\','/')
			expected_word = expected_word.replace('\\','/')
		return True if actual_word == expected_word else False;
	
	# Private class members
	__re_args = re.compile('\/\*\*ARGS:\s*(.+?)\s*\*\/')
	__re_discard = \
		re.compile('(--discard\s+blank|--discard\s+comment|-kb|-kc)')
	__re_syscode_parm = re.compile('\/\*\*SYSCODE:\s*(.+?)\s*\*\/')
	__re_syscode = re.compile('^\s*=\s*(.+)$')
	__re_no_output = re.compile('\/\*\*NO-OUTPUT\s*\*\/')
	__re_not_windows = re.compile('\/\*\*NOT-WINDOWS\s*\*\/')
	__re_wordsize = re.compile('\/\*\*WORDSIZE=(\d+)\s*\*\/')
	__re_redirect = re.compile('\/\*\*REDIRECT\s*\*\/')
	__re_altfiles = re.compile('\/\*\*ALTFILES:\s*(.+?)\s*\*\/')
	__re_exitcode = re.compile('exit code (0x[a-fA-F0-9]+)')

