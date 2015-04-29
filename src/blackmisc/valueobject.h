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

    //! Traits class to test whether a class is derived from CValueObject.
    //! \todo TemplateIsBaseOf gives incorrect result due to ambiguity if there is more than one specialization of CValueObject which is a base of T.
    template <class T>
    using IsValueObject = typename std::is_base_of<CEmpty, T>::type;

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

        //! As icon, not implemented by all classes
        //! \todo Here because incomplete type. Move to policy class during policy refactoring.
        virtual CIcon toIcon() const;

        //! As pixmap, required for most GUI views
        //! \todo Here because incomplete type. Move to policy class during policy refactoring.
        virtual QPixmap toPixmap() const;

        //! Parse from string, e.g. 100km/h
        //! \todo Here to avoid name hiding in PQ classes. Fix during policy refactoring.
        virtual void parseFromString(const QString &) { qFatal("Not implemented"); }

    protected:
        //! String for QString conversion
        //! \todo Here because pure virtual. Move to CValueObject when all dynamic polymorphism is removed.
        virtual QString convertToQString(bool i18n = false) const = 0;

        //! Protected default constructor
        CEmpty() = default;

        //! Protected copy constructor
        CEmpty(const CEmpty &) = default;

        //! Protected copy assignment operator
        CEmpty &operator =(const CEmpty &) = default;

        //! Non-virtual protected destructor
        ~CEmpty() = default;
    };

    //! Dummy comparison.
    inline int compare(const CEmpty &, const CEmpty &) { return 0; }

    /*!
     * Terminating base cases for the recursive methods of CValueObject.
     */
    struct CValueObjectDummyBase
    {
        //! To JSON
        static QJsonObject toJson() { return {}; }

        //! From JSON
        static void convertFromJson(const QJsonObject &) {}

        //! Marshall to DBus
        static void marshallToDbus(QDBusArgument &) {}

        //! Unmarshall from DBus
        static void unmarshallFromDbus(const QDBusArgument &) {}
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
        template <class Derived>
        class MetaType
        {
        public:
            //! Register metadata
            static void registerMetadata()
            {
                Private::MetaTypeHelper<Derived>::maybeRegisterMetaType();
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
                return baseIsA(static_cast<const typename Derived::base_type *>(derived()), metaTypeId);
            }

            //! Method to return CVariant
            CVariant toCVariant() const;

            //! Set from CVariant
            void convertFromCVariant(const CVariant &variant);

            //! Return QVariant, used with DBus QVariant lists
            //! \todo remove virtual
            virtual QVariant toQVariant() const
            {
                return Private::MetaTypeHelper<Derived>::maybeToQVariant(*derived());
            }

            //! Set from QVariant
            //! \todo remove virtual
            virtual void convertFromQVariant(const QVariant &variant)
            {
                return Private::MetaTypeHelper<Derived>::maybeConvertFromQVariant(*derived(), variant);
            }

        private:
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            Derived *derived() { return static_cast<Derived *>(this); }

            template <typename Base2> static bool baseIsA(const Base2 *base, int metaTypeId) { return base->isA(metaTypeId); }
            static bool baseIsA(const CEmpty *, int) { return false; }
        };

        /*!
         * CRTP class template from which a derived class can inherit common methods dealing with hashing instances by metatuple.
         */
        template <class Derived, bool IsTupleBased = true>
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
                return BlackMisc::qHash(toMetaTuple(value)) ^ baseHash(static_cast<const typename Derived::base_type &>(value));
            }

            template <typename T> static uint baseHash(const T &base) { return qHash(base); }
            static uint baseHash(const CEmpty &) { return 0; }
        };

        /*!
         * Specialization of HashByTuple for classes not registered with the tuple system.
         */
        template <class Derived>
        class HashByTuple<Derived, false>
        {};

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
        public Mixin::HashByTuple<Derived, Policy::Hash::IsMetaTuple<Derived, Base>::value>,
        private CValueObjectPolicy<Derived>::Equals::template Ops<Derived, Base>,
        private CValueObjectPolicy<Derived>::LessThan::template Ops<Derived, Base>,
        private CValueObjectPolicy<Derived>::Compare::template Ops<Derived, Base>
    {
        static_assert(std::is_same<CEmpty, Base>::value || IsValueObject<Base>::value, "Base must be either CEmpty or derived from CValueObject");

        using DBusPolicy = typename CValueObjectPolicy<Derived>::DBus;
        using JsonPolicy = typename CValueObjectPolicy<Derived>::Json;
        using PropertyIndexPolicy = typename CValueObjectPolicy<Derived>::PropertyIndex;

        using BaseOrDummy = typename std::conditional<std::is_same<Base, CEmpty>::value, CValueObjectDummyBase, Base>::type;

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
        friend QTextStream &operator<<(QTextStream &textStream, const Derived &obj)
        {
            textStream << obj.stringForStreaming();
            return textStream;
        }

        //! Operator << for QDataStream
        friend QDataStream &operator<<(QDataStream &stream, const Derived &valueObject)
        {
            stream << valueObject.stringForStreaming();
            return stream;
        }

        //! Stream operator << for std::cout
        friend std::ostream &operator<<(std::ostream &ostr, const Derived &obj)
        {
            ostr << obj.stringForStreaming().toStdString();
            return ostr;
        }

        //! Unmarshalling operator >>, DBus to object
        friend const QDBusArgument &operator>>(const QDBusArgument &arg, Derived &obj)
        {
            arg.beginStructure();
            static_cast<CValueObject &>(obj).unmarshallFromDbus(arg); // virtual method is protected in Derived
            arg.endStructure();
            return arg;
        }

        //! Marshalling operator <<, object to DBus
        friend QDBusArgument &operator<<(QDBusArgument &arg, const Derived &obj)
        {
            arg.beginStructure();
            static_cast<const CValueObject &>(obj).marshallToDbus(arg); // virtual method is protected in Derived
            arg.endStructure();
            return arg;
        }

        //! operator >> for JSON
        friend const QJsonObject &operator>>(const QJsonObject &json, Derived &valueObject)
        {
            valueObject.convertFromJson(json);
            return json;
        }

        //! operator >> for JSON
        friend const QJsonValue &operator>>(const QJsonValue &json, Derived &valueObject)
        {
            valueObject.convertFromJson(json.toObject());
            return json;
        }

        //! operator >> for JSON
        friend const QJsonValueRef &operator>>(const QJsonValueRef &json, Derived &valueObject)
        {
            valueObject.convertFromJson(json.toObject());
            return json;
        }

        //! operator << for JSON
        friend QJsonArray &operator<<(QJsonArray &json, const Derived &value)
        {
            json.append(value.toJson());
            return json;
        }

        //! operator << for JSON
        friend QJsonObject& operator<<(QJsonObject &json, const std::pair<QString, const Derived &> &value)
        {
            json.insert(value.first, QJsonValue(value.second.toJson()));
            return json;
        }

    public:
        //! Base class
        using base_type = Base;

        //! Destructor
        virtual ~CValueObject() {}

        //! Base class enums
        enum ColumnIndex
        {
            IndexPixmap = 10, // manually set to avoid circular dependencies
            IndexIcon,
            IndexString
        };

        //! Cast as QString
        QString toQString(bool i18n = false) const { return this->convertToQString(i18n); }

        //! Cast to pretty-printed QString
        virtual QString toFormattedQString(bool i18n = false) const { return this->toQString(i18n); }

        //! To std string
        std::string toStdString(bool i18n = false) const { return this->convertToQString(i18n).toStdString(); }

        //! Update by variant map
        //! \return number of values changed, with skipEqualValues equal values will not be changed
        CPropertyIndexList apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false); // implemented later due to cyclic include dependency

        //! \copydoc BlackMisc::Mixin::MetaType::toCVariant
        using Mixin::MetaType<Derived>::toCVariant;

        //! \copydoc BlackMisc::Mixin::MetaType::convertFromCVariant
        using Mixin::MetaType<Derived>::convertFromCVariant;

        //! Cast to JSON object
        virtual QJsonObject toJson() const
        {
            QJsonObject json = JsonPolicy::serializeImpl(*derived());
            return Json::appendJsonObject(json, BaseOrDummy::toJson());
        }

        //! Assign from JSON object
        virtual void convertFromJson(const QJsonObject &json)
        {
            BaseOrDummy::convertFromJson(json);
            JsonPolicy::deserializeImpl(json, *derived());
        }

        //! \copydoc BlackMisc::Mixin::MetaType::toQVariant
        using Mixin::MetaType<Derived>::toQVariant;

        //! \copydoc BlackMisc::Mixin::MetaType::convertFromQVariant
        using Mixin::MetaType<Derived>::convertFromQVariant;

        //! Set property by index
        virtual void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index) { PropertyIndexPolicy::setPropertyByIndex(*derived(), variant, index); }

        //! Property by index
        virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const; // implemented later due to cyclic include dependency

        //! Property by index as String
        virtual QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const { return PropertyIndexPolicy::propertyByIndexAsString(*derived(), index, i18n); }

        //! Is given variant equal to value of property index?
        virtual bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const { return PropertyIndexPolicy::equalsPropertyByIndex(*derived(), compareValue, index); }

        //! \copydoc BlackMisc::Mixin::MetaType::isA
        using Mixin::MetaType<Derived>::isA;

        //! \copydoc BlackMisc::Mixin::MetaType::registerMetadata
        using Mixin::MetaType<Derived>::registerMetadata;

    protected:
        template <typename T>
        friend struct Private::CValueObjectMetaInfo;

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

        //! String for streaming operators
        virtual QString stringForStreaming() const { return this->convertToQString(); }

        //! \copydoc BlackMisc::Mixin::MetaType::getMetaTypeId
        using Mixin::MetaType<Derived>::getMetaTypeId;

    public:
        //! Marshall to DBus
        virtual void marshallToDbus(QDBusArgument &argument) const
        {
            BaseOrDummy::marshallToDbus(argument);
            DBusPolicy::marshallImpl(argument, *derived());
        }

        //! Unmarshall from DBus
        virtual void unmarshallFromDbus(const QDBusArgument &argument)
        {
            BaseOrDummy::unmarshallFromDbus(argument);
            DBusPolicy::unmarshallImpl(argument, *derived());
        }

    private:
        const Derived *derived() const { return static_cast<const Derived *>(this); }
        Derived *derived() { return static_cast<Derived *>(this); }
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
        template <class Derived>
        CVariant MetaType<Derived>::toCVariant() const
        {
            return CVariant(derived()->toQVariant());
        }
        template <class Derived>
        void MetaType<Derived>::convertFromCVariant(const CVariant &variant)
        {
            derived()->convertFromQVariant(variant.getQVariant());
        }
    }
    template <class Derived, class Base>
    CPropertyIndexList CValueObject<Derived, Base>::apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues)
    {
        CPropertyIndexList result;
        PropertyIndexPolicy::apply(*derived(), indexMap, result, skipEqualValues);
        return result;
    }
    template <class Derived, class Base>
    CVariant CValueObject<Derived, Base>::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        CVariant result;
        PropertyIndexPolicy::propertyByIndex(*derived(), index, result);
        return result;
    }
    namespace Policy
    {
        namespace PropertyIndex
        {
            template <class T, class...>
            void Default::apply(T &obj, const CPropertyIndexVariantMap &indexMap, CPropertyIndexList &o_changed, bool skipEqualValues, Default::EnableIfEmptyBase<T>)
            {
                if (indexMap.isEmpty()) return;

                const auto &map = indexMap.map();
                for (auto it = map.begin(); it != map.end(); ++it)
                {
                    const CVariant value = it.value().toCVariant();
                    const CPropertyIndex index = it.key();
                    if (skipEqualValues)
                    {
                        bool equal = obj.equalsPropertyByIndex(value, index);
                        if (equal) { continue; }
                    }
                    obj.setPropertyByIndex(value, index);
                    o_changed.push_back(index);
                }
            }
            template <class T, class...>
            void Default::setPropertyByIndex(T &obj, const CVariant &variant, const CPropertyIndex &index, Default::EnableIfEmptyBase<T>)
            {
                if (index.isMyself())
                {
                    obj.convertFromCVariant(variant);
                    return;
                }

                // not all classes have implemented nesting
                const QString m = QString("Property by index not found (setter), index: ").append(index.toQString());
                qFatal("%s", qPrintable(m));
            }
            template <class T, class...>
            void Default::propertyByIndex(const T &obj, const CPropertyIndex &index, CVariant &o_property, Default::EnableIfEmptyBase<T>)
            {
                if (index.isMyself())
                {
                    o_property = obj.toCVariant();
                    return;
                }
                using Base = CValueObject<T, typename T::base_type>;
                auto i = index.frontCasted<typename CValueObject<T, typename T::base_type>::ColumnIndex>();
                switch (i)
                {
                case Base::IndexIcon:
                    o_property = CVariant::from(obj.toIcon());
                    return;
                case Base::IndexPixmap:
                    o_property = CVariant::from(obj.toPixmap());
                    return;
                case Base::IndexString:
                    o_property = CVariant(obj.toQString());
                    return;
                default:
                    break;
                }

                // not all classes have implemented nesting
                const QString m = QString("Property by index not found, index: ").append(index.toQString());
                qFatal("%s", qPrintable(m));
            }
            template <class T, class...>
            QString Default::propertyByIndexAsString(const T &obj, const CPropertyIndex &index, bool i18n, Default::EnableIfEmptyBase<T>)
            {
                // default implementation, requires propertyByIndex
                return obj.propertyByIndex(index).toQString(i18n);
            }
            template <class T, class...>
            bool Default::equalsPropertyByIndex(const T &obj, const CVariant &compareValue, const CPropertyIndex &index, Default::EnableIfEmptyBase<T>)
            {
                return obj.propertyByIndex(index) == compareValue;
            }
        }
    }
}

#endif // guard
