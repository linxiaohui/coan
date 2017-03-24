#ifndef CHEW_H
#define CHEW_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
 *                                                                         *
 *   Contributed by Mike Kinghan, imk@burroingroingjoing.com,              *
 *   derived from the code of Tony Finch                                   *
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
#include "eol.h"
#include <type_traits>
#include <cctype>

/**
 * \file chew.h
 *   This defines `template struct chewer<CharSeq>` and associated tag classes.
 */


namespace chew_mode
{

/// Do not recognize C/C++ comments
static bool const plaintext = false;
/// Recognize C/C++ comments
static bool const cxxtext = true;

/// Types of text
enum class text_type
{
	none,
	whitespace,
	greyspace,
	continuation,
	cxx_comment,
	c_comment,
	string_literal,
	character_literal,
	raw_string_literal,
	header_name,
	code,
	name,
	literal_space,
	punctuation,
	number_space,
	stringify,
	token_paste
};
/// A tag class for selecting a chew mode
struct whitespace
{
    /// Constant type id
	static text_type const id = text_type::whitespace;
	whitespace() {}
};
/// A tag class for selecting a chew mode
struct greyspace
{
    /// Constant type id
	static text_type const id = text_type::greyspace;
	greyspace() {}
};
/// A tag class for selecting a chew mode
struct continuation
{
    /// Constant type id
	static text_type const id = text_type::continuation;
	continuation() {}
};
/// A tag class for selecting a chew mode
struct cxx_comment
{
    /// Constant type id
	static text_type const id = text_type::cxx_comment;
	cxx_comment() {}
};

/// A tag class for selecting a chew mode
struct c_comment
{
    /// Constant type id
	static text_type const id = text_type::c_comment;
	c_comment() {}
};

/// A tag class for selecting a chew mode
struct string_literal
{
    /// Constant type id
	static text_type const id = text_type::string_literal;
	string_literal() {}
};

/// A tag class for selecting a chew mode
struct character_literal
{
    /// Constant type id
	static text_type const id = text_type::character_literal;
	character_literal() {}
};

/// A tag class for selecting a chew mode
struct raw_string_literal
{
    /// Constant type id
	static text_type const id = text_type::raw_string_literal;
	raw_string_literal() {}
};

/// A tag class for selecting a chew mode
struct header_name
{
    /// Constant type id
	static text_type const id = text_type::header_name;
	header_name() {}
};

/// A tag class for selecting a chew mode
struct code
{
    /// Constant type id
	static text_type const id = text_type::code;
	code() {}
};

/// A tag class for selecting a chew mode
struct name
{
    /// Constant type id
	static text_type const id = text_type::name;
	name() {}
};

/// A tag class for selecting a chew mode
struct literal_space
{
    /// Constant type id
	static text_type const id = text_type::literal_space;
	literal_space() {}
};

/// A tag class for selecting a chew mode
struct punctuation
{
    /// Constant type id
	static text_type const id = text_type::punctuation;
	punctuation() {}
};

/// A tag class for selecting a chew mode
struct number_space
{
    /// Constant type id
	static text_type const id = text_type::number_space;
	number_space() {}
};

/// A tag class for selecting a chew mode
struct stringify
{
    /// Constant type id
	static text_type const id = text_type::stringify;
	stringify() {}
};

/// A tag class for selecting a chew mode
struct token_paste
{
    /// Constant type id
	static text_type const id = text_type::token_paste;
	token_paste() {}
};


} // namespace chew_mode

