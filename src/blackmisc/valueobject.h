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

    //! Traits class to test whether a class is derived from CValueObjectStdTuple.
    //! \todo TemplateIsBaseOf gives incorrect result due to ambiguity if there is more than one specialization of CValueObjectStdTuple which is a base of T.
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
     * Default base class for CValueObjectStdTuple.
     */
    class CEmpty
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
        //! \todo Here because pure virtual. Move to CValueObjectStdTuple when all dynamic polymorphism is removed.
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
     * Terminating base cases for the recursive methods of CValueObjectStdTuple.
     */
    struct CValueObjectDummyBase
    {
        //! Value hash
        static uint getValueHash() { return 0; }

        //! To JSON
        static QJsonObject toJson() { return {}; }

        //! From JSON
        static void convertFromJson(const QJsonObject &) {}

        //! Is a
        static bool isA(int) { return false; }

        //! Marshall to DBus
        static void marshallToDbus(QDBusArgument &) {}

        //! Unmarshall from DBus
        static void unmarshallFromDbus(const QDBusArgument &) {}
    };

    /*!
     * Default policy classes for use by CValueObjectStdTuple.
     *
     * The default policies are inherited from the policies of the base class. There is a specialization
     * for the terminating case in which the base class is CEmpty.
     *
     * Specialize this template to use non-default policies for a particular derived class.
     * Due to the void default template parameter, specializations can inherit from CValueObjectStdTuplePolicy<>
     * so that only the policies which differ from the default need be specified.
     * Policy classes which can be used are defined in namespace BlackMisc::Policy.
     */
    template <class Derived = void> struct CValueObjectStdTuplePolicy
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
     * Default policy classes for use by CValueObjectStdTuple.
     *
     * Specialization for the terminating case in which the base class is CEmpty.
     */
    template <> struct CValueObjectStdTuplePolicy<CEmpty>
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
     * Standard implementation of CValueObject using meta tuple system.
     *
     * This uses policy-based design. Specialize the class template CValueObjectStdTuplePolicy
     * to specify different policy classes.
     *
     * \tparam Derived  The class which is inheriting from this one (CRTP).
     * \tparam Base     The class which this one shall inherit from (default is CEmpty,
     *                  but this can be changed to create a deeper inheritance hierarchy).
     */
    template <class Derived, class Base /*= CEmpty*/> class CValueObjectStdTuple :
        public Base,
        private CValueObjectStdTuplePolicy<Derived>::Equals::template Ops<Derived, Base>,
        private CValueObjectStdTuplePolicy<Derived>::LessThan::template Ops<Derived, Base>,
        private CValueObjectStdTuplePolicy<Derived>::Compare::template Ops<Derived, Base>
    {
        static_assert(std::is_same<CEmpty, Base>::value || IsValueObject<Base>::value, "Base must be either CEmpty or derived from CValueObjectStdTuple");

        using MetaTypePolicy = typename CValueObjectStdTuplePolicy<Derived>::MetaType;
        using HashPolicy = typename CValueObjectStdTuplePolicy<Derived>::Hash;
        using DBusPolicy = typename CValueObjectStdTuplePolicy<Derived>::DBus;
        using JsonPolicy = typename CValueObjectStdTuplePolicy<Derived>::Json;
        using PropertyIndexPolicy = typename CValueObjectStdTuplePolicy<Derived>::PropertyIndex;

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
            static_cast<CValueObjectStdTuple &>(obj).unmarshallFromDbus(arg); // virtual method is protected in Derived
            arg.endStructure();
            return arg;
        }

        //! Marshalling operator <<, object to DBus
        friend QDBusArgument &operator<<(QDBusArgument &arg, const Derived &obj)
        {
            arg.beginStructure();
            static_cast<const CValueObjectStdTuple &>(obj).marshallToDbus(arg); // virtual method is protected in Derived
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
        friend QJsonObject& operator<<(QJsonObject &json, const std::pair<QString, Derived> &value)
        {
            json.insert(value.first, QJsonValue(value.second.toJson()));
            return json;
        }

        //! qHash overload, needed for storing value in a QSet.
        friend uint qHash(const Derived &value, uint seed = 0)
        {
            return qHash(value.getValueHash(), seed);
        }

    public:
        //! Base class
        using base_type = Base;

        //! Destructor
        virtual ~CValueObjectStdTuple() {}

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

        //! Method to return CVariant
        CVariant toCVariant() const;

        //! Set from CVariant
        void convertFromCVariant(const CVariant &variant);

        //! Value hash, allows comparisons between QVariants
        virtual uint getValueHash() const
        {
            return HashPolicy::hashImpl(*derived()) ^ BaseOrDummy::getValueHash();
        }

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

        //! Virtual method to return QVariant, used with DBus QVariant lists
        virtual QVariant toQVariant() const
        {
            return maybeToQVariant(IsRegisteredQMetaType<Derived>());
        }

        //! Set from QVariant
        virtual void convertFromQVariant(const QVariant &variant)
        {
            return maybeConvertFromQVariant(variant, IsRegisteredQMetaType<Derived>());
        }

        //! Set property by index
        virtual void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index) { PropertyIndexPolicy::setPropertyByIndex(*derived(), variant, index); }

        //! Property by index
        virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const; // implemented later due to cyclic include dependency

        //! Property by index as String
        virtual QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const { return PropertyIndexPolicy::propertyByIndexAsString(*derived(), index, i18n); }

        //! Is given variant equal to value of property index?
        virtual bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const { return PropertyIndexPolicy::equalsPropertyByIndex(*derived(), compareValue, index); }

        //! Register metadata
        static void registerMetadata()
        {
            MetaTypePolicy::template registerImpl<Derived>();
        }

    protected:
        template <typename T>
        friend struct Private::CValueObjectMetaInfo;

        //! Default constructor.
        CValueObjectStdTuple() = default;

        //! Template constructor, forwards all arguments to base class constructor.
        //! \todo When our compilers support C++11 inheriting constructors, use those instead.
        template <typename T, typename... Ts, typename = typename std::enable_if<! std::is_same<CValueObjectStdTuple, typename std::decay<T>::type>::value>::type>
        CValueObjectStdTuple(T &&first, Ts &&... args) : Base(std::forward<T>(first), std::forward<Ts>(args)...) {}

        //! Copy constructor.
        CValueObjectStdTuple(const CValueObjectStdTuple &) = default;

        //! Copy assignment operator.
        CValueObjectStdTuple &operator =(const CValueObjectStdTuple &) = default;

        //! String for streaming operators
        virtual QString stringForStreaming() const { return this->convertToQString(); }

        //! Returns the Qt meta type ID of this object.
        virtual int getMetaTypeId() const
        {
            return maybeGetMetaTypeId(IsRegisteredQMetaType<Derived>());
        }

        /*!
         * Returns true if this object is an instance of the class with the given meta type ID,
         * or one of its subclasses.
         */
        virtual bool isA(int metaTypeId) const
        {
            if (metaTypeId == QMetaType::UnknownType) { return false; }
            if (metaTypeId == maybeGetMetaTypeId(IsRegisteredQMetaType<Derived>())) { return true; }
            return BaseOrDummy::isA(metaTypeId);
        }

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

        // fallbacks in case Derived is not a registered meta type
        template <class T> using IsRegisteredQMetaType = std::integral_constant<bool, QMetaTypeId<T>::Defined>;
        static int maybeGetMetaTypeId(std::true_type) { return qMetaTypeId<Derived>(); }
        static int maybeGetMetaTypeId(std::false_type) { return QMetaType::UnknownType; }
        QVariant maybeToQVariant(std::true_type) const { return QVariant::fromValue(*derived()); }
        QVariant maybeToQVariant(std::false_type) const { return {}; }
        void maybeConvertFromQVariant(const QVariant &variant, std::true_type) { BlackMisc::setFromQVariant(derived(), variant); }
        void maybeConvertFromQVariant(const QVariant &variant, std::false_type) { Q_UNUSED(variant); }
    };

} // namespace

// TODO Includes due to cyclic dependencies can be removed when CValueObjectStdTuple is split into parts along policy boundaries.
#include "variant.h"
#include "propertyindex.h"
#include "propertyindexlist.h"
#include "iconlist.h"

// TODO Implementations of templates that must appear after those includes, should be moved at the same time that policies are refactored.
namespace BlackMisc
{
    template <class Derived, class Base>
    CVariant CValueObjectStdTuple<Derived, Base>::toCVariant() const
    {
        return CVariant(this->toQVariant());
    }
    template <class Derived, class Base>
    void CValueObjectStdTuple<Derived, Base>::convertFromCVariant(const CVariant &variant)
    {
        this->convertFromQVariant(variant.getQVariant());
    }
    template <class Derived, class Base>
    CPropertyIndexList CValueObjectStdTuple<Derived, Base>::apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues)
    {
        CPropertyIndexList result;
        PropertyIndexPolicy::apply(*derived(), indexMap, result, skipEqualValues);
        return result;
    }
    template <class Derived, class Base>
    CVariant CValueObjectStdTuple<Derived, Base>::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
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
                using Base = CValueObjectStdTuple<T, typename T::base_type>;
                auto i = index.frontCasted<typename Base::ColumnIndex>();
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
