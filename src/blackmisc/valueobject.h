/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUEOBJECT_H
#define BLACKMISC_VALUEOBJECT_H

#include "blackmiscexport.h"
#include "dbus.h"
#include "tuple.h"
#include "json.h"
#include "compare.h"
#include "variant.h"
#include "propertyindexvariantmap.h"
#include "iconlist.h"
#include "blackmiscfreefunctions.h"
#include <QtDBus/QDBusMetaType>
#include <QString>
#include <QtGlobal>
#include <QDataStream>
#include <QDebug>
#include <QPixmap>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <type_traits>
#include <iostream>

namespace BlackMisc
{

    /*!
     * Default base class for CValueObject.
     */
    class BLACKMISC_EXPORT CEmpty
    {
    public:
        //! Base class is alias of itself
        using base_type = CEmpty;

    protected:
        //! Protected default constructor
        CEmpty() = default;

        //! Protected copy constructor
        CEmpty(const CEmpty &) = default;

        //! Protected copy assignment operator
        CEmpty &operator =(const CEmpty &) = default;

        //! Non-virtual protected destructor
        ~CEmpty() = default;
    };

    /*!
     * Standard implementation of CValueObject using meta tuple system.
     *
     * This uses policy-based design. Specialize the class template CValueObjectPolicy
     * to specify different policy classes.
     *
     * \tparam Derived  The class which is inheriting from this one (CRTP).
     * \tparam Base     The class which this one shall inherit from (default is CEmpty,
     *                  but this can be changed to create a deeper inheritance hierarchy).
     */
    template <class Derived, class Base /*= CEmpty*/> class CValueObject :
        public Base,
        public Mixin::MetaType<Derived>,
        public Mixin::HashByTuple<Derived>,
        public Mixin::DBusByTuple<Derived>,
        public Mixin::JsonByTuple<Derived>,
        public Mixin::EqualsByTuple<Derived>,
        public Mixin::LessThanByTuple<Derived>,
        public Mixin::CompareByTuple<Derived>,
        public Mixin::String<Derived>,
        public Mixin::Index<Derived>,
        public Mixin::Icon<Derived>
    {
    public:
        //! Base class
        using base_type = Base;

        //! \copydoc BlackMisc::Mixin::String::toQString
        using Mixin::String<Derived>::toQString;

        //! \copydoc BlackMisc::Mixin::String::toFormattedQString
        using Mixin::String<Derived>::toFormattedQString;

        //! \copydoc BlackMisc::Mixin::String::toStdString
        using Mixin::String<Derived>::toStdString;

        //! \copydoc BlackMisc::Mixin::Index::apply
        using Mixin::Index<Derived>::apply;

        //! \copydoc BlackMisc::Mixin::MetaType::toCVariant
        using Mixin::MetaType<Derived>::toCVariant;

        //! \copydoc BlackMisc::Mixin::MetaType::convertFromCVariant
        using Mixin::MetaType<Derived>::convertFromCVariant;

        //! \copydoc BlackMisc::Mixin::JsonByTuple::toJson
        using Mixin::JsonByTuple<Derived>::toJson;

        //! \copydoc BlackMisc::Mixin::JsonByTuple::convertFromJson
        using Mixin::JsonByTuple<Derived>::convertFromJson;

        //! \copydoc BlackMisc::Mixin::MetaType::toQVariant
        using Mixin::MetaType<Derived>::toQVariant;

        //! \copydoc BlackMisc::Mixin::MetaType::convertFromQVariant
        using Mixin::MetaType<Derived>::convertFromQVariant;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        using Mixin::Index<Derived>::setPropertyByIndex;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        using Mixin::Index<Derived>::propertyByIndex;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndexAsString
        using Mixin::Index<Derived>::propertyByIndexAsString;

        //! \copydoc BlackMisc::Mixin::Index::equalsPropertyByIndex
        using Mixin::Index<Derived>::equalsPropertyByIndex;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon
        using Mixin::Icon<Derived>::toIcon;

        //! \copydoc BlackMisc::Mixin::Icon::toPixmap
        using Mixin::Icon<Derived>::toPixmap;

        //! \copydoc BlackMisc::Mixin::MetaType::isA
        using Mixin::MetaType<Derived>::isA;

        //! \copydoc BlackMisc::Mixin::MetaType::registerMetadata
        using Mixin::MetaType<Derived>::registerMetadata;

    protected:
        //! Default constructor.
        CValueObject() = default;

        //! Template constructor, forwards all arguments to base class constructor.
        //! \todo When our compilers support C++11 inheriting constructors, use those instead.
        template <typename T, typename... Ts, typename = typename std::enable_if<! std::is_same<CValueObject, typename std::decay<T>::type>::value>::type>
        CValueObject(T &&first, Ts &&... args) : Base(std::forward<T>(first), std::forward<Ts>(args)...) {}

        //! Copy constructor.
        CValueObject(const CValueObject &) = default;

        //! Copy assignment operator.
        CValueObject &operator =(const CValueObject &) = default;

        //! Destructor
        ~CValueObject() = default;

    public:
        //! \copydoc BlackMisc::Mixin::MetaType::getMetaTypeId
        using Mixin::MetaType<Derived>::getMetaTypeId;

        //! \copydoc BlackMisc::Mixin::String::stringForStreaming
        using Mixin::String<Derived>::stringForStreaming;

        //! \copydoc BlackMisc::Mixin::DBusByTuple::marshallToDbus
        using Mixin::DBusByTuple<Derived>::marshallToDbus;

        //! \copydoc BlackMisc::Mixin::DBusByTuple::unmarshallFromDbus
        using Mixin::DBusByTuple<Derived>::unmarshallFromDbus;
    };

} // namespace

#endif // guard