/// An exemplar `chew_mode::whitespace`
chew_mode::whitespace const whitespace;
/// An exemplar `chew_mode::greyspace`
chew_mode::greyspace const greyspace;
/// An exemplar `chew_mode::continuation`
chew_mode::continuation const continuation;
/// An exemplar `chew_mode::cxx_comment`
chew_mode::cxx_comment const cxx_comment;
/// An exemplar `chew_mode::c_comment`
chew_mode::c_comment const c_comment;
/// An exemplar `chew_mode::string_literal`
chew_mode::string_literal const string_literal;
/// An exemplar `chew_mode::character_literal`
chew_mode::character_literal const character_literal;
/// An exemplar `chew_mode::raw_string_literal`
chew_mode::raw_string_literal const raw_string_literal;
/// An exemplar `chew_mode::header_name`
chew_mode::header_name const header_name;
/// An exemplar `chew_mode::code`
chew_mode::code const code;
/// An exemplar `chew_mode::name`
chew_mode::name const name;
/// An exemplar `chew_mode::literal_space`
chew_mode::literal_space const literal_space;
/// An exemplar `chew_mode::punctuation`
chew_mode::punctuation const punctuation;
/// An exemplar `chew_mode::number_space`
chew_mode::number_space const number_space;
/// An exemplar `chew_mode::stringify`
chew_mode::stringify const stringify;
/// An exemplar `chew_mode::token_paste`
chew_mode::token_paste const token_paste;

/** \brief `template struct chewer<CharSeq> is a cursor-like type that is
 *  associated with a character-sequence type upon which it can performs some
 *  routine forms of lexical scanning.
 *
 *  \tparam CharSeq A character-sequence type
 */
template<class CharSeq>
struct chewer : private no_assign
{
    static_assert(traits::is_random_access_char_sequence<CharSeq>::value,">:[");

    /// Type of the character-sequence consumed
    using sequence_type = CharSeq;
    /// Value-type of the character-sequence
    using char_type = typename sequence_type::value_type;
    /// SFINAE type equating to `void` if enabled.
    template<class Mode1, class Mode2>
    using void_if  =
    typename std::enable_if<std::is_same<Mode1,Mode2>::value>::type;

	/** \brief Construct from a `sequence_type` and initial offset
	 *  \param cxx Is C/C++ code to be scanned?
	 *  \param seq A `sequence_type` to be scanned
	 *  \param off Offset in `seq` at which to start scanning
	 *
	 *  The constructor performs no scanning of `seq`
	 */
	explicit chewer(bool cxx, sequence_type & seq, size_t off = 0)
		: _cxx(cxx),_seq(seq),_buf(nullptr),_cur(off),_len(0) {
		sync();
	}

	/** \brief Construct, scanning a given a `sequence_type` from a given offset
	 *  \tparam Mode The type of mode in which to scan
	 *  \param cxx Is C/C++ code to be scanned?
	 *  \param mode A `Mode`
	 *  \param seq A `sequence_type` to be scanned
	 *  \param off Offset in `seq` at which to start scanning
	 *
	 *  The constructor performs scanning of `seq` according to `Mode`
	 */
	template<class Mode>
	explicit chewer(bool cxx, Mode mode, sequence_type & seq, size_t off = 0)
		: _cxx(cxx), _seq(seq),_buf(nullptr),_cur(off),_len(0) {
		scan<Mode>();
	}

	/// Say whether scanning as C/C++ source
	bool cxx() const {
		return _cxx;
	}

    /** \brief Synchronise the object with the associated `sequence_type`
     *
     * Must be called after any operation on the associated `sequence_type`
     * that would invalidate an iterator on `sequence_type`. before further
     * operations on the object.
     */
	void sync() {
		_buf = const_cast<char_type *>(_seq.data());
		_len = _seq.size();
	}

	/// Scan the associated `sequence_type` in a given mode from a given offset.
	template<class Mode>
	void scan(Mode mode, size_t off) {
		cursor(off);
		scan<Mode>();
	}

	/** \brief Adjust position and then scan in a sequence of modes
	 *
	 *  \tparam FirstMode Type of the first sccanning mode
	 *  \tparam OtherModes Types of successive subsequent scanning modes
	 *  \param adjust A signed amount by which the object's scanning
     *  position in the assoiciated `sequence_type` shall be moved
     *  before scanning starts.
     *  \param first_mode A `FirstMode`
     *  \param other_modes A sequence of modes of types per `OtherModes`
     *
     *  The adjusted scanning position is not range checked.
	 */
	template<class FirstMode, class ...OtherModes>
	void scan(	ptrdiff_t adjust,
				FirstMode first_mode, OtherModes... other_modes) {
		adjust >= 0 ? on(adjust) : back(-adjust);
		scan<FirstMode,OtherModes...>();
	}

