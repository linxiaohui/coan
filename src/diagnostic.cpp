/***************************************************************************
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
 *   Neither the name of Mike Kinghan nor the names of its contributors    *
 *   may be used to endorse or promote products derived from this software *
 *   without specific prior written permission.                            *
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

#include "diagnostic.h"
#include "dataset.h"
#include "if_control.h"
#include "line_despatch.h"
#include "options.h"
#include "io.h"
#include <iostream>
#include <iomanip>

/** \file diagnostic.cpp
 *   This file implements types supporting the production of diagnostics.
 */

using namespace std;

unsigned diagnostic_base::_infos_(0);
unsigned diagnostic_base::_warnings_(0);
unsigned diagnostic_base::_errors_(0);
unsigned diagnostic_base::_abends_(0);
unsigned diagnostic_base::_error_directives_generated_(0);
unsigned diagnostic_base::_error_directives_operative_(0);

list<diagnostic_base::ptr> diagnostic_base::_queue_;

diagnostic_base::diagnostic_base(std::string const & severity_name,
                                 severity level, unsigned id,
                                 bool in_code_only)
	: _gagged(false)
{
	unsigned reason = code(level,id);

	if (options::diagnostic_gagged(reason)) {
		/* This message is filtered out. Don't output*/
		_gagged = true;
		return;
	}
	_text << options::prog_name() << ": ";
	if (!options::got_opts()) {
		_text << "while parsing options: ";
	}
	if (!io::in_file_name().empty()) {
		_text << io::in_file_name() << ": ";
	}
	if (line_despatch::cur_line().num()) {
		_text << "line " << line_despatch::cur_line().num() << ": ";
	}
	_text << severity_name << ' ' << "0x" << setfill('0')
	      << hex << setw(5) << reason << ": " << dec;
}

void diagnostic_base::despatch() const throw(unsigned)
{
	count();
	if (!text().empty()) {
		cerr << text() << '\n';
	}
	if (level() == severity::abend) {
		exit(exitcode());
	}
	if (level() == severity::error) {
		throw(unsigned(*this));
	}
}

void diagnostic_base::epilogue()
{
	size_t infiles = dataset::files();
	char const * diagnostic_status = "";
	int ret = exitcode();
	if_control::top();
	io::top();

	if (_abends_) {
		diagnostic_status = " ABNORMALLY";
	} else if (_errors_) {
		diagnostic_status = " with errors";
	} else if (_warnings_) {
		diagnostic_status = " with warnings";
	} else if (_infos_) {
		diagnostic_status = " with remarks";
	}

	progress_summary_all_done() << "Completed"
		<< diagnostic_status << ", exit code 0x" << setfill('0') << setw(2) <<
		hex << ret << dec << emit();
	if (infiles) {
		info_summary_files_reached() <<
		 dataset::done_files() << " out of " << infiles <<
		 " input files were reached; "
		 << (infiles - dataset::done_files()) << " files were not reached"
		 << emit();
		info_summary_files_dropped() <<
			 (dataset::done_files() - dataset::error_files())
			 <<  " out of " << dataset::done_files() <<
			 " files reached were valid; " << dataset::error_files() <<
			 " were abandoned due to parse errors" << emit();
		if (infiles == dataset::done_files() && dataset::error_files() == 0) {
			if (line_despatch::lines_suppressed()) {
				info_summary_summary_dropped_lines() <<
					"Input lines were dropped" << emit();
			}
			if (line_despatch::lines_changed()) {
				info_summary_summary_changed_lines() <<
					 "Input lines were changed" << emit();
			}
			if (_error_directives_generated_) {
				warning_summary_summary_errored_lines() <<
					"Input lines were changed to #error directives"
					<< emit();
			}
			if (_error_directives_operative_) {
				warning_summary_summary_error_output() <<
				   "Unconditional #error directives are operative"
				   << emit();
			}
		}
	}
}

int diagnostic_base::exitcode()
{
	unsigned severities = 0;
	unsigned summaries = 0;
	if (line_despatch::lines_suppressed()) {
		summaries |= 16;
		++_infos_;
	}
	if (line_despatch::lines_changed()) {
		summaries |= 32;
		++_infos_;
	}
	if (_error_directives_generated_) {
		summaries |= 64;
		++_warnings_;
	}
	if (_error_directives_operative_) {
		summaries |= 128;
		++_warnings_;
	}
	if (_infos_) {
		severities |= 1;
	}
	if (_warnings_) {
		severities |= 2;
	}
	if (_errors_) {
		severities |= 4;
	}
	if (_abends_) {
		severities |= 8;
	}
	return severities < 4 ? severities | summaries : severities;
}

void diagnostic_base::flush_all()
{
	for(    ; !_queue_.empty(); _queue_.pop_front()) {
		_queue_.begin()->get()->despatch();
	}
}

// EOF
