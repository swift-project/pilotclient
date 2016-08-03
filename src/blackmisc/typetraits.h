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

    class CPropertyIndex;

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
    struct THasToQString : public std::false_type {};
    //! \cond
    template <typename T>
    struct THasToQString<T, void_t<decltype(std::declval<T>().toQString())>> : public std::true_type {};
    //! \endcond

#ifdef Q_CC_MSVC // work around what seems to be an expression SFINAE bug in MSVC
    namespace Private
    {
        struct THasPushBackHelper
        {
            struct Base { int push_back; };
            template <typename T> struct Derived : public T, public Base {};
            template <typename T, T> struct TypeCheck {};
            template <typename T> static std::false_type test(TypeCheck<decltype(&Base::push_back), &Derived<T>::push_back> *);
            template <typename T> static std::true_type test(...);
        };
    }
    template <typename T>
    using THasPushBack = decltype(Private::THasPushBackHelper::test<T>(nullptr));
#else
    /*!
     * Trait which is true if the expression a.push_back(v) is valid when a and v are instances of T and T::value_type.
     */
    template <typename T, typename = void_t<>>
    struct THasPushBack : public std::false_type {};
    //! \cond
    template <typename T>
    struct THasPushBack<T, void_t<decltype(std::declval<T>().push_back(std::declval<typename T::value_type>()))>> : public std::true_type {};
    //! \endcond
#endif

#ifdef Q_CC_MSVC
    namespace Private
    {
        struct THasGetLogCategoriesHelper
        {
            struct Base { int getLogCategories; };
            template <typename T> struct Derived : public T, public Base {};
            template <typename T, T> struct TypeCheck {};
            template <typename T> static std::false_type test(TypeCheck<decltype(&Base::getLogCategories), &Derived<T>::getLogCategories> *);
            template <typename T> static std::true_type test(...);
        };
    }
    template <typename T>
    using THasGetLogCategories = decltype(Private::THasGetLogCategoriesHelper::test<T>(nullptr));
#else
    /*!
     * Trait to detect whether a class T has a static member function named getLogCategories.
     */
    template <typename T, typename = void_t<>>
    struct THasGetLogCategories : public std::false_type {};
    //! \cond
    template <typename T>
    struct THasGetLogCategories<T, void_t<decltype(T::getLogCategories())>> : public std::true_type {};
    //! \endcond
#endif

    /*!
     * Trait to detect whether a class T can be used as a key in a QHash.
     */
    template <typename T, typename = void_t<>>
    struct TModelsQHashKey : public std::false_type {};
    //! \cond
    template <typename T>
    struct TModelsQHashKey<T, void_t<decltype(std::declval<T>() == std::declval<T>(), qHash(std::declval<T>()))>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait to detect whether a class T can be used as a key in a QMap.
     */
    template <typename T, typename = void_t<>>
    struct TModelsQMapKey : public std::false_type {};
    //! \cond
    template <typename T>
    struct TModelsQMapKey<T, void_t<decltype(std::declval<T>() < std::declval<T>())>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait which is true if the expression compare(a, b) is valid when a and b are instances of T and U.
     */
    template <typename T, typename U, typename = void_t<>>
    struct THasCompare : public std::false_type {};
    //! \cond
    template <typename T, typename U>
    struct THasCompare<T, U, void_t<decltype(compare(std::declval<T>(), std::declval<U>()))>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait which is true if the expression a.compareByPropertyIndex(b, i) is valid when a and b are instances of T,
     * and i is an instance of CPropertyIndex.
     */
    template <typename T, typename = void_t<>>
    struct THasCompareByPropertyIndex : public std::false_type {};
    //! \cond
    template <typename T>
    struct THasCompareByPropertyIndex<T, void_t<decltype(std::declval<T>().compareByPropertyIndex(std::declval<CPropertyIndex>(), std::declval<T>()))>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait which is true if the expression a.propertyByIndex(i) is valid with a is an instance of T and i is an
     * instance of CPropertyIndex.
     */
    template <typename T, typename = void_t<>>
    struct THasPropertyByIndex : public std::false_type {};
    //! \cond
    template <typename T>
    struct THasPropertyByIndex<T, void_t<decltype(std::declval<T>().propertyByIndex(std::declval<CPropertyIndex>()))>> : public std::true_type {};
    //! \endcond

    /*!
     * Trait which is true if the expression a == b is valid when a and b are instances of T and U.
     */
    template <typename T, typename U, typename = void_t<>>
    struct TIsEqualityComparable : public std::false_type {};
    //! \cond
    template <typename T, typename U>
    struct TIsEqualityComparable<T, U, void_t<decltype(std::declval<T>() == std::declval<U>())>> : public std::true_type {};
    //! \endcond

}

#endif