	/** \brief Scan in a sequence of modes
	 *
	 *  \tparam FirstMode Type of the first sccanning mode
	 *  \tparam OtherModes Types of successive subsequent scanning modes
     *
	 */
	template<class FirstMode, class ...OtherModes>
	void scan() {
		sync();
		consume<FirstMode,OtherModes...>();
	}

	/// \brief Say whether the scanning position is past the end of the
	/// associated `sequence_type`
	bool overshoot(size_t off = 0) const {
		return _cur + off >= _len;
	}

	/// Get the character at an offset from the scanning position
	char_type atoff(ptrdiff_t off) const {
		return _buf[_cur + off];
	}

	/// \brief Get a reference to the character at an offset from the
	/// scanning position
	char_type & atoff(size_t off) {
		return _buf[_cur + off];
	}

	/// `operator[]() const` is an alias for `atoff() const`
	char_type operator[](size_t off) const {
		return atoff(off);
	}

	/// `operator[]()` is an alias for `atoff()`
	char_type & operator[](size_t off) {
		return atoff(off);
	}

	/// Get the scanning position
	size_t cursor() const {
		return _cur;
	}

	/// \brief Get the remaining length of the associated `sequence_type` from
	/// the scanning position.
	size_t remaining() const {
		return _len - _cur;
	}

	/** \brief Set the scanning position.
     *  \param off An offset in the associated `sequence_type`
     *
     *  The scanning position is set = `off` without range-checking
     */
	void cursor(size_t off) {
		sync();
		_cur = off;
	}

	/// Get the character at the scanning position
	char_type curch() const {
		return _buf[_cur];
	}

	/// Get a reference to the character at the scanning position
	char_type & curch() {
		return _buf[_cur];
	}

	/// Assign the scanning position, returning `*this`
	chewer & operator=(size_t off) {
		cursor(off);
		return *this;
	}

	/// Explicitly cast to `size_t`, returning the scanning positition
	explicit operator size_t () const {
		return cursor();
	}

	///@{
	/// \brief Get a [const] reference to the associated `sequence_type`
	sequence_type & buf() {
		return _seq;
	}
	sequence_type const & buf() const {
		return _seq;
	}
	///@}

	/// `operator*() const ` is an alias for `curch() const`.
	char_type operator *() const {
		return curch();
	}
	/// `operator*()` is an alias for `curch()`.
	char_type & operator *() {
		return curch();
	}

	/// Explicitly cast to bool, returning `!overshoot()`
	explicit operator bool () const {
		return !overshoot();
	}

	/** \brief Advance the scanning position an amount.
	 *
	 *  \param n The mount by which the advance the scanning position.
	 *
     * The new position is not range-chacked.
     */
	void on(size_t n) {
		_cur += n;
	}

	/** \brief Retreat the scanning position an amount.
	 *
	 *  \param n The amount to move the scanning position back.
	 *	The new position is not range-chacked.
	 */
	void back(size_t n) {
		_cur -= n;
	}

	/// Increment the scanning position, returning `*this`
	chewer & operator++() {
		++_cur;
		return *this;
	}

	/// Decrement the scanning position, returning `*this`
    chewer & operator--() {
		--_cur;
		return *this;
	}

	/// Advance the scanning position an amount, returning `*this`
	chewer & operator+=(size_t n) {
		on(n);
		return *this;
	}

	/// Retreat the scanning position an amount, returning `*this`
	chewer & operator-=(size_t n) {
		back(n);
		return *this;
	}

	/// `operator()(move,mode)` calls `scan(move,mode)` returning `*this`
	template<class Mode>
	chewer & operator()(ptrdiff_t move, Mode mode) {
		scan(move,mode);
		return *this;
	}

