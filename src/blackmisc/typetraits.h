/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_TYPETRAITS_H
#define BLACKMISC_TYPETRAITS_H

#if defined(Q_CC_CLANG) || (defined(Q_CC_GNU) && __GNUC__ >= 5)
#define BLACK_HAS_FIXED_CWG1558
#endif

namespace BlackMisc
{

    //! \cond PRIVATE
#ifdef BLACK_HAS_FIXED_CWG1558
    // Own implementation of C++17 std::void_t, simple variadic alias
    // template which is always void. Useful for expression SFINAE.
    template <typename...>
    using void_t = void;
#else // Work around defect in the C++ standard
    namespace Private
    {
        template <typename...>
        struct make_void { using type = void; };
    }
    template <typename... Ts>
    using void_t = typename Private::make_void<Ts...>::type;
#endif
    //! \endcond

    /*!
     * Trait to detect whether T contains a member function toQString.
     */
    template <typename T, typename = void_t<>>
    struct HasToQString : public std::false_type {};
    //! \cond
    template <typename T>
    struct HasToQString<T, void_t<decltype(std::declval<T>().toQString())>> : public std::true_type {};
    //! \endcond

#ifdef Q_CC_MSVC // work around what seems to be an expression SFINAE bug in MSVC
    namespace Private
    {
        struct HasGetLogCategoriesHelper
        {
            struct Base { int getLogCategories; };
            template <typename T> struct Derived : public T, public Base {};
            template <typename T, T> struct TypeCheck {};
            template <typename T> static std::false_type test(TypeCheck<decltype(&Base::getLogCategories), &Derived<T>::getLogCategories> *);
            template <typename T> static std::true_type test(...);
        };
    }
    template <typename T>
    using HasGetLogCategories = decltype(Private::HasGetLogCategoriesHelper::test<T>(nullptr));
#else
    /*!
     * Trait to detect whether a class T has a static member function named getLogCategories.
     */
    template <typename T, typename = void_t<>>
    struct HasGetLogCategories : public std::false_type {};
    //! \cond
    template <typename T>
    struct HasGetLogCategories<T, void_t<decltype(T::getLogCategories())>> : public std::true_type {};
    //! \endcond
#endif

    /*!
     * Trait to detect whether a class T can be used as a key in a QHash.
     */
    template <typename T, typename = void_t<>>
    struct ModelsQHashKey : public std::false_type {};
    //! \cond
    template <typename T>
    struct ModelsQHashKey<T, void_t<decltype(std::declval<T>() == std::declval<T>(), qHash(std::declval<T>()))>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait to detect whether a class T can be used as a key in a QMap.
     */
    template <typename T, typename = void_t<>>
    struct ModelsQMapKey : public std::false_type {};
    //! \cond
    template <typename T>
    struct ModelsQMapKey<T, void_t<decltype(std::declval<T>() < std::declval<T>())>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait which is true if the expression compare(a, b) is valid when a and b are instances of T and U.
     */
    template <typename T, typename U, typename = void_t<>>
    struct HasCompare : public std::false_type {};
    //! \cond
    template <typename T, typename U>
    struct HasCompare<T, U, void_t<decltype(compare(std::declval<T>(), std::declval<U>()))>> : public std::true_type {};
    //! \endcond

}

#endif
