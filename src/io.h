#ifndef IO_H
#define IO_H
#pragma once
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

#include "filesys.h"
#include <string>
#include <cassert>

/** \file io.h
 *   This defines `struct io`
 */


/// \brief Manages input and output file handling.
struct io {

	/// Nominal filename for the standard input stream.
	static char const * const _stdin_name_; // = "[stdin]"


	/** \brief Read the name of a source file from `stdin`.
     *
	 *  Filenames may contain spaces if quoted.
     *
	 *  \return A pointer to the source filename in static storage,
	 *   if a valid filename is read; `nullptr` if no filename is read.
	 */
	static std::string read_filename();

	/** \brief Open a named file for reading or writing.
     *
	 *  \param file The name of the file to be opened
	 *  \param mode	The read or write mode in which to open the file,
	 *  `"r"` or `"w"`.
	 *  \return  Handle to opened file on success, else `nullptr`.
	 */
	static FILE * open_file(std::string const & file, const char *mode);

	/** \brief Open an input file and the appropriate output file.
     *
	 *  \param  fname	The name of the input file to open.
	 *      An empty string, "", denotes the standard input
	 */
	static void open(std::string const & fname);

	/** \brief Finalise the current pairing of source input and processed
	 *   output, if any.
     *
	 *   \param error 0 if no errors were raised in
	 *      processing the input file that is to be closed, else the
	 *      reason code of the error that aborted processing.
     *
	 *  If a file is associated with either of these streams, it is closed.
	 *  The output file replaces the input file, if `error` == 0
	 *  and the `--replace` option is in force, and is also backed up
	 *  beforehand, if the `--backup`  option is in force. If the function is
	 *  called before any input has been opened it is a no-op
	 */
	static void close(unsigned error);

	/// Close the current source file.
	static void close_input();

	/// Reinitialize the module.
	static void top();

	/// Get the name of the current source file.
	static std::string in_file_name() {
		return _in_filename_;
	}

	/// Get a pointer to the output stream.
	static std::ostream * output() {
		return _output_;
	}

	/// Get a pointer to the input stream.
	static std::istream * input() {
		return _input_;
	}

	/** \brief Set the directory in which to output a spin.
	 *	\param	optarg	The commandline option specifing the spin directory
	 *		name.
	 *	The function stores the absolute path of the spin directory.
	 */
	static void set_spin_dir(char const *optarg);

	/** \brief Set a path prefix (of input files) that will be assumed to
	 *   match the name of the spin directory.
	 *	\param	optarg	The commandline option specifing the spin prefix.
     *
	 *	The function stores the absolute path of the spin prefix.
	 */
	static void set_spin_prefix(char const *optarg);

	/// Get the name of operative spin directory.
	static std::string const & spin_dir() {
		return _spin_dir_;
	}

	/// Say whether there is any spin directory.
	static bool spin() {
		return !_spin_dir_.empty();
	}

private:

	/**	\brief Replace the current input source file with the temporary output
	 *  file.
     *
	 *  Replace the current input source file with the temporary output
	 *  file, when the `--replace` option is in force.
     *
	 *  The input source file has either been deleted or renamed to a backup
	 *  when this function is called, depending on whether the `--backup`
	 *  option is in force.
	 */
	static void replace_infile();

	/** \brief Generate a backup name for the current input source file.
     *
	 *  Generate a backup name for the current input source file,
	 *  when the `--replace` and `--backup` option are in force.
     *
	 *  The backup filename is saved in static storage.
     *
	 *  \param filename The name of the source file to be backed up.
	 */
	static void backup_infile(const char *filename);

	/** \brief Backup the current input source file.
     *
	 *  Backup the current input source file when the `--backup` option is
	 *  in force.
     *
	 *  The current source file is renamed with a generated backup filename.
	 */
	static void backup_infile();

	/** \brief Delete the current input source file.
     *
	 *  Delete the current input source file, preparatory to replacing it
	 *  with the corresponding output file.
	 */
	static void delete_infile();


	/// Open the output file.
	static void open_outfile();

	/** \brief Open an output stream for the current input file.
	 *
	 *  The output stream is `cout` unless input source
	 *  files are to be replaced with output. Otherwise the
	 *  output stream is opened on a temporary output file.
	 */
	static void open_output();

	/** \brief Create an output file in the spin directory corresponding to
	 *  an input file.
	 */
	static void make_spinfile();

	/** \brief Create a temporary file to which output will be written.
     *
	 *   Create a temporary file to which output will be written
	 *   preparatory to replacing the input file with this output
	 *   when `--replace` is in effect.
	 */
	static void make_tempfile();

	/** \brief Generate a filename for backing up an input file.
     *
	 *  Generate a filename for backing up an input file
	 *  when `--replace` and `--backup` are in effect.
	 *  \param  filename The name of the input file to be backed up.
	 */
	static void make_backup_name(std::string const & filename);

	/// The name of the current source file
	static std::string _in_filename_;
	/// The output stream
	static std::ostream * _output_;
	/// The input stream
	static std::istream * _input_;
	/// The input file
	static std::filebuf _infile_;
	/// File permissions mask of input file, in case file is replaced
	static fs::permissions _in_out_permissions_;
	///  Current output filename, if needed
	static std::string _out_filename_;
	/// Backup filename, if needed
	static std::string _bak_filename_;
	/// The output file
	static std::filebuf _outfile_;
	/// Name of directory in which to output a spin
	static std::string _spin_dir_;
	/// Path prefix assumed to match the spin directory
	static std::string _spin_prefix_;
};

#endif /* EOF*/
