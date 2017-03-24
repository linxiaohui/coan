#ifndef DATASET_H
#define DATASET_H
#pragma once
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

#include "prohibit.h"
#include "file_tree.h"

/** \file dataset.h
 *   This defines class `dataset`.
 */

/** \brief `struct dataset` encapsulates the tree of input files to be processed
 *   by coan.
 */
struct dataset : private no_copy {

	/** \brief Encapsulates the selection of files for processing
     *
	 *   `struct selector` encapsulates the selection of files for
	 *   processing that satisfy the `--filter` option.
	*/
	struct selector {
		/** \brief Construct from a filter string.
         *
		 *  Explicitly construct given a filter string.
		 *
		 *  \param extensions An empty string or else a comma-punctuated
		 *  list of the extensions of files that are to be selected.
		 */
		explicit selector(std::string const & extensions = std::string());

		/** \brief Say whether a file is selected for processing.
		 *   \param  filename    The name of the file to be tested.
		 *   \return True iff the file is selected.
		 */
		bool operator()(std::string const & filename);

		/// Get the number of files so far selected.
		unsigned files() const {
			return _files;
		}
	private:
		/// A vector of the file extensions that are to be selected.
		std::vector<std::string> _filter_extensions;
		/// The number of files so far selected.
		unsigned _files;
	};

	/** \brief `struct driver` encapsulates traversal of an input dataset to
	 *   select and process files.
	 */
	struct driver : file_tree::traverser {

		/// Get the number of files reached in the dataset.
		unsigned done_files() const {
			return _done_files;
		}

		/// Get the number of files abandoned due to errors.
		unsigned error_files() const {
			return _error_files;
		}

		/// Process a file in the dataset.
		void at_file(std::string const & filename);

	private:
		/// The number of files reached.
		unsigned _done_files = 0;
		/// The number of files abandoned due to errors.
		unsigned _error_files = 0;
	};

	/** \brief Specify the filtering of files in the `dataset`.
	 *  \param extensions An empty string or else a comma-punctuated
	 *      list of the extensions of files that are to be selected.
	 */
	static void set_filter(std::string extensions) {
		_selector_ = selector(extensions);
	}
	/** \brief Add files to the `dataset`.
     *
	 *  \param	path	Name of file or directory to be included in the
	 *       `dataset`.
     *
	 *  If `path` is a file that satisfies any `--filter` option
	 *  it is added to the `dataset`.
     *
	 *  If `path` is a directory then files recursively beneath it
	 *  that satisfy any `--filter` option are added to the `dataset`.
    */
	static void add(std::string const & path);

	/// Traverse the dataset processing the selected files.
	static void traverse() {
		_ftree_.traverse(_driver_);
	}

	/// Get the number of files in the `dataset`.
	static unsigned files() {
		return _selector_.files();
	}

	/// Get the number of files reached by traversal of the `dataset`
	static unsigned done_files() {
		return _driver_.done_files();
	}

	/** \brief Get the number of files abandoned due to errors in traversal of
	 *	the `dataset`.
	 */
	static unsigned error_files() {
		return _driver_.error_files();
	}

private:

	/// The `selector` for including files in the `dataset`
	static selector _selector_;
	/// The `driver` for traversing `dataset`
	static driver _driver_;
	/// The tree of input files
	static file_tree _ftree_;
};

#endif /* EOF*/
