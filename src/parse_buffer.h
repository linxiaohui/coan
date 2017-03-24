#ifndef PARSE_BUFFER_H
#define PARSE_BUFFER_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2013 Mike Kinghan, imk@burroingroingjoing.com		   *
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

#include <string>
#include "eol.h"

/** \file parse_buffer.h
 *
 *	This file defines `struct parse_buffer`
 */

/** \brief `struct parse_buffer` is a polymorphic base for classes representing
 *       possibly extensible lines of parsed text.
 */
struct parse_buffer
{
    /// Value-type of `parse_buffer`
    using value_type = char;

	/// Default constuctor
	parse_buffer() = default;

	/// Destructor
	virtual ~parse_buffer(){};

	/// Explicitly construct from an `std::string`
	explicit parse_buffer(std::string const & str)
	: _text(str){}

	/// Assign another `parse_buffer`
	parse_buffer & operator=(parse_buffer const &) = default;

	/// Assign an `std::string`
	parse_buffer & operator=(std::string const & str) {
		_text = str;
		return *this;
	}

	/// Equality
	bool operator==(parse_buffer const & other) const {
		return _text == other._text;
	}

	/// Inequality operator
	bool operator!=(parse_buffer const & other) const {
		return _text != other._text;
	}

	/// Get the length of the `parse_buffer`
	size_t size() const {
		return _text.size();
	}

	///@{
	/// \brief Get a [const] reference to the underlying std::string
	std::string & str() {
		return _text;
	}
	std::string const & str() const {
		return _text;
	}
	///@}

	/// Explicitly cast to `std::string`
	explicit operator std::string const &() const {
		return str();
	}

	///@{
	/// \brief Get a [const] pointer to the data
	char const * data() const {
		return _text.data();
	}
	char * data() {
		return const_cast<char *>(_text.data());
	}
	///@}

	/// Get a substring of the `parse_buffer`.
	std::string substr(size_t start, size_t len = std::string::npos) const {
		return _text.substr(start,len);
	}

	/// Say whether an offset is out of tange
	bool overshoot(size_t off = 0) const {
		return off >= size();
	}

	/// Get the character indexed by an offset. Not range-checked
	char at(size_t off) const {
		return _text[off];
	}

	/// Get a reference to the character an offset. Not range-checked.
	char & at(size_t off) {
		return _text[off];
	}
	/// Get the character at an offset. Not range checked.
	char operator[](size_t off) const {
		return at(off);
	}

	/// Get a reference to the character at an offset. Not range checked.
	char & operator[](size_t offset) {
		return at(offset);
	}

	/// Empty the `parse_buffer`.
	void clear() {
		_text.clear();
	}

	/** \brief Say whether there is a newline sequence at an offset.
     *
	 *	\param offset An offset into the `parse_buffer`
     *
	 *	\return 0 to indicate the there is no newline sequence at
	 *	`offset`; otherwise the length of the newline sequence at `offset`
	 */
	unsigned eol(size_t offset) const {
		return ::eol(_text,offset);
	}

	/** \brief Extend the string, perhaps.
     *
	 *	\return The number of characters by which the string is
	 *	extended.
     *
	 *	The virtual member function may either do nothing or it may attempt
	 *	to extend the string by appending characters from some opaque source.
	 */
	virtual size_t extend() {
		return 0;
	}

	/** \brief Test whether an extension of the string is pending at an offset.
     *
	 *	\param off The offset at which to test.
     *
	 *	The virtual member function shall return a value to indicate
	 *	whether a sequence of characters at offset `off` betokens
	 *	a logical extension of the string already in the buffer.
     *
	 *	\return 0 if no extension is betokened. Otherwise the length
	 *	of the character sequence that betokens an extension.
	 */
	virtual size_t extension_pending(size_t off) const {
		return 0;
	}

	/** \brief Extend the string, perhaps.
     *
	 *	\param skip The number of characters currently ahead of the cursor
	 *		that constitute a sequence betokening an extension. This
	 *		should be the value returned immediately beforehand by
	 *		\c extension_pending()
     *
	 *	\return The net number of characters by which the string is
	 *	extended.
     *
	 *	The virtual member function shall do nothing if `skip` is 0.
	 *	Otherwise it may either do nothing or it may attempt to extend the
	 *	string by appending characters from some opaque source, possibly
	 *	replacing up to `skip` characters that are ahead of the cursor.
	 */
	virtual size_t extend(size_t skip) {
		return 0;
	}

protected:

    /// The data
	std::string _text;

};

#endif // EOF
