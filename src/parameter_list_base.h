#ifndef PARAMETER_LIST_BASE_H
#define PARAMETER_LIST_BASE_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2007-2013 Mike Kinghan, imk@burroingroingjoing.com      *
 *   All rights reserved.                                                  *
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
#include "chew.h"
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

/** \file parameter_list_base.h
 *   This file defines `struct innards::parameter_list_base`
 */

// Forward decl
struct symbol;

namespace innards {

/** \brief `template struct innards::parameter_list_base<Tag>` generically
 *   defines a common interface of types representing a formal
 *   parameter list or an argment list.
 */
struct parameter_list_base {

	/** \brief Constructor for `n` parameters
     *
     *  \param n The number of parameters.
     *
	 *	If `n` > 0, the list of placeholder parameters `$1,...,$n`
	 *	is constructed.
	 */
	explicit parameter_list_base(size_t n = 0);

	/** \brief Explicitly construct a given a `chewer<CharSeq>`.
	 *
	 *  \tparam CharSeq A character-sequence type.
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     */
	template<class CharSeq>
	explicit parameter_list_base(chewer<CharSeq> & chew)
	: _defect(none),_variadic(false) {
		read(chew);
	}

	/// Copy constructor.
	parameter_list_base(parameter_list_base const & other)
	:	 _params(
		other._params ?
			new std::vector<std::string>(*other._params) : nullptr),
		_defect(other._defect),
		_variadic(other._variadic){}

	/// Swap with another `parameter_list_base`
	void swap(parameter_list_base & other) {
		std::swap(_params,other._params);
		std::swap(_defect,other._defect);
		std::swap(_variadic,other._variadic);
	}

	/// Assignment
	parameter_list_base & operator=(parameter_list_base other) {
		swap(other);
		return *this;
	}

	/// Equality
	bool operator==(parameter_list_base const & other) const {
		return str() == other.str();
	}

	/// Inequality
	bool operator!=(parameter_list_base const & other) const {
		return !(*this == other);
	}

	/// Say whether the `parameter_list_base` is well-formed
	bool well_formed() const {
		return _defect == none;
	}

	/// Say whether the `parameter_list_base` is variadic
	bool variadic() const {
		return _variadic;
	}

	/** \brief Get the number of parameters in the `parameter_list_base`
     *
	 *	The non-null parameter list "()" has size 0, as does the null
	 *	parameter list ""
	 */
	size_t size() const {
		return _params ? _params->size() : 0;
	}

	/// Say whether the `parameter_list_base` is null, i.e. is not even "()"
	bool null() const {
		return !_params;
	}

	/// Cast to boolean = !null()
	operator bool () const {
		return !null();
	}

	/// Cast the parameter list to its canonical string representation
	std::string str() const;


    ///@{
    /** \brief Get a [const] iterator to the start of the parameter list.
     *
     *  \return An iterator to the start of the parameter list,
     *  or `std::vector<string>::iterator()` if `null()` is true
     */
    std::vector<std::string>::iterator begin() {
        using namespace std;
        return null() ? vector<string>::iterator() : _params->begin();
    }

    std::vector<std::string>::const_iterator begin() const {
        using namespace std;
        return null() ? vector<string>::const_iterator() : _params->begin();
    }
    ///@}

    ///@{
    /** \brief Get a [const] iterator to the end of the parameter list.
     *
     *  \return An iterator to the end of the parameter list,
     *  or `std::vector<string>::iterator()` if `null()` is true
     */
    std::vector<std::string>::iterator end() {
        using namespace std;
        return null() ? vector<string>::iterator() : _params->end();
    }

    std::vector<std::string>::const_iterator end() const {
        using namespace std;
        return null() ? vector<string>::const_iterator() : _params->end();
    }
    ///@}

    ///@{
    /** \brief Get a range-checked [const] reference to the nth parameter.
     *
     *  \param n Index of the requested parameter.
     *  \return A [const] reference to the nth paremeter.
     *  \throw `std::out_of_range` on out-of-range error
     */
    std::string const & at(size_t n) const {
        using namespace std;
        if (!size()) {
            throw out_of_range("Out of range in parameter_list_base::at(size_t)");
        }
        return (*_params)[n];
    }

    std::string & at(size_t n) {
        using namespace std;
        if (!size()) {
            throw out_of_range("Out of range in parameter_list_base::at(size_t)");
        }
        return (*_params)[n];
    }
    ///@}

    ///@{
    /** \brief Get an un-range-checked [const] reference to the nth parameter.
     *  \param n Index of the requested parameter.
     *  \return A [const] reference to the parameter at index `n`.
     *  \throw `std::out_of_range` on out-of-range error
     */
    std::string const & operator[](size_t n) const {
        return (*_params)[n];
    }

    std::string & operator[](size_t n) {
        return (*_params)[n];
    }
    ///@}


	/// Get the index of the parameter that matches a string, if any, else -1
	size_t which(std::string const & str) const;

	/*! \brief Read the `parameter_list_base` from `chewer<CharSeq>`
     *
	 *  \tparam CharSeq A character-sequence type.
	 *  \param chew On entry, a `chewer<CharSeq>` positioned at the offset
	 *  in the associated `CharSeq` from which to scan. On return`chew` is
	 *  positioned to the first offset not consumed.
     *
	 *  The `parameter_list_base` is emptied and replaced by parsing the text
	 *  from `chew`
	 */
    template<class CharSeq>
 	void read(chewer<CharSeq> & chew);

	/** \brief Make a parameter list from a range of objects that are
	 *	convertible to `std::string`.
     *
	 *	\tparam BiIter A bidorectional iterator.
	 *	\param first The start of the range of objects.
	 *	\param last The end of the range of objects.
	 *  \return A string comprising the comma-punctuated list
	 *  of [first,last), enclosed in parentheses.
	 */
	template<typename BiIter>
	static std::string make(BiIter first, BiIter last) {
		std::string s(1,'(');
		if (first != last) {
			auto last_but_one = last;
			std::advance(last_but_one,-1);
			for (	;first != last_but_one; ++first) {
				s += std::string(*first) + ',';
			}
			s += std::string(first);
		}
		s += ')';
		return s;
	}

protected:

	/// Enumeration of possible defects in a `parameter_list_base`
	enum defect {
		/// No defects
		none,
		/// An empty parameter
		empty_param,
		/// No closing ')'
		unclosed,
		/// Non-identifier where parameter expected.
		non_param
	};

	/// The list of parameters
	std::shared_ptr<std::vector<std::string>> _params;
	/// Is the `parameter_list_base` well-formed?
	defect _defect;
	/// Is the `parameter_list_base` variadic?
	bool _variadic;
};

} // namespace innards

#endif //EOF
