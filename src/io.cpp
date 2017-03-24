/***************************************************************************
 *   Copyright (C) 2004, 2006 Symbian Software Ltd.                        *
 *   All rights reserved.                                                  *
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed originally by Mike Kinghan, imk@burroingroingjoing.com    *
 *                                                                         *
 *   Redistribution and use in source and binary forms, with or without    *
 *   modification, are permitted provided that the following conditions    *
 *   are met:                                                              *
 *                                                                         *
 *   Redistributions of source code must retain the above copyright        *
 *   notice, this list of conditions and the following disclaimer.         *
 *                                                                         *
 *   Redistributions in binary form must reproduce the above copyright     *
 *   notice, this list of conditions and the following disclaimer in the   *
 *   documentation and/or other materials provided with the distribution.  *
 *                                                                         *
 *   Neither the name of Symbian Software Ltd. nor the names of its        *
 *   contributors may be used to endorse or promote products derived from  *
 *   this software without specific prior written permission.              *
 *                                                                         *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS   *
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT     *
 *   LIMITED TO, THE IMPLIED WARRANTIES OF  MERCHANTABILITY AND FITNESS    *
 *   FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE        *
 *   COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,   *
 *   INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,  *
 *   BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS *
 *   OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED    *
 *   AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,*
 *   OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF *
 *   THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH  *
 *   DAMAGE.                                                               *
 *                                                                         *
 **************************************************************************/
#include "io.h"
#include "options.h"
#include "path.h"
#include "directive.h"
#include "diagnostic.h"
#include "symbol.h"
#include <fstream>
#include <iostream>

/** \file io.cpp
 *   This file implements `struct io`
 */

using namespace std;

string io::_in_filename_;
ostream * io::_output_(nullptr);
fs::permissions io::_in_out_permissions_(-1);
filebuf io::_outfile_;
istream * io::_input_(nullptr);
filebuf io::_infile_;
string io::_out_filename_;
string io::_bak_filename_;
char const * const io::_stdin_name_ = "[stdin]";
string io::_spin_dir_;
string io::_spin_prefix_;

void io::top() {
	line_despatch::top();
	_in_filename_.resize(0);
}

void io::delete_infile()
{
	if (remove(_in_filename_.c_str())) {
		abend_cant_delete_file() <<
			 "Cannot remove file \"" << _in_filename_ << '\"' << emit();
	}
}

string io::read_filename()
{
	string filename;
	int ch;
	bool quoted;
	ch = getchar();
	/* Skip whitespace on stdin*/
	for (	; ch != EOF && isspace(ch); ch = getchar()) {};
	if (ch == EOF) {
		return filename;
	}
	quoted = ch == '\"';
	if (quoted) {
		for (ch = getchar() ; ch != EOF && ch != '\"'; ch = getchar()) {
			if (isspace(ch) && ch != ' ') {
				abend_illegal_filename() <<
					 "Illegal whitespace in _input_ filename: \""
					 << filename << "..." << emit();
			}
			filename += ch;
		}
		if (ch == EOF) {
			abend_eof_in_filename() <<
				"A quoted _input_ filename is unterminated: \""
				<< filename << "..." << emit();
		}
	} else {
		for (	; ch != EOF && !isspace(ch); ch = getchar()) {
			filename += ch;
		}
	}
	return filename;
}

void io::make_tempfile()
{
	path_t path(fs::real_path(_in_filename_));
	path.pop_back();
	path.push_back("coan_out_XXXXXX");
	_out_filename_ = fs::tempname(path.str());
	if (_out_filename_.empty()) {
		abend_no_tempfile() << "Cannot create temporary file" << emit();
	}
}

void io::replace_infile()
{
	if (rename(_out_filename_.c_str(),_in_filename_.c_str())) {
		abend_cant_rename_file() <<
			 "Cannot rename file \"" << _out_filename_ << "\" as \""
			 << _in_filename_ << '\"' << emit();
	} else if (_in_out_permissions_ != -1) {
		_in_out_permissions_ =
			fs::set_permissions(_in_filename_,_in_out_permissions_);
		assert(_in_out_permissions_ != -1);
	}
}

void io::make_backup_name(string const & filename)
{
	_bak_filename_ = filename;
	do {
		_bak_filename_ += options::backup_suffix();
	} while(fs::obj_type(_bak_filename_) != fs::OBJ_NONE);
}

void io::backup_infile()
{
	make_backup_name(_in_filename_);
	if (rename(_in_filename_.c_str(),
	           _bak_filename_.c_str())) {
		abend_cant_rename_file() <<
			 "Cannot rename file \"" << _in_filename_ << "\" as \""
			 << _bak_filename_ << '\"' << emit();
	}
}

void io::open_outfile()
{
	_outfile_.open(_out_filename_.c_str(),ios_base::out);
	if (!_outfile_.is_open()) {
		abend_cant_open_output() << "Can't open " <<
			 _out_filename_ << " for writing" << emit();
	}
	_output_ = new ostream(&_outfile_);
}

void io::open_output()
{
	delete _output_;
	if (spin()) {
		make_spinfile();
		open_outfile();
	} else if (options::replace()) {
		make_tempfile();
		open_outfile();
	} else {
		_output_ = new ostream(cout.rdbuf());
	}
}

void io::close(unsigned error)
{
	delete _input_, _input_ = nullptr;
	delete _output_, _output_ = nullptr;
	_infile_.close();
	_outfile_.close();
	if (!error) {
		if (options::replace() && !spin()) {
			if (options::backup_suffix().length()) {
				backup_infile();
			} else {
				delete_infile();
			}
			replace_infile();
		}
	} else {
		if (!options::keep_going()) {
			exit(diagnostic_base::exitcode());
		}
	}
	top();
}

void io::open(string const & fname)
{
	_in_filename_ = fname;
	delete _input_;
	if (fname != _stdin_name_) {
		_in_out_permissions_ =
			options::replace() ? fs::get_permissions(fname) : -1;
		_infile_.open(fname.c_str(),ios_base::in);
		if (!_infile_.is_open()) {
			abend_cant_open_input() << "Can't open " <<
				_in_filename_ << " for reading" << emit();
		}
		_input_ = new istream(&_infile_);
		assert(_input_->good());
	} else {
		_input_ = new istream(cin.rdbuf());
	}
	open_output();
	line_despatch::top();
	symbol::per_file_init();
	if (options::list_once_per_file()) {
		directive_base::erase_all();
	}
}

void io::set_spin_dir(char const *optarg)
{
	_spin_dir_ = fs::abs_path(optarg);
}

void io::set_spin_prefix(char const *optarg)
{
	_spin_prefix_ = fs::real_path(optarg);
}

void io::make_spinfile()
{
	path_t spin_filename(_spin_dir_);
	path_t in_filename(_in_filename_);
	assert(fs::is_absolute(_in_filename_));
	if (_spin_prefix_.empty()) {
		spin_filename += in_filename.segment(1);
	} else {
		static path_t prefix(_spin_prefix_);
		spin_filename += in_filename.segment(prefix.elements());
	}
	size_t parts = spin_filename.elements();
	string dir = spin_filename.segment(0,parts - 1);
	fs::make_dir(dir);
	_out_filename_ = spin_filename.str();
}


/* EOF*/