	/// `operator()(mode)` calls `scan(mode)` returning `*this`
	template<class FirstMode, class ...OtherModes>
	typename
	std::enable_if<!std::is_arithmetic<FirstMode>::value,chewer &>::type
	operator()(FirstMode first_mode, OtherModes... other_modes) {
		scan<FirstMode,OtherModes...>();
		return *this;
	}

private:

    /// SFINAE type equating to void if `sequence_type` has an `extend` method.
    template<typename U>
    using if_sequence_is_extensible =
        typename
        std::enable_if<traits::has_extend_method<U>::value>::type;

    /// SFINAE type equating to void if `sequence_type` lacks an `extend` method.
    template<typename U>
    using if_sequence_is_not_extensible =
        typename
        std::enable_if<!traits::has_extend_method<U>::value>::type;

    ///@{
	/// Extend the associated `sequence_type` by reading more input.
	template<typename U = sequence_type>
	void extend(if_sequence_is_extensible<U> * = nullptr) {
		_seq.extend();
		sync();
	}
	template<typename U = sequence_type>
	void extend(if_sequence_is_not_extensible<U> * = nullptr) {}
	///@}

    ///@{
	/*! \brief Extend the associated `sequence_type` by reading more input,
		possibly replacing `skip` characters ahead of the cursor.
	*/
	template<typename U = sequence_type>
	void extend(size_t skip, if_sequence_is_extensible<U> * = nullptr) {
		_seq.extend(skip);
		sync();
	}
	template<typename U = sequence_type>
	void
	extend(
        size_t skip,
        if_sequence_is_not_extensible<U> * = nullptr) {}
    ///@}

    /** \brief Test for a newline-sequence at an offset from the
     * scanning position.
     * \param off Offset in the associated `sequence_type` at which to
     * test for new-line.
     * \return The length of the newline-sequence detected, if any
     * (1 for Unix '\n', 2 for Windows "\r\n"), else 0
	 */
	unsigned eol(size_t off = 0) {
		return ::eol(_seq,_cur + off);
	}

	/// \brief Say whether there is a line-continuation at an offset
    /// from the scanning position.
    /// \param off The offset at which to test
    /// \return True iff there is a line-continuation at offset `off`
	bool line_continues(size_t off = 0) {
		return atoff(off) == '\\' && eol(off + 1);
	}

    //@{
	/// \brief Consume characters satisfying a given mode, without
	/// preliminary `snyc()`
	template<class Mode>
    void_if<Mode,chew_mode::continuation> consume() {
        size_t nl_len;
        for ( 	; curch() == '\\' && (nl_len = eol(1)); on(nl_len + 1)) {
            if (overshoot(nl_len + 1)) {
                extend(nl_len);
            }
        }
	}

    template<class Mode>
    void_if<Mode,chew_mode::whitespace> consume() {
        consume<chew_mode::continuation>();
        for (	;!overshoot() && isspace(curch());
                ++_cur,consume<chew_mode::continuation>()) {}
    }

    template<class Mode>
    void_if<Mode,chew_mode::number_space> consume() {
        consume<chew_mode::continuation>();
        size_t mark = _cur;
        if (!overshoot() && curch() == '.') {
            ++_cur;
        }
        if (overshoot() || !isdigit(curch())) {
            _cur = mark;
            return;
        }
        for (++_cur	;!overshoot() && (
                        isalnum(curch()) || curch() == '_' || curch() == '+' ||
                            curch() == '-' || curch() == '.');
                ++_cur,consume<chew_mode::continuation>()) {}
    }

    template<class Mode>
    void_if<Mode,chew_mode::name> consume() {
       consume<chew_mode::continuation>();
       if (!overshoot() && (isalpha(curch()) || curch() == '_')) {
           for (++_cur; !overshoot() && (isalnum(curch()) || curch() == '_');
                ++_cur,consume<chew_mode::continuation>()){}
        }
    }


    template<class Mode>
    void_if<Mode,chew_mode::punctuation> consume() {
        consume<chew_mode::continuation>();
        for (	;!overshoot() && ispunct(curch()) &&
            (curch() != '_' && curch() != '\'' && curch() != '\"');
                ++_cur,consume<chew_mode::continuation>()) {}
    }

