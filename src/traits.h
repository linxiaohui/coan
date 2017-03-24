#ifndef TRAITS_H
#define TRAITS_H
#pragma once
/***************************************************************************
 *   Copyright (C) 2013 Mike Kinghan, imk@burroingroingjoing.com           *
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
#include <type_traits>
#include <cstddef>

/** \file traits.h
 *	This file defines meta functions for testing type-traits
 */

namespace traits {

using std::is_same;
using std::declval;

/** \brief `template struct traits::has_value_type_of<T,V>`
 *   exports a static const boolean member `value` that is true
 *   iff `T` publicly defines type `value_type` as `V`
 */
template<typename T, typename V>
struct has_value_type_of
{
    /// \cond NO_DOXYGEN
    template<typename A>
    static constexpr bool test(typename A::value_type * ptr) {
        return is_same<decltype(ptr),V *>::value;
    }
    static constexpr bool test(...) {
        return false;
    }
    static const bool value = test<T>(nullptr);
    /// \endcond NO_DOXYGEN
};


/** \brief `template struct traits::has_size_method<T>`
 *  exports a static const boolean member `value` that is true
 *  iff the method `size_t T::size() const` exists.
 */
template<typename T>
struct has_size_method
{
    /// \cond NO_DOXYGEN
    template<typename A>
    static constexpr bool test(decltype(declval<A>().size()) * ptr) {
        return is_same<decltype(ptr),size_t *>::value;
    }
    static constexpr bool test(...) {
        return false;
    }

    static const bool value = test<T>(nullptr);
    /// \endcond NO_DOXYGEN
};

/** \brief `template struct traits::has_size_index_operator<T>`
 *  exports a static const boolean member `value` that is true
 *  iff the method `T::value_type & T::operator[](size_t)` exists.
 */
template< typename T>
struct has_index_operator
{
    /// \cond NO_DOXYGEN
    template<typename A>
    static constexpr
    auto test( decltype(&declval<A>()[size_t(0)]) ptr) ->
        decltype(is_same<decltype(*ptr),typename A::value_type &>::value)
    {
        return is_same<decltype(*ptr),typename A::value_type &>::value;
    }

    static constexpr bool test(...) {
        return false;
    }

    static const bool value = test<T>(nullptr);
    /// \endcond NO_DOXYGEN
};

/** \brief `template struct traits::has_data_method<T>`
 *  exports a static const boolean member `value` that is true
 *  iff the method `T::value_type const * T::data()` exists.
 */
template< typename T>
struct has_data_method
{
    /// \cond NO_DOXYGEN
    template<typename A>
    static constexpr auto test(decltype(declval<A const>().data()) ptr) ->
        decltype(is_same<decltype(ptr),typename A::value_type const *>::value)
    {
        return is_same<decltype(ptr),typename A::value_type const *>::value;
    }

    static constexpr bool test(...) {
        return false;
    }

    static const bool value = test<T>(nullptr);
    /// \endcond NO_DOXYGEN
};

/** \brief `template struct traits::has_extend_method<T>`
 *  exports a static const boolean member `value` that is true
 *  iff the method `size_t T::extend()` exists.
 */
template< typename T>
struct has_extend_method
{
    /// \cond NO_DOXYGEN
    template<typename A>
    static constexpr bool test(decltype(declval<A>().extend()) * ptr) {
        return is_same<decltype(ptr),size_t *>::value;
    }

    template<typename A>
    static constexpr bool test(...) {
        return false;
    }

    static const bool value = test<T>(nullptr);
    /// \endcond NO_DOXYGEN
};

/** \brief `template struct traits::is_random_access_char_sequence<T>`
 *  exports a static const boolean member `value` that is true
 *  iff T has a `value_type` = `char` and methods `size_t T::size() const`,
 *  `char const *T::data() const` and `char & T::operator[](size_t)`
 */
template<typename T>
struct is_random_access_char_sequence
{
    /// \cond NO_DOXYGEN
    static const bool value =
        has_value_type_of<T,char>::value &&
        has_size_method<T>::value &&
        has_data_method<T>::value &&
        has_index_operator<T>::value;
    /// \endcond NO_DOXYGEN
};


} // namespace traits

#endif // EOF

