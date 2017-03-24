#ifndef PATH_H
#define PATH_H
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
#include "platform.h"
#include <vector>
#include <string>
#include <stdexcept>

/** \file path.h
 *   This file defines `struct path`.
 */

/** \brief Encapsulates a filesystem path.
 *
 *   `struct path` controls a string representing an absolute or
 *   relative pathname and provides queries and operations on it.
 *
 *   A `path` contains a cursor that can be used to maintain
 *   and modify an index into the elements of the path.
 *
 *   \tparam  Delim  The character that delimits elements of a path.
 */
template<char Delim>
struct path {

	path() = default;

	/// Explicitly construct a `path` from a string.
	explicit path(std::string const & path)
		:   _path(path) {
		analyse();
	}

	/// Assign from a string.
	path & operator=(std::string const & str) {
		cut();
		_path = str;
		analyse();
		posn() = 0;
		return *this;
	}

	/// Equality
	bool operator==(path const & other) const {
		return _path == other._path;
	}

	/// Inequality
	bool operator!=(path const & other) const {
		return !(_path == other._path);
	}

	/// Less-than operator.
	bool operator<(path const & other) const {
		return _path < other._path;
	}

	/** \brief Get the number of elements in the `path`
     *
	 *   \return The number of elements in the `path`.
     *
	 *   The returned value is the number of `Delim`
	 *   characters in the `path`, even if there is a trailing
	 *   `Delim`.
	 */
	size_t elements() const {
		return _elements.size();
	}

	/// Get the `path` as a string.
	std::string const & str() const {
		return _path;
	}

	/// Get the length of the `path`.
	size_t length() const {
		return _path.length();
	}

	/** \brief Return a path element by index.
	 *  \param which Index to the requested element
	 *  \return The indexed path element as a string.
	 *
	 *  If `index` is out of range an `std::out_of_range`
	 *  exception is thrown.
	 */
	std::string element(size_t which) const {
		if (which >= elements()) {
			throw std::out_of_range("Out of range in path::element(size_t)");
		}
		element_pos const & element = _elements[which];
		return element.second == 0 ?
		       std::string(1,Delim) :
		       _path.substr(element.first,element.second);
	}

	/// Get the path element at the cursor.
	std::string cur_element() const {
		return element(posn());
	}

	/// The subscript operator is equivalent to `element(which)`
	std::string operator[](size_t which) const {
		return element(which);
	}

	/** \brief Say whether the path consists of an initial subsequence
	 *	of the elements of another.
     *
	 *	\param  other The `path` to be compared with `*this`.
	 *	\return True if this path consists of an initial subsequence
	 *	of the elements of `other`.
	 */
	bool is_prefix_of(path const & other) const {
		path common = common_prefix(*this,other);
		return *this == common;
	}

	/** \brief Get a sub-sequence of the path's elements as a string.
	 *  \param  start   Index of the first element of the requested
	 *           sub-sequence.
	 *  \param  len Number of elements in the requested subsequence.
     *
	 *  \return The requested sub-sequence as a string. If `start` is
	 *  out of range an empty string is returned. If there are fewer
	 *  than `len` elements at `start` then all remaining are
	 *  returned.
	 */
	std::string
	segment(size_t start = 0, size_t len = std::string::npos) const {
		element_pos section = get_section(start,len);
		return section.first != std::string::npos ?
		       _path.substr(section.first,section.second) : std::string();
	}

	/** \brief Append a string to the path.
     *
	 *  \param  str The string to append.
     *
	 *  A trailing `Delim` character is appended if not already present,
	 *	then `str`. `str` is not tested non-empty or for a leading `Delim`.
	 */
	void push_back(std::string const & str) {
		if (length() && _last != Delim) {
			_path += Delim;
		}
		_path +=  str;
		analyse();
	}

	/// \brief Remove the last element of the path, if any.
	void pop_back() {
		size_t parts = elements();
		if (parts) {
			size_t cut = _elements[parts - 1].first;
			cut -= parts-- > 1;
			_path.erase(cut);
			analyse();
		}
	}

	/** \brief Prepend a string to the path.
     *
	 *  \param  str The string to prepend.
     *
	 *  A leading `Delim` char is prepended if not already present,
	 *	then `str`. `str` is not tested non-empty or for a trailing `Delim`.
	 */
	void push_front(std::string const & str) {
		if (_first != Delim) {
			_path.insert(0,1,Delim);
		}
		_path.insert(0,str);
		analyse();
	}

	/// \brief Remove the first element of the path, if any.
	void pop_front() {
		size_t parts = elements();
		if (parts) {
			size_t start = _elements[0].first;
			size_t len = _elements[0].second;
			len += parts-- > 1;
			_path.erase(start,len);
			analyse();
		}
	}


	/** \brief Get the concatenation of the path with a string.
     *
	 *  \param  str The string to be appended.
	 *  \return A new path that results from appending `str` to a copy
	 *  of `*this` with an intervening `Delim`.
     *
	 *	The new path inherits the cursor of `*this`
	 */
	path operator+(std::string const & str) const {
		path p(*this);
		p.append(str);
		return p;
	}

	/** \brief Get the concatenation of the path with another.
     *
	 *  \param  rhs The path to be appended.
	 *  \return A new path that results from appending `rhs` to a copy
	 *   of `*this` with an intervening `Delim`.
     *
	 *  The new path inherits the cursor of *this
	 */
	path operator+(path const & rhs) const {
		return *this + rhs.str();
	}

	/// `operator+=(str)` is equivalent to `push_back(str)`
	path & operator+=(std::string const & str) {
		push_back(str);
		return *this;
	}

