/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUEOBJECT_H
#define BLACKMISC_VALUEOBJECT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/mixin/mixindbus.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/mixin/mixinjson.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/mixin/mixinmetatype.h"
#include "blackmisc/mixin/mixinindex.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/mixin/mixinhash.h"
#include "blackmisc/mixin/mixinstring.h"

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
#define BLACK_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, Extern)    \
    namespace Namespace                                                \
    {                                                                  \
        class Class;                                                   \
    }                                                                  \
    namespace BlackMisc::Private                                       \
    {                                                                  \
        Extern template struct CValueObjectMetaInfo<Namespace::Class>; \
        Extern template struct MetaTypeHelper<Namespace::Class>;       \
    }                                                                  \
    namespace BlackMisc::Mixin                                         \
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
 * \def BLACK_DECLARE_VALUEOBJECT_MIXINS
 * Explicit template declaration of mixins for a CValueObject subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def BLACK_DEFINE_VALUEOBJECT_MIXINS
 * Explicit template definition of mixins for a CValueObject subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define BLACK_DECLARE_VALUEOBJECT_MIXINS(Namespace, Class)
#    define BLACK_DEFINE_VALUEOBJECT_MIXINS(Namespace, Class)
#else
#    define BLACK_DECLARE_VALUEOBJECT_MIXINS(Namespace, Class) BLACK_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, extern)
#    define BLACK_DEFINE_VALUEOBJECT_MIXINS(Namespace, Class) BLACK_TEMPLATE_VALUEOBJECT_MIXINS(Namespace, Class, )
#endif

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
        CEmpty &operator=(const CEmpty &) = default;

        //! Non-virtual protected destructor
        ~CEmpty() = default;
    };

    /*!
     * Mix of the most commonly used mixin classes.
     * \see BlackMisc::Mixin
     * \tparam Derived  The class which is inheriting from this one (CRTP).
     */
    template <class Derived>
    class CValueObject :
        public CEmpty,
        public Mixin::MetaType<Derived>,
        public Mixin::HashByMetaClass<Derived>,
        public Mixin::DBusByMetaClass<Derived>,
        public Mixin::DataStreamByMetaClass<Derived>,
        public Mixin::JsonByMetaClass<Derived>,
        public Mixin::EqualsByMetaClass<Derived>,
        public Mixin::LessThanByMetaClass<Derived>,
        public Mixin::CompareByMetaClass<Derived>,
        public Mixin::String<Derived>,
        public Mixin::Index<Derived>,
        public Mixin::Icon<Derived>
    {
    public:
        //! Base class
        using base_type = CEmpty;

        //! \copydoc BlackMisc::Mixin::String::toQString
        using Mixin::String<Derived>::toQString;

        //! \copydoc BlackMisc::Mixin::String::toFormattedQString
        //! \deprecated not really used and just using toQString
        using Mixin::String<Derived>::toFormattedQString;

        //! \copydoc BlackMisc::Mixin::String::toStdString
        using Mixin::String<Derived>::toStdString;

        //! \copydoc BlackMisc::Mixin::Index::apply
        using Mixin::Index<Derived>::apply;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJson
        using Mixin::JsonByMetaClass<Derived>::toJson;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::toJsonString
        using Mixin::JsonByMetaClass<Derived>::toJsonString;

        //! \copydoc BlackMisc::Mixin::JsonByMetaClass::convertFromJson
        using Mixin::JsonByMetaClass<Derived>::convertFromJson;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        using Mixin::Index<Derived>::setPropertyByIndex;

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        using Mixin::Index<Derived>::propertyByIndex;

        //! \copydoc BlackMisc::Mixin::Index::comparePropertyByIndex
        using Mixin::Index<Derived>::comparePropertyByIndex;

        //! \copydoc BlackMisc::Mixin::Index::equalsPropertyByIndex
        using Mixin::Index<Derived>::equalsPropertyByIndex;

        //! \copydoc BlackMisc::Mixin::Icon::toIcon
        using Mixin::Icon<Derived>::toIcon;

        //! \copydoc BlackMisc::Mixin::MetaType::isA
        using Mixin::MetaType<Derived>::isA;

        //! \copydoc BlackMisc::Mixin::MetaType::registerMetadata
        using Mixin::MetaType<Derived>::registerMetadata;

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
        //! \copydoc BlackMisc::Mixin::MetaType::getMetaTypeId
        using Mixin::MetaType<Derived>::getMetaTypeId;

        //! \copydoc BlackMisc::Mixin::String::stringForStreaming
        using Mixin::String<Derived>::stringForStreaming;

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::marshallToDbus
        using Mixin::DBusByMetaClass<Derived>::marshallToDbus;

        //! \copydoc BlackMisc::Mixin::DBusByMetaClass::unmarshallFromDbus
        using Mixin::DBusByMetaClass<Derived>::unmarshallFromDbus;

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        using Mixin::DataStreamByMetaClass<Derived>::marshalToDataStream;

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        using Mixin::DataStreamByMetaClass<Derived>::unmarshalFromDataStream;
    };

} // namespace

#endif // guard
