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
#include "icons.h"
#include "blackmiscfreefunctions.h"
#include "valueobject_private.h"
#include "valueobject_policy.h"
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
    class CPropertyIndex;
    class CPropertyIndexList;
    class CPropertyIndexVariantMap;
    class CIcon;
    class CVariant;
    class CEmpty;

    /*!
     * This registers the value type T with the BlackMisc meta type system,
     * making it available for use with the extended feature set of BlackMisc::CVariant.
     *
     * The implementation (ab)uses the QMetaType converter function registration mechanism
     * to store a type-erased representation of the set of operations supported by T.
     * Unlike the singleton pattern, this approach means that CVariant can be used in plugins.
     */
    template <typename T>
    void registerMetaValueType()
    {
        if (QMetaType::hasRegisteredConverterFunction<T, Private::IValueObjectMetaInfo *>()) { return; }
        auto converter = [](const T &) { static Private::CValueObjectMetaInfo<T> info; return &info; };
        bool ok = QMetaType::registerConverter<T, Private::IValueObjectMetaInfo *>(converter);
        Q_ASSERT(ok);
        Q_UNUSED(ok);
    }

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
     * Default policy classes for use by CValueObject.
     *
     * The default policies are inherited from the policies of the base class. There is a specialization
     * for the terminating case in which the base class is CEmpty.
     *
     * Specialize this template to use non-default policies for a particular derived class.
     * Due to the void default template parameter, specializations can inherit from CValueObjectPolicy<>
     * so that only the policies which differ from the default need be specified.
     * Policy classes which can be used are defined in namespace BlackMisc::Policy.
     */
    template <class Derived = void> struct CValueObjectPolicy
    {
        using MetaType = Policy::MetaType::Inherit; //!< Metatype policy
        using Equals = Policy::Equals::Inherit;     //!< Equals policy
        using LessThan = Policy::LessThan::Inherit; //!< LessThan policy
        using Compare = Policy::Compare::Inherit;   //!< Compare policy
        using Hash = Policy::Hash::Inherit;         //!< Hash policy
        using DBus = Policy::DBus::Inherit;         //!< DBus policy
        using Json = Policy::Json::Inherit;         //!< JSON policy
        using PropertyIndex = Policy::PropertyIndex::Default;   //!< PropertyIndex policy
    };

    /*!
     * Default policy classes for use by CValueObject.
     *
     * Specialization for the terminating case in which the base class is CEmpty.
     */
    template <> struct CValueObjectPolicy<CEmpty>
    {
        using MetaType = Policy::MetaType::Default;     //!< Metatype policy
        using Equals = Policy::Equals::MetaTuple;       //!< Equals policy
        using LessThan = Policy::LessThan::MetaTuple;   //!< Less than policy
        using Compare = Policy::Compare::MetaTuple;     //!< Compare policy
        using Hash = Policy::Hash::MetaTuple;           //!< Hash policy
        using DBus = Policy::DBus::MetaTuple;           //!< DBus policy
        using Json = Policy::Json::MetaTuple;           //!< JSon policy
        using PropertyIndex = Policy::PropertyIndex::Default;   //!< PropertyIndex policy
    };

    /*!
     * Policy classes for use by classes with incomplete migration to new CValueObject.
     *
     * This is to make it easier to apply the necessary changes to these classes for #356.
     * \todo Remove this and finish migrating classes that use it.
     */
    struct CValueObjectLegacy : public CValueObjectPolicy<CEmpty>
    {
        using Equals = Policy::Equals::None;        //!< Equals policy
        using LessThan = Policy::LessThan::None;    //!< Less than policy
        using Compare = Policy::Compare::None;      //!< Compare policy
        using Hash = Policy::Hash::Own;             //!< Hash policy
        using DBus = Policy::DBus::Own;             //!< DBus policy
        using Json = Policy::Json::Own;             //!< JSon policy
    };

    namespace Mixin
    {

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with the metatype of the class.
         */
        template <class Derived, class... AdditionalTypes>
        class MetaType
        {
        public:
            //! Register metadata
            static void registerMetadata()
            {
                Private::MetaTypeHelper<Derived>::maybeRegisterMetaType();

                [](...){}((qRegisterMetaType<AdditionalTypes>(), qDBusRegisterMetaType<AdditionalTypes>(), 0)...);
            }

            //! Returns the Qt meta type ID of this object.
            int getMetaTypeId() const
            {
                return Private::MetaTypeHelper<Derived>::maybeGetMetaTypeId();
            }

            //! Returns true if this object is an instance of the class with the given meta type ID, or one of its subclasses.
            bool isA(int metaTypeId) const
            {
                if (metaTypeId == QMetaType::UnknownType) { return false; }
                if (metaTypeId == getMetaTypeId()) { return true; }
                return baseIsA(static_cast<const MetaBaseOfT<Derived> *>(derived()), metaTypeId);
            }

            //! Method to return CVariant
            //! \deprecated Use CVariant::to() instead.
            CVariant toCVariant() const;

            //! Set from CVariant
            //! \deprecated Use CVariant::from() instead.
            void convertFromCVariant(const CVariant &variant);

            //! Return QVariant, used with DBus QVariant lists
            //! \deprecated Use QVariant::fromValue() instead.
            QVariant toQVariant() const
            {
                return Private::MetaTypeHelper<Derived>::maybeToQVariant(*derived());
            }

            //! Set from QVariant
            //! \deprecated Use QVariant::value() instead.
            void convertFromQVariant(const QVariant &variant)
            {
                return Private::MetaTypeHelper<Derived>::maybeConvertFromQVariant(*derived(), variant);
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename Base2> static bool baseIsA(const Base2 *base, int metaTypeId) { return base->isA(metaTypeId); }
            static bool baseIsA(const void *, int) { return false; }
        };

        /*!
         * Variant of MetaType mixin which also registers QList<Derived> with the type system.
         */
        template <class Derived>
        class MetaTypeAndQList : public MetaType<Derived, QList<Derived>>
        {};

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by metatuple.
         */
        template <class Derived>
        class HashByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! qHash overload, needed for storing value in a QSet.
            friend uint qHash(const Derived &value, uint seed = 0)
            {
                return ::qHash(hashImpl(value), seed);
            }

        private:
            static uint hashImpl(const Derived &value)
            {
                return BlackMisc::qHash(toMetaTuple(value)) ^ baseHash(static_cast<const BaseOfT<Derived> *>(&value));
            }

            template <typename T> static uint baseHash(const T *base) { return qHash(*base); }
            static uint baseHash(const void *) { return 0; }
        };

        /*!
         * CRTP class template which will generate marshalling operators for a derived class with its own marshalling implementation.
         */
        template <class Derived>
        class DBusOperators
        {
        public:
            //! Unmarshalling operator >>, DBus to object
            friend const QDBusArgument &operator>>(const QDBusArgument &arg, Derived &obj)
            {
                arg.beginStructure();
                obj.unmarshallFromDbus(arg);
                arg.endStructure();
                return arg;
            }

            //! Marshalling operator <<, object to DBus
            friend QDBusArgument &operator<<(QDBusArgument &arg, const Derived &obj)
            {
                arg.beginStructure();
                obj.marshallToDbus(arg);
                arg.endStructure();
                return arg;
            }
        };

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with marshalling instances by metatuple.
         */
        template <class Derived>
        class DBusByTuple : public DBusOperators<Derived>, private Private::EncapsulationBreaker
        {
        public:
            //! Marshall without begin/endStructure, for when composed within another object
            void marshallToDbus(QDBusArgument &arg) const
            {
                baseMarshall(static_cast<const BaseOfT<Derived> *>(derived()), arg);
                using BlackMisc::operator<<;
                arg << Private::EncapsulationBreaker::toMetaTuple(*derived());
            }

            //! Unmarshall without begin/endStructure, for when composed within another object
            void unmarshallFromDbus(const QDBusArgument &arg)
            {
                baseUnmarshall(static_cast<BaseOfT<Derived> *>(derived()), arg);
                using BlackMisc::operator>>;
                arg >> Private::EncapsulationBreaker::toMetaTuple(*derived());
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static void baseMarshall(const T *base, QDBusArgument &arg) { base->marshallToDbus(arg); }
            template <typename T> static void baseUnmarshall(T *base, const QDBusArgument &arg) { base->unmarshallFromDbus(arg); }
            static void baseMarshall(const void *, QDBusArgument &) {}
            static void baseUnmarshall(void *, const QDBusArgument &) {}
        };

        /*!
         * CRTP class template which will generate marshalling operators for a derived class with its own marshalling implementation.
         */
        template <class Derived>
        class JsonOperators
        {
        public:
            //! operator >> for JSON
            friend const QJsonObject &operator>>(const QJsonObject &json, Derived &obj)
            {
                obj.convertFromJson(json);
                return json;
            }

            //! operator >> for JSON
            friend const QJsonValue &operator>>(const QJsonValue &json, Derived &obj)
            {
                obj.convertFromJson(json.toObject());
                return json;
            }

            //! operator >> for JSON
            friend const QJsonValueRef &operator>>(const QJsonValueRef &json, Derived &obj)
            {
                obj.convertFromJson(json.toObject());
                return json;
            }

            //! operator << for JSON
            friend QJsonArray &operator<<(QJsonArray &json, const Derived &obj)
            {
                json.append(obj.toJson());
                return json;
            }

            //! operator << for JSON
            friend QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const Derived &> &value)
            {
                json.insert(value.first, QJsonValue(value.second.toJson()));
                return json;
            }
        };

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with JSON by metatuple.
         */
        template <class Derived>
        class JsonByTuple : public JsonOperators<Derived>, private Private::EncapsulationBreaker
        {
        public:
            //! Cast to JSON object
            QJsonObject toJson() const
            {
                QJsonObject json = BlackMisc::serializeJson(Private::EncapsulationBreaker::toMetaTuple(*derived()));
                return Json::appendJsonObject(json, baseToJson(static_cast<const BaseOfT<Derived> *>(derived())));
            }

            //! Assign from JSON object
            void convertFromJson(const QJsonObject &json)
            {
                baseConvertFromJson(static_cast<BaseOfT<Derived> *>(derived()), json);
                BlackMisc::deserializeJson(json, Private::EncapsulationBreaker::toMetaTuple(*derived()));
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename T> static QJsonObject baseToJson(const T *base) { return base->toJson(); }
            template <typename T> static void baseConvertFromJson(T *base, const QJsonObject &json) { base->convertFromJson(json); }
            static QJsonObject baseToJson(const void *) { return {}; }
            static void baseConvertFromJson(void *, const QJsonObject &) {}
        };

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented using its compare function.
         */
        template <class Derived>
        class EqualsByCompare
        {
        public:
            //! Equals
            friend bool operator ==(const Derived &a, const Derived &b) { return compare(a, b) == 0; }

            //! Not equal
            friend bool operator !=(const Derived &a, const Derived &b) { return compare(a, b) != 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator== implemented by metatuple.
         */
        template <class Derived>
        class EqualsByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Equals
            friend bool operator ==(const Derived &a, const Derived &b) { return equals(a, b); }

            //! Not equal
            friend bool operator !=(const Derived &a, const Derived &b) { return ! equals(a, b); }

        private:
            static bool equals(const Derived &a, const Derived &b)
            {
                return toMetaTuple(a) == toMetaTuple(b) && baseEquals(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
            }
            template <typename T> static bool baseEquals(const T *a, const T *b) { return *a == *b; }
            static bool baseEquals(const void *, const void *) { return true; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented using its compare function.
         */
        template <class Derived>
        class LessThanByCompare
        {
        public:
            //! Less than
            friend bool operator <(const Derived &a, const Derived &b) { return compare(a, b) < 0; }

            //! Greater than
            friend bool operator >(const Derived &a, const Derived &b) { return compare(a, b) > 0; }

            //! Less than or equal
            friend bool operator <=(const Derived &a, const Derived &b) { return compare(a, b) <= 0; }

            //! Greater than or equal
            friend bool operator >=(const Derived &a, const Derived &b) { return compare(a, b) >= 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit operator< implemented by metatuple.
         */
        template <class Derived>
        class LessThanByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Less than
            friend bool operator <(const Derived &a, const Derived &b) { return less(a, b); }

            //! Greater than
            friend bool operator >(const Derived &a, const Derived &b) { return less(b, a); }

            //! Less than or equal
            friend bool operator <=(const Derived &a, const Derived &b) { return ! less(b, a); }

            //! Greater than or equal
            friend bool operator >=(const Derived &a, const Derived &b) { return ! less(a, b); }

        private:
            static bool less(const Derived &a, const Derived &b)
            {
                if (baseLess(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b))) { return true; }
                return toMetaTuple(a) < toMetaTuple(b);
            }
            template <typename T> static bool baseLess(const T *a, const T *b) { return *a < *b; }
            static bool baseLess(const void *, const void *) { return false; }
        };

        /*!
         * CRTP class template from which a derived class can inherit non-member compare() implemented by metatuple.
         */
        template <class Derived>
        class CompareByTuple : private Private::EncapsulationBreaker
        {
        public:
            //! Return negative, zero, or positive if a is less than, equal to, or greater than b.
            friend int compare(const Derived &a, const Derived &b) { return compareImpl(a, b); }

        private:
            static int compareImpl(const Derived &a, const Derived &b)
            {
                int baseCmp = baseCompare(static_cast<const BaseOfT<Derived> *>(&a), static_cast<const BaseOfT<Derived> *>(&b));
                if (baseCmp) { return baseCmp; }
                return BlackMisc::compare(toMetaTuple(a), toMetaTuple(b));
            }
            template <typename T> static int baseCompare(const T *a, const T *b) { return compare(*a, *b); }
            static int baseCompare(const void *, const void *) { return 0; }
        };

        /*!
         * CRTP class template from which a derived class can inherit string streaming operations.
         */
        template <class Derived>
        class String
        {
        public:
            //! Stream << overload to be used in debugging messages
            friend QDebug operator<<(QDebug debug, const Derived &obj)
            {
                debug << obj.stringForStreaming();
                return debug;
            }

            //! Operator << when there is no debug stream
            friend QNoDebug operator<<(QNoDebug nodebug, const Derived &obj)
            {
                Q_UNUSED(obj);
                return nodebug;
            }

            //! Operator << based on text stream
            friend QTextStream &operator<<(QTextStream &stream, const Derived &obj)
            {
                stream << obj.stringForStreaming();
                return stream;
            }

            //! Operator << for QDataStream
            friend QDataStream &operator<<(QDataStream &stream, const Derived &obj)
            {
                stream << obj.stringForStreaming();
                return stream;
            }

            //! Stream operator << for std::cout
            friend std::ostream &operator<<(std::ostream &ostr, const Derived &obj)
            {
                ostr << obj.stringForStreaming().toStdString();
                return ostr;
            }

            //! Cast as QString
            QString toQString(bool i18n = false) const { return derived()->convertToQString(i18n); }

            //! Cast to pretty-printed QString
            QString toFormattedQString(bool i18n = false) const { return derived()->toQString(i18n); }

            //! To std string
            std::string toStdString(bool i18n = false) const { return derived()->convertToQString(i18n).toStdString(); }

            //! String for streaming operators
            QString stringForStreaming() const { return derived()->convertToQString(); }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

        /*!
         * CRTP class template from which a derived class can inherit property indexing functions.
         */
        template <class Derived>
        class Index
        {
        public:
            //! Base class enums
            enum ColumnIndex
            {
                IndexPixmap = 10, // manually set to avoid circular dependencies
                IndexIcon,
                IndexString
            };

            //! Update by variant map
            //! \return number of values changed, with skipEqualValues equal values will not be changed
            CPropertyIndexList apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false); // implemented later due to cyclic include dependency

            //! Set property by index
            void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index); // implemented later due to cyclic include dependency

            //! Property by index
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const; // implemented later due to cyclic include dependency

            //! Property by index as String
            QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const; // implemented later due to cyclic include dependency

            //! Is given variant equal to value of property index?
            bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const; // implemented later due to cyclic include dependency

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

        /*!
         * CRTP class template from which a derived class can inherit icon-related functions.
         */
        template <class Derived, CIcons::IconIndex IconIndex = CIcons::StandardIconUnknown16>
        class Icon
        {
        public:
            //! As icon, not implemented by all classes
            CIcon toIcon() const; // implemented later due to cyclic include dependency

            //! As pixmap, required for most GUI views
            QPixmap toPixmap() const; // implemented later due to cyclic include dependency

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }
        };

    }

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

// TODO Includes due to cyclic dependencies can be removed when CValueObject is split into parts along policy boundaries.
#include "variant.h"
#include "propertyindex.h"
#include "propertyindexlist.h"
#include "iconlist.h"

// TODO Implementations of templates that must appear after those includes, should be moved at the same time that policies are refactored.
namespace BlackMisc
{
    namespace Mixin
    {
        template <class Derived, class... AdditionalTypes>
        CVariant MetaType<Derived, AdditionalTypes...>::toCVariant() const
        {
            return CVariant(derived()->toQVariant());
        }
        template <class Derived, class... AdditionalTypes>
        void MetaType<Derived, AdditionalTypes...>::convertFromCVariant(const CVariant &variant)
        {
            derived()->convertFromQVariant(variant.getQVariant());
        }
        template <class Derived>
        CPropertyIndexList Index<Derived>::apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues)
        {
            if (indexMap.isEmpty()) return {};

            CPropertyIndexList changed;
            const auto &map = indexMap.map();
            for (auto it = map.begin(); it != map.end(); ++it)
            {
                const CVariant value = it.value().toCVariant();
                const CPropertyIndex index = it.key();
                if (skipEqualValues)
                {
                    bool equal = derived()->equalsPropertyByIndex(value, index);
                    if (equal) { continue; }
                }
                derived()->setPropertyByIndex(value, index);
                changed.push_back(index);
            }
            return changed;
        }
        template <class Derived>
        void Index<Derived>::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                derived()->convertFromCVariant(variant);
                return;
            }

            // not all classes have implemented nesting
            const QString m = QString("Property by index not found (setter), index: ").append(index.toQString());
            qFatal("%s", qPrintable(m));
        }
        template <class Derived>
        CVariant Index<Derived>::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself())
            {
                return derived()->toCVariant();
            }
            auto i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcon:
                return CVariant::from(derived()->toIcon());
            case IndexPixmap:
                return CVariant::from(derived()->toPixmap());
            case IndexString:
                return CVariant(derived()->toQString());
            default:
                break;
            }

            // not all classes have implemented nesting
            const QString m = QString("Property by index not found, index: ").append(index.toQString());
            qFatal("%s", qPrintable(m));
            return {};
        }
        template <class Derived>
        QString Index<Derived>::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
        {
            // default implementation, requires propertyByIndex
            return derived()->propertyByIndex(index).toQString(i18n);
        }
        template <class Derived>
        bool Index<Derived>::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
        {
            return derived()->propertyByIndex(index) == compareValue;
        }
        template <class Derived, CIcons::IconIndex IconIndex>
        CIcon Icon<Derived, IconIndex>::toIcon() const
        {
            return CIconList::iconByIndex(IconIndex);
        }
        template <class Derived, CIcons::IconIndex IconIndex>
        QPixmap Icon<Derived, IconIndex>::toPixmap() const
        {
            return derived()->toIcon().toPixmap();
        }
    }
}

#endif // guard
