// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_INHERITANCE_TRAITS_H
#define BLACKMISC_INHERITANCE_TRAITS_H

#include <type_traits>

template <typename T>
struct QMetaTypeId;

namespace BlackMisc
{
    class CPropertyIndexRef;

    /*!
     * If T has a member typedef base_type, this trait will obtain it, otherwise void.
     */
    template <typename T, typename = std::void_t<>>
    struct TBaseOf
    {
        using type = void; //!< void
    };
    //! \cond
    template <typename T>
    struct TBaseOf<T, std::void_t<typename T::base_type>>
    {
        using type = typename T::base_type; //!< T::base_type
    };
    //! \endcond

    /*!
     * True if T has a member typedef base_type which is a registered metatype.
     */
    template <typename T>
    inline constexpr bool THasMetaBaseV = QMetaTypeId<typename TBaseOf<T>::type>::Defined;

    /*!
     * It T has a member typedef base_type which is a registered metatype, this trait will obtain it, otherwise void.
     */
    template <class T>
    struct TMetaBaseOf
    {
        //! Type of T::base_type, or void if not declared.
        using type = std::conditional_t<THasMetaBaseV<T>, typename TBaseOf<T>::type, void>;
    };

    /*!
     * If T has a member typedef base_type which has a member propertyByIndex, this trait will obtain it, otherwise void.
     */
    template <typename T, typename = std::void_t<>>
    struct TIndexBaseOf
    {
        using type = void; //!< void
    };
    //! \cond
    template <typename T>
    struct TIndexBaseOf<T, std::void_t<decltype(std::declval<typename T::base_type>().propertyByIndex(std::declval<CPropertyIndexRef>()))>>
    {
        using type = typename T::base_type; //!< T::base_type
    };
    //! \endcond

    /*!
     * Alias for typename TBaseOf<T>::type.
     */
    template <class T>
    using TBaseOfT = typename TBaseOf<T>::type;

    /*!
     * Alias for typename TMetaBaseOf<T>::type.
     */
    template <class T>
    using TMetaBaseOfT = typename TMetaBaseOf<T>::type;

    /*!
     * Alias for typename TIndexBaseOf<T>::type.
     */
    template <class T>
    using TIndexBaseOfT = typename TIndexBaseOf<T>::type;
}

#endif
