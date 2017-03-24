#!/usr/bin/python

copyright = 'Copyright (c) 2012-2013 Michael Kinghan'
import sys, os, glob

top_srcdir = os.getenv('COAN_PKGDIR')
if not top_srcdir:
	top_srcdir = os.path.abspath(os.pardir)
	
sys.path.append(os.path.join(top_srcdir,'python'))

from coanlib import set_prog, report_metrics

set_prog("coan_test_timer")
time_path_pat = os.path.join(top_srcdir,'test_coan','*.time.txt')
time_files = glob.glob(time_path_pat)
size_path_pat = os.path.join(top_srcdir,'test_coan','*.size.txt')
size_files = glob.glob(size_path_pat)
report_metrics(time_files,size_files)

