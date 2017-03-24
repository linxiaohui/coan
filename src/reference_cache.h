#ifndef REFERENCE_CACHE_H
#define REFERENCE_CACHE_H
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

#include "evaluation.h"
#include <string>
#include <map>

/** \file reference_cache.h
 *   This file defines `struct reference_cache`.
 */

/** \brief `struct reference_cache` encapsulates a cache of the
 *	the expansions and evaluations of symbol references.
 */
struct reference_cache
{
    /// Encapsulates an entry in the reference cache
	struct entry
	{
		/** Constructor
		 *  \param expansion The expansion of the reference
		 *  \param eval The evaluation of the reference
		 *  \param reported Has the reference been reported?
		 *  \param complete Is the expansion complete
         *       (or imcomplete because too big)
		 */
		entry(
			std::string const & expansion,
			evaluation const & eval,
			bool reported = false,
			bool complete = true)
		: 	_expansion(expansion),_eval(eval),_reported(reported),
			_complete(complete){}

		/// Get the expansion of the cached reference
		std::string const & expansion() const {
			return _expansion;
		}

		///@{
		/// \brief Get a [const] reference to the evaluation of the cached
		/// reference.
		evaluation const & eval() const {
			return _eval;
		}
		evaluation & eval() {
			return _eval;
		}
		///@}

		/// Has the cached reference been reported
		bool reported() const {
			return _reported;
		}

		/// Mark the entry as reported, or not.
		void set_reported(bool value = true) {
			_reported = value;
		}

		/// Is the cached reference fully expanded?
		bool complete() const {
			return _complete;
		}

		/// Mark the entry as fully expanded, or not.
		void set_complete(bool value = true) {
			_complete = value;
		}

	private:
		/// The expansion of the cached reference
		std::string _expansion;
		/// The evaluation of the cached reference
		evaluation _eval;
		/// Has the reference been reported.
		bool _reported;
		/// Is the cached expansion complete?
		bool _complete;
	};

    /// Type of map implementing the reference cache
	using map = std::map<std::string,entry>;
	/// Value-type of `map`
	using value_type = map::value_type;
	/// Iterator type on `map`
	using iterator = map::iterator;
	/// Const-iterator type on `map`
	using const_iterator = map::const_iterator;
	/// Type of result of insertion
	using insert_result = std::pair<iterator,bool>;

	/** \brief Insert a `value_type` into the cache
	 *  \param v The `value_type` to insert.
	 *  \param hint A hint-iterator as per `std::map::insert`.
	 *  \return An iterator to the inserted `value_type`.
	 */
	static iterator insert(value_type const & v, iterator hint) {
		return get_map().insert(hint,v);
	}


	/** \brief Insert an `entry` by key into cache
	 *  \param key The key of the entry.
	 *  \param e The entry to be inserted.
	 *  \param hint A hint-iterator as per `std::map::insert`.
	 *  \return An iterator to the inserted `value_type`.
	 */
	static iterator
	insert(std::string const & key, entry const & e, iterator hint) {
		return insert(value_type(key,e),hint);
	}

	/// Get an iteraror to the lower bound of a key in the cache.
	static iterator lower_bound(std::string const & key) {
		return get_map().lower_bound(key);
	}

	/// Delete all cached references of a given symbol.
	static void erase_symbol(std::string const & id){
		auto i = get_map().lower_bound(id);
		while (i != get_map().end()) {
			std::string const & key = i->first;
			if (key.find(id) == 0 &&
				(key.length() == id.length() || key[id.length()] == '(')) {
				i = get_map().erase(i);
			} else {
				break;
			}
		}
	}

	/// Empty the cache
	static void clear() {
		get_map().clear();
	}

	/// Say whether an iterator points to the end of the cache.
	template<class Iter>
	static bool at_end(Iter i) {
		return i == get_map().end();
	}

private:

	/// The cache map.
	static map & get_map() {
		static map the_map;
		return the_map;
	}
};

#endif //EOF
