/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_INHERITANCE_TRAITS_H
#define BLACKMISC_INHERITANCE_TRAITS_H

#include "typetraits.h"
#include <QMetaType>
#include <type_traits>

namespace BlackMisc
{
    class CPropertyIndex;

    /*!
     * If T has a member typedef base_type, this trait will obtain it, otherwise void.
     */
    template <typename T, typename = void_t<>>
    struct BaseOf
    {
        using type = void; //!< void
    };
    //! \cond
    template <typename T>
    struct BaseOf<T, void_t<typename T::base_type>>
    {
        using type = typename T::base_type; //!< T::base_type
    };
    //! \endcond

    /*!
     * It T has a member typedef base_type which is a registered metatype, this trait will obtain it, otherwise void.
     */
    template <class T>
    struct MetaBaseOf
    {
        //! Type of T::base_type, or void if not declared.
        using type = std::conditional_t<QMetaTypeId<typename BaseOf<T>::type>::Defined, typename BaseOf<T>::type, void>;
    };

    /*!
     * If T has a member typedef base_type which has a member propertyByIndex, this trait will obtain it, otherwise void.
     */
    template <typename T, typename = void_t<>>
    struct IndexBaseOf
    {
        using type = void; //!< void
    };
    //! \cond
    template <typename T>
    struct IndexBaseOf<T, void_t<decltype(std::declval<typename T::base_type>().propertyByIndex(std::declval<CPropertyIndex>()))>>
    {
        using type = typename T::base_type; //!< T::base_type
    };
    //! \endcond

    /*!
     * Alias for typename BaseOf<T>::type.
     */
    template <class T>
    using BaseOfT = typename BaseOf<T>::type;

    /*!
     * Alias for typename MetaBaseOf<T>::type.
     */
    template <class T>
    using MetaBaseOfT = typename MetaBaseOf<T>::type;

    /*!
     * Alias for typename IndexBaseOf<T>::type.
     */
    template <class T>
    using IndexBaseOfT = typename IndexBaseOf<T>::type;
}

#endif