	/// `operator+=(path)` is equivalent to `push_back(path.str())`
	path & operator+=(path const & rhs) {
		push_back(rhs.str());
		return *this;
	}

	/** \brief Insert a string into the path.
	 *  \param  after The index of the element after which to insert.
	 *  \param  str The string to insert.
     *
	 *  If `after` < `elements() - 1`, then `str` is inserted
	 *  following element `after` and delimited from the next
	 *  element by a `Delim` character. Otherwise the member
	 *  function is equivalent to `append(str)`.
	 */
	void insert(size_t after,std::string const & str) {
		if (after < elements() - 1) {
			element_pos element = _elements[after];
			after = element.first + element.second + 1;
			_path.insert(after,str + Delim);
			analyse();
		} else {
			push_back(str);
		}
	}

	/** \brief Remove a segment from the path.
	 *  \param  start   The index of the first element to remove.
	 *  \param  len     The number of elements to remove.
     *
	 *  If `start` is out of range then nothing is done. If there
	 *  are fewer than `len` elements at `start` then all remaining
	 *  are removed.
	 */
	void cut(size_t start = 0, size_t len = std::string::npos) {
		element_pos section = get_section(start,len);
		if (section.first != std::string::npos) {
			if (section.first + section.second < _path.length()) {
				++section.second;
			} else if (section.first > 0) {
				--section.first;
			}
			_path.erase(section.first,section.second);
			analyse();
		}
	}

	/** \brief Correct anomalies in the path.
     *
	 *  All sequences of > 1 `Delim` are reduced to 1 and any trailing
	 *  `Delim` is removed.
	 */
	void rectify() {
		std::string match(2,Delim);
		for (size_t posn ; (posn = _path.find(match)) != std::string::npos; ) {
			_path.erase(posn + 1);
		}
		size_t len = _path.length();
		if (len > 1 && _path[len - 1] == Delim) {
			_path.resize(len - 1);
		}
		analyse();
	}

	///@{
	/** \brief Get a [const] reference to the path's cursor.
     *
	 *  \return A [const] reference to the cursor.
     *
	 *  The \c cursor can be used to maintain and modify an
	 *   index into the path's elements.
     *
	 *  No operation on a path except assignment modifies its
	 *  cursor, even if the operation leaves the cursor out of
	 *  range.
	 */
	int & posn() {
		return _cursor;
	}
	int const & posn() const {
		return _cursor;
	}
	///@}

	/// \brief Set the cursor to index the final element, or to 0 if the `path`
	/// is empty.
	/// \return The new cursor position.
	int to_end() {
		_cursor = elements() ? elements() - 1 : 0;
		return _cursor;
	}

	/** \brief Get the common initial prefix of two paths
     *
	 *  \param  lhs The first path to compare
	 *  \param  rhs The second path to compare
	 *  \return A `path` comprising the common initial elements of
	 *       `lhs` and `rhs`.
	 */
	static path common_prefix(path const & lhs, path const & rhs) {
		size_t count = std::min(lhs.elements(),rhs.elements());
		path p;
		for (size_t ndx = 0; ndx < count; ++ndx) {
			if (rhs[ndx] == lhs[ndx]) {
				p += rhs[ndx];
			}
		}
		return p;
	}

private:

	/** \brief Type of an element or section locator in a `path`.
     *
	 *   The type of an element or section locator in a `path`,
	 *   consisting of the index and length of the element.
	 */
	using element_pos = std::pair<size_t,size_t>;

	/// Analyse the `path` to determine its composition.
	void analyse() {
		_elements.resize(0);
		size_t start = 0;
		size_t end;
		for (   ; (end = _path.find(Delim,start)) != std::string::npos;
		        start = end + 1) {
			_elements.push_back(element_pos(start,end - start));
		}
		if (start < _path.length()) {
			_elements.push_back(element_pos(start,_path.length() - start));
		}
		if (_path.length()) {
			_first = _path[0];
			_last = _path[_path.length() - 1];
		} else {
			_first = _last = 0;
		}
		if (_cursor >= elements()) {
            to_end();
		}
	}

	/** \brief Get the location of a section of the `path`.
	 *  \param  start   The index of the first element of the required section.
	 *  \param  len     The number of elements in the required section. Defaults
	 *           to `std::string::npos` to request all elements from `start`.
	 *  \return An `element_pos` that delimits the required section.
	 */
	element_pos
	get_section(size_t start, size_t len = std::string::npos) const {
		if (len && start < elements()) {
			len = std::min(len,_path.length() - start);
			element_pos const & first = _elements[start];
			if (start + len <= elements()) {
				element_pos const & last = _elements[(start + len) - 1];
				len = (last.first + last.second) - first.first;
				return len ?
				       element_pos(first.first,len) : element_pos(first.first,1);
			}
			return element_pos(first.first,len);
		}
		return element_pos(std::string::npos,std::string::npos);
	}

	/// The string representation of the `path`.
	std::string _path;
	/// The structure of the path as a sequence of `element_pos`.
	std::vector<element_pos> _elements;
	/// The cursor of the path.
	int _cursor = 0;
	/// The first character of the `path`.
	char _first = 0;
	/// The last character of the `path`.
	char _last = 0;
};

/*! \brief Get a path by concatenation of a string with a path.

    \tparam Delim The host system's path delimiter.

    \param str The lefthand operand
    \param path The righthand operand
    \return The path resulting from concatenation of `str` and `path`

    The `cursor` of the new path is initialized to 0
*/
template<char Delim>
path<Delim> operator+(std::string const & str, path<Delim> const & path)
{
	return path<Delim>(str) + path;
}

/// Alias `path_t` according to host system path delimiter
using path_t = path<PATH_DELIM>;


#endif /* EOF*/