    template<class Mode>
    void_if<Mode,chew_mode::stringify> consume() {
        consume<chew_mode::continuation>();
        _cur += !overshoot() && curch() == '#';
    }

    template<class Mode>
    void_if<Mode,chew_mode::token_paste> consume() {
        size_t mark = _cur;
        consume<chew_mode::continuation>();
        if ((_cur += !overshoot() && curch() == '#') > mark) {
            consume<chew_mode::continuation>();
            if (!overshoot() && curch() == '#') {
                ++_cur;
            } else {
                _cur = mark;
            }
        }
    }

    template<class Mode>
    void_if<Mode,chew_mode::cxx_comment> consume() {
        consume<chew_mode::continuation>();
        size_t mark = _cur;
        if (!overshoot() && curch() == '/') {
            ++_cur;
            consume<chew_mode::continuation>();
            if (overshoot() || curch() != '/') {
                _cur = mark;
                return;
            }
            for(++_cur;
                !overshoot();
                    ++_cur,consume<chew_mode::continuation>()){};
        }
    }

    template<class Mode>
    void_if<Mode,chew_mode::c_comment> consume() {
        consume<chew_mode::continuation>();
        size_t mark = _cur;
        if (overshoot() || curch() != '/') {
            return;
        }
        ++_cur;
        consume<chew_mode::continuation>();
        if (overshoot() || curch() != '*') {
            _cur = mark;
            return;
        }
        bool closing = false;
        ++_cur;
        consume<chew_mode::continuation>();
        for (	; !overshoot();
                closing = (curch() == '*'),
                ++_cur,consume<chew_mode::continuation>()) {
            if (curch() == '/') {
                if (closing) {
                    ++_cur;
                    return;
                }
            } else {
                size_t nl_len = eol();
                if (nl_len && overshoot(nl_len + 1)) {
                    extend(nl_len);
                }
            }
        }
        eof_in_comment();
    }

    template<class Mode>
    void_if<Mode,chew_mode::string_literal> consume() {
        consume<chew_mode::continuation>();
        consume_enclosed_string<'\"','\"'>();
    }

    template<class Mode>
    void_if<Mode,chew_mode::header_name> consume() {
        consume<chew_mode::continuation>();
        size_t mark = _cur;
        consume_enclosed_string<'<','>'>();
        if (_cur == mark) {
            consume_enclosed_string<'\"','\"'>();
        }
    }

    template<class Mode>
    void_if<Mode,chew_mode::character_literal> consume() {
        consume<chew_mode::continuation>();
        if (overshoot() || curch() != '\'') {
            return;
        }
        size_t mark = _cur;
        ++_cur;
        consume<chew_mode::continuation>();
        bool escape = false;
        for (	; !overshoot();	++_cur,consume<chew_mode::continuation>()) {
            if (curch() == '\'') {
                if (!escape) {
                    ++_cur;
                    return;
                }
                escape = false;
            } else if (curch() == '\\') {
                escape = !escape;
            } else {
                escape = false;
            }
        }
        _cur = mark;
    }

    template<class Mode>
    void_if<Mode,chew_mode::raw_string_literal> consume() {
        consume<chew_mode::continuation>();
        if (overshoot() || curch() != 'R') {
            return;
        }
        size_t mark = _cur;
        ++_cur;
        consume<chew_mode::continuation>();
        if (overshoot() || curch() != '\"') {
            _cur = mark;
            return;
        }
        ++_cur;
        consume<chew_mode::continuation>();
        for (	; !overshoot(); ++_cur,consume<chew_mode::continuation>()) {
            if (!isgraph(curch()) || curch() == '\\') {
                _cur = mark;
                return;
            }
            if (curch() == '(') {
                break;
            }
        }
        ++_cur;
        consume<chew_mode::continuation>();
        for (	; !overshoot(); ++_cur,consume<chew_mode::continuation>()) {
            if (curch() == ')') {
                break;
            }
            size_t nl_len = eol();
            if (nl_len != 0) {
                extend(nl_len);
            }
        }
        ++_cur;
        consume<chew_mode::continuation>();
        for (; !overshoot(); ++_cur,consume<chew_mode::continuation>()) {
            if (!isgraph(curch()) || curch() == '\\') {
                break;
            }
            if (curch() == '\"') {
                ++_cur;
                return;
            }
        }
        missing_terminator(mark,'\"');
    }

