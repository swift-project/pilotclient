// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_VALUEOBJECT_H
#define SWIFT_MISC_VALUEOBJECT_H

#include "misc/swiftmiscexport.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/metaclass.h"
#include "misc/mixin/mixinjson.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinicon.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinstring.h"

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

//! \cond
#define SWIFT_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, Extern)    \
    namespace Namespace                                                \
    {                                                                  \
        class Class;                                                   \
    }                                                                  \
    namespace swift::misc::private_ns                                  \
    {                                                                  \
        Extern template struct CValueObjectMetaInfo<Namespace::Class>; \
        Extern template struct MetaTypeHelper<Namespace::Class>;       \
    }                                                                  \
    namespace swift::misc::mixin                                       \
    {                                                                  \
        Extern template class MetaType<Namespace::Class>;              \
        Extern template class HashByMetaClass<Namespace::Class>;       \
        Extern template class DBusByMetaClass<Namespace::Class>;       \
        Extern template class DataStreamByMetaClass<Namespace::Class>; \
        Extern template class JsonByMetaClass<Namespace::Class>;       \
        Extern template class EqualsByMetaClass<Namespace::Class>;     \
        Extern template class LessThanByMetaClass<Namespace::Class>;   \
        Extern template class CompareByMetaClass<Namespace::Class>;    \
        Extern template class String<Namespace::Class>;                \
        Extern template class Index<Namespace::Class>;                 \
        Extern template class Icon<Namespace::Class>;                  \
    }
//! \endcond

/*!
 * \def SWIFT_DECLARE_VALUEOBJECT_MIXINS
 * Explicit template declaration of mixins for a CValueObject subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def SWIFT_DEFINE_VALUEOBJECT_MIXINS
 * Explicit template definition of mixins for a CValueObject subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_DECLARE_VALUEOBJECT_MIXINS(Namespace, Class)
#    define SWIFT_DEFINE_VALUEOBJECT_MIXINS(Namespace, Class)
#else
#    define SWIFT_DECLARE_VALUEOBJECT_MIXINS(Namespace, Class) SWIFT_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, extern)
#    define SWIFT_DEFINE_VALUEOBJECT_MIXINS(Namespace, Class) SWIFT_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, )
#endif

namespace swift::misc
{
    /*!
     * Default base class for CValueObject.
     */
    class SWIFT_MISC_EXPORT CEmpty
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
        CEmpty &operator=(const CEmpty &) = default;

        //! Non-virtual protected destructor
        ~CEmpty() = default;
    };

    /*!
     * Mix of the most commonly used mixin classes.
     * \see swift::misc::mixin
     * \tparam Derived  The class which is inheriting from this one (CRTP).
     */
    template <class Derived>
    class CValueObject :
        public CEmpty,
        public mixin::MetaType<Derived>,
        public mixin::HashByMetaClass<Derived>,
        public mixin::DBusByMetaClass<Derived>,
        public mixin::DataStreamByMetaClass<Derived>,
        public mixin::JsonByMetaClass<Derived>,
        public mixin::EqualsByMetaClass<Derived>,
        public mixin::LessThanByMetaClass<Derived>,
        public mixin::CompareByMetaClass<Derived>,
        public mixin::String<Derived>,
        public mixin::Index<Derived>,
        public mixin::Icon<Derived>
    {
    public:
        //! Base class
        using base_type = CEmpty;

        //! \copydoc swift::misc::mixin::String::toQString
        using mixin::String<Derived>::toQString;

        //! \copydoc swift::misc::mixin::String::toStdString
        using mixin::String<Derived>::toStdString;

        //! \copydoc swift::misc::mixin::Index::apply
        using mixin::Index<Derived>::apply;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJson
        using mixin::JsonByMetaClass<Derived>::toJson;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::toJsonString
        using mixin::JsonByMetaClass<Derived>::toJsonString;

        //! \copydoc swift::misc::mixin::JsonByMetaClass::convertFromJson
        using mixin::JsonByMetaClass<Derived>::convertFromJson;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        using mixin::Index<Derived>::setPropertyByIndex;

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        using mixin::Index<Derived>::propertyByIndex;

        //! \copydoc swift::misc::mixin::Index::comparePropertyByIndex
        using mixin::Index<Derived>::comparePropertyByIndex;

        //! \copydoc swift::misc::mixin::Index::equalsPropertyByIndex
        using mixin::Index<Derived>::equalsPropertyByIndex;

        //! \copydoc swift::misc::mixin::Icon::toIcon
        using mixin::Icon<Derived>::toIcon;

        //! \copydoc swift::misc::mixin::MetaType::isA
        using mixin::MetaType<Derived>::isA;

        //! \copydoc swift::misc::mixin::MetaType::registerMetadata
        using mixin::MetaType<Derived>::registerMetadata;

    protected:
        //! Inheriting constructors.
        using CEmpty::CEmpty;

        //! Default constructor.
        CValueObject() = default;

        //! Copy constructor.
        CValueObject(const CValueObject &) = default;

        //! Copy assignment operator.
        CValueObject &operator=(const CValueObject &) = default;

        //! Destructor
        ~CValueObject() = default;

    public:
        //! \copydoc swift::misc::mixin::MetaType::getMetaTypeId
        using mixin::MetaType<Derived>::getMetaTypeId;

        //! \copydoc swift::misc::mixin::String::stringForStreaming
        using mixin::String<Derived>::stringForStreaming;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::marshallToDbus
        using mixin::DBusByMetaClass<Derived>::marshallToDbus;

        //! \copydoc swift::misc::mixin::DBusByMetaClass::unmarshallFromDbus
        using mixin::DBusByMetaClass<Derived>::unmarshallFromDbus;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        using mixin::DataStreamByMetaClass<Derived>::marshalToDataStream;

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        using mixin::DataStreamByMetaClass<Derived>::unmarshalFromDataStream;
    };

} // namespace

#endif // guard