    template<class Mode>
    void_if<Mode,chew_mode::greyspace> consume() {
        if (!_cxx) {
            consume<chew_mode::whitespace>();
            return;
        }
        for (	;!overshoot(); ) {
            consume<chew_mode::whitespace>();
            size_t mark = _cur;
            consume<chew_mode::cxx_comment>();
            if (_cur != mark) {
                break;
            }
            consume<chew_mode::c_comment>();
            if (_cur == mark) {
                break;
            }
        }
    }

    template<class Mode>
    void_if<Mode,chew_mode::code> consume() {
        for (	;!overshoot(); ++_cur) {
            consume<chew_mode::greyspace>();
            consume<chew_mode::character_literal>();
            consume<chew_mode::string_literal>();
            consume<chew_mode::raw_string_literal>();
        }
    }

    template<class Mode>
    void_if<Mode,chew_mode::literal_space> consume() {
        consume<chew_mode::character_literal,
                chew_mode::string_literal,
                chew_mode::raw_string_literal>();
    }
    ///@}

    ///@{
    /// \brief Consume characters satisfying a sequence of modes,
    /// without preliminary `sync()`
	template<class First, class Next, class ...Rest>
	typename std::enable_if<sizeof ...(Rest) == 0>::type
	consume();

	template<class First, class Next, class ...Rest>
	typename std::enable_if<sizeof ...(Rest) != 0>::type
	consume();
	///@}


	/// Consume characters between delimiting characters
	template<char_type Opener, char_type Closer>
	void consume_enclosed_string();

	/// Diagnose a missing terminator character
	void missing_terminator(size_t off, char_type missing);
	/// Diagnose end of file in C-comment.
	void eof_in_comment();

	/// Scanning C/C++ source?
	bool _cxx;
	/// The sequence that is consumed.
	sequence_type & _seq;
	/// Pointer to the data controlled by `_seq`
	char_type *_buf;
	/// The scanning position in `_buf`.
	size_t _cur;
	/// The length of the data at `_buf'
	size_t _len;

};

template<class CharSeq>
template<
    typename chewer<CharSeq>::char_type Opener,
    typename chewer<CharSeq>::char_type Closer
>
void chewer<CharSeq>::consume_enclosed_string()
{
	if (overshoot() || curch() != Opener) {
		return;
	}
	size_t mark = _cur++;
	consume<chew_mode::continuation>();
	bool escape = false;
	for (	; !overshoot(); ++_cur,consume<chew_mode::continuation>()) {
		if (curch() == Closer) {
			if (!escape) {
				++_cur;
				return;
			}
			escape = false;
		} else if (curch() == '\\') {
			escape = !escape;
		} else {
			escape = false;
		}
	}
	if (overshoot()) {
		missing_terminator(mark,Closer);
	}
}

template<class CharSeq>
template<class First, class Next, class ...Rest>
typename std::enable_if<sizeof ...(Rest) == 0>::type
chewer<CharSeq>::consume() {
	for (	;!overshoot(); ) {
		size_t mark = _cur;
		consume<First>();
		consume<Next>();
		if (_cur == mark) {
			break;
		}
	}
}

template<class CharSeq>
template<class First, class Next, class ...Rest>
typename std::enable_if<sizeof ...(Rest) != 0>::type
chewer<CharSeq>::consume() {
	for (	;!overshoot(); ) {
		size_t mark = _cur;
		consume<First>();
		consume<Next,Rest...>();
		if (_cur == mark) {
			break;
		}
	}
}

#endif /* EOF*/
