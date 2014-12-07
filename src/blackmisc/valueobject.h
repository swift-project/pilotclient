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

    namespace PhysicalQuantities
    {
        template <class MU, class PQ> class CPhysicalQuantity;

        //! Traits class to test whether a class is a physical quantity class. Useful for enable_if.
        template <class T>
        class IsQuantity
        {
            struct yes { char x; };
            struct no { yes x[2]; };
            template <class MU, class PQ> static yes test(const CPhysicalQuantity<MU, PQ> &);
            static no test(...);

        public:
            //! True if and only if T is derived from CPhysicalQuantity.
            static const bool value = sizeof(test(*(T *)0)) == sizeof(yes);
        };
    }

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
     * Base class for value types.
     */
    class CValueObject
    {
        //! Stream << overload to be used in debugging messages
        friend QDebug operator<<(QDebug debug, const CValueObject &uc)
        {
            debug << uc.stringForStreaming();
            return debug;
        }

        //! Operator << based on text stream
        friend QTextStream &operator<<(QTextStream &textStream, const CValueObject &uc)
        {
            textStream << uc.stringForStreaming();
            return textStream;
        }

        //! Operator << when there is no debug stream
        friend QNoDebug operator<<(QNoDebug nodebug, const CValueObject &valueObject)
        {
            Q_UNUSED(valueObject);
            return nodebug;
        }

        //! Operator << for QDataStream
        friend QDataStream &operator<<(QDataStream &stream, const CValueObject &valueObject)
        {
            stream << valueObject.stringForStreaming();
            return stream;
        }

        //! Stream operator << for std::cout
        friend std::ostream &operator<<(std::ostream &ostr, const CValueObject &uc)
        {
            ostr << uc.stringForStreaming().toStdString();
            return ostr;
        }

        //! Unmarshalling operator >>, DBus to object
        friend const QDBusArgument &operator>>(const QDBusArgument &argument, CValueObject &valueObject);

        //! Marshalling operator <<, object to DBus
        friend QDBusArgument &operator<<(QDBusArgument &argument, const CValueObject &valueObject);

        //! Operator == with value map
        friend bool operator==(const CPropertyIndexVariantMap &valueMap, const CValueObject &valueObject);

        //! Operator != with value map
        friend bool operator!=(const CPropertyIndexVariantMap &valueMap, const CValueObject &valueObject);

        //! Operator == with value map
        friend bool operator==(const CValueObject &valueObject, const CPropertyIndexVariantMap &valueMap);

        //! Operator != with value map
        friend bool operator!=(const CValueObject &valueObject, const CPropertyIndexVariantMap &valueMap);

        /*!
         * Compares two instances of related classes
         * and returns an integer less than, equal to, or greater than zero
         * if v1 is less than, equal to, or greater than v2.
         * \pre The runtime types of the two objects must be the same or related by inheritance.
         */
        friend int compare(const CValueObject &v1, const CValueObject &v2);

    public:
        //! Root
        using base_type = CValueObject;

        //! Base class enums
        enum ColumnIndex
        {
            IndexPixmap = 10, // manually set to avoid circular dependencies
            IndexIcon,
            IndexString
        };

        //! Virtual destructor
        virtual ~CValueObject() {}

        //! Cast as QString
        QString toQString(bool i18n = false) const;

        //! Cast to pretty-printed QString
        virtual QString toFormattedQString(bool i18n = false) const;

        //! To std string
        std::string toStdString(bool i18n = false) const;

        //! Update by variant map
        //! \return number of values changed, with skipEqualValues equal values will not be changed
        CPropertyIndexList apply(const BlackMisc::CPropertyIndexVariantMap &indexMap, bool skipEqualValues = false);

        //! Value hash, allows comparisons between QVariants
        virtual uint getValueHash() const = 0;

        //! Virtual method to return QVariant, used with DBus QVariant lists
        virtual QVariant toQVariant() const = 0;

        //! Method to return CVariant
        CVariant toCVariant() const;

        //! Equals another CValueObject in QVariant?
        virtual bool equalsQVariant(const QVariant &qVariant) const;

        //! Set from QVariant
        virtual void convertFromQVariant(const QVariant &variant) = 0;

        //! Set from CVariant
        void convertFromCVariant(const CVariant &variant);

        //! Contribute to JSON object
        virtual QJsonObject toJson() const { QJsonObject json; return json;}

        //! Initialize from JSON object
        virtual void convertFromJson(const QJsonObject &json) { Q_UNUSED(json); }

        //! As icon, not implement by all classes
        virtual CIcon toIcon() const;

        //! As pixmap, required for most GUI views
        virtual QPixmap toPixmap() const;

        //! Set property by index
        virtual void setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index);

        //! Property by index
        virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

        //! Property by index as String
        //! \details Intentionally not abstract, avoiding all classes need to implement this method
        virtual QString propertyByIndexAsString(const CPropertyIndex &index, bool i18n = false) const;

        //! Is given variant equal to value of property index?
        virtual bool equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const;

        //! The stored object as CValueObject
        static const CValueObject *fromQVariant(const QVariant &variant);

    protected:
        template <typename T>
        friend struct Private::CValueObjectMetaInfo;

        //! Default constructor.
        CValueObject() = default;

        //! Copy constructor.
        CValueObject(const CValueObject &) = default;

        //! Copy assignment operator.
        CValueObject &operator =(const CValueObject &) = default;

        //! String for streaming operators
        virtual QString stringForStreaming() const;

        //! String for QString conversion
        virtual QString convertToQString(bool i18n = false) const = 0;

        //! Returns the Qt meta type ID of this object.
        virtual int getMetaTypeId() const = 0;

        /*!
         * Returns true if this object is an instance of the class with the given meta type ID,
         * or one of its subclasses.
         */
        virtual bool isA(int metaTypeId) const { Q_UNUSED(metaTypeId); return false; }

        /*!
         * Compare this value with another value of the same type
         * \param other
         * \return Less than, equal to, or greater than zero if this is
         *         less than, equal to, or greather than other.
         * \pre Other must have the same runtime type as the this object.
         * \remark It is usually safer to use the friend function compare() instead.
         */
        virtual int compareImpl(const CValueObject &other) const = 0;

        //! Marshall to DBus
        virtual void marshallToDbus(QDBusArgument &) const = 0;

        //! Unmarshall from DBus
        virtual void unmarshallFromDbus(const QDBusArgument &) = 0;

        //! Parse from string, e.g. 100km/h
        virtual void parseFromString(const QString &) { qFatal("Not implemented"); }
    };

    //! \private FIXME defined out-of-line because it depends on CValueObject
    template <typename T>
    int Private::CValueObjectMetaInfo<T>::compare(const void *lhs, const void *rhs) const
    {
        // Casting to CValueObject here only because compareImpl is protected and CValueObjectMetaInfo is a friend of CValueObject.
        // TODO: make compareImpl public (and typesafe), then this doesn't need to be defined out-of-line.
        return static_cast<const CValueObject *>(lhs)->compareImpl(*static_cast<const CValueObject *>(rhs));
    }

    /*!
     * Default policy classes for use by CValueObjectStdTuple.
     *
     * The default policies are inherited from the policies of the base class. There is a specialization
     * for the terminating case in which the base class is CValueObject.
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
    };

    /*!
     * Default policy classes for use by CValueObjectStdTuple.
     *
     * Specialization for the terminating case in which the base class is CValueObject.
     */
    template <> struct CValueObjectStdTuplePolicy<CValueObject>
    {
        using MetaType = Policy::MetaType::Default;     //!< Metatype policy
        using Equals = Policy::Equals::MetaTuple;       //!< Equals policy
        using LessThan = Policy::LessThan::MetaTuple;   //!< Less than policy
        using Compare = Policy::Compare::MetaTuple;     //!< Compare policy
        using Hash = Policy::Hash::MetaTuple;           //!< Hash policy
        using DBus = Policy::DBus::MetaTuple;           //!< DBus policy
        using Json = Policy::Json::MetaTuple;           //!< JSon policy
    };

    /*!
     * Standard implementation of CValueObject using meta tuple system.
     *
     * This uses policy-based design. Specialize the class template CValueObjectStdTuplePolicy
     * to specify different policy classes.
     *
     * \tparam Derived  The class which is inheriting from this one (CRTP).
     * \tparam Base     The class which this one shall inherit from (default is CValueObject,
     *                  but this can be changed to create a deeper inheritance hierarchy).
     */
    template <class Derived, class Base = CValueObject> class CValueObjectStdTuple :
        public Base,
        private CValueObjectStdTuplePolicy<Derived>::Equals::template Ops<Derived, Base>,
        private CValueObjectStdTuplePolicy<Derived>::LessThan::template Ops<Derived, Base>
    {
        static_assert(std::is_base_of<CValueObject, Base>::value, "Base must be derived from CValueObject");

        using MetaTypePolicy = typename CValueObjectStdTuplePolicy<Derived>::MetaType;
        using ComparePolicy = typename CValueObjectStdTuplePolicy<Derived>::Compare;
        using HashPolicy = typename CValueObjectStdTuplePolicy<Derived>::Hash;
        using DBusPolicy = typename CValueObjectStdTuplePolicy<Derived>::DBus;
        using JsonPolicy = typename CValueObjectStdTuplePolicy<Derived>::Json;

    public:
        //! Base class
        using base_type = Base;

        //! \copydoc CValueObject::getValueHash()
        virtual uint getValueHash() const override
        {
            return HashPolicy::hashImpl(*derived()) ^ Base::getValueHash();
        }

        //! \copydoc CValueObject::toJson
        virtual QJsonObject toJson() const override
        {
            QJsonObject json = JsonPolicy::serializeImpl(*derived());
            return Json::appendJsonObject(json, Base::toJson());
        }

        //! \copydoc CValueObject::convertFromJson
        virtual void convertFromJson(const QJsonObject &json) override
        {
            Base::convertFromJson(json);
            JsonPolicy::deserializeImpl(json, *derived());
        }

        //! \copydoc CValueObject::toQVariant()
        virtual QVariant toQVariant() const override
        {
            return maybeToQVariant(IsRegisteredQMetaType<Derived>());
        }

        //! \copydoc CValueObject::convertFromQVariant
        virtual void convertFromQVariant(const QVariant &variant) override
        {
            return maybeConvertFromQVariant(variant, IsRegisteredQMetaType<Derived>());
        }

        //! Register metadata
        static void registerMetadata()
        {
            MetaTypePolicy::template registerImpl<Derived>();
        }

    protected:
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

        //! \copydoc CValueObject::getMetaTypeId
        virtual int getMetaTypeId() const override
        {
            return maybeGetMetaTypeId(IsRegisteredQMetaType<Derived>());
        }

        //! \copydoc CValueObject::isA
        virtual bool isA(int metaTypeId) const override
        {
            if (metaTypeId == QMetaType::UnknownType) { return false; }
            if (metaTypeId == maybeGetMetaTypeId(IsRegisteredQMetaType<Derived>())) { return true; }
            return Base::isA(metaTypeId);
        }

        //! \copydoc CValueObject::compareImpl
        virtual int compareImpl(const CValueObject &other) const override
        {
            const auto &otherDerived = static_cast<const Derived &>(other);
            int result = ComparePolicy::compareImpl(*derived(), otherDerived);
            if (result) return result;
            return Base::compareImpl(other);
        }

        //! \copydoc CValueObject::marshallToDbus()
        virtual void marshallToDbus(QDBusArgument &argument) const override
        {
            Base::marshallToDbus(argument);
            DBusPolicy::marshallImpl(argument, *derived());
        }

        //! \copydoc CValueObject::unmarshallFromDbus()
        virtual void unmarshallFromDbus(const QDBusArgument &argument) override
        {
            Base::unmarshallFromDbus(argument);
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

    /*!
     * Non-member non-friend operator for streaming T objects to QDBusArgument.
     * Needed because we can't rely on the friend operator in some cases due to
     * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
     * which matches more types than it can actually handle.
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type const &
    operator>>(const QDBusArgument &argument, T &valueObject)
    {
        return argument >> static_cast<CValueObject &>(valueObject);
    }

    /*!
     * Non-member non-friend operator for streaming T objects from QDBusArgument.
     * Needed because we can't rely on the friend operator in some cases due to
     * an unrelated template for streaming Container<T> in QtDBus/qdbusargument.h
     * which matches more types than it can actually handle.
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QDBusArgument>::type &
    operator<<(QDBusArgument &argument, const T &valueObject)
    {
        return argument << static_cast<CValueObject const &>(valueObject);
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonObject &operator>>(const QJsonObject &json, CValueObject &valueObject)
    {
        valueObject.convertFromJson(json);
        return json;
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonValue &operator>>(const QJsonValue &json, CValueObject &valueObject)
    {
        valueObject.convertFromJson(json.toObject());
        return json;
    }

    //! Non member, non friend operator >> for JSON
    inline const QJsonValueRef &operator>>(const QJsonValueRef &json, CValueObject &valueObject)
    {
        valueObject.convertFromJson(json.toObject());
        return json;
    }

    //! Non member, non friend operator << for JSON
    inline QJsonArray &operator<<(QJsonArray &json, const CValueObject &value)
    {
        json.append(value.toJson());
        return json;
    }

    /*!
     * Non member, non friend operator << for JSON
     * \param json
     * \param value as pair name/value
     * \return
     */
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, QJsonObject>::type &
    operator<<(QJsonObject &json, const std::pair<QString, T> &value)
    {
        json.insert(value.first, QJsonValue(value.second.toJson()));
        return json;
    }

    //! Allow comparison with QVariant, e.g. QVariant == CFrequency ?
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const QVariant &variant, const T &valueObject)
    {
        if (!variant.canConvert<T>()) return false;
        T vuc = variant.value<T>();
        return vuc == valueObject;
    }

    //! Allow comparison with QVariant, e.g. QVariant != CFrequency ?
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const QVariant &variant, const T &valueObject)
    {
        return !(variant == valueObject);
    }

    //! Allow comparison with QVariant, e.g. QVariant == CFrequency ?
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator==(const T &valueObject, const QVariant &variant)
    {
        return variant == valueObject;
    }

    //! Allow comparison with QVariant, e.g. QVariant != CFrequency ?
    template <class T> typename std::enable_if<std::is_base_of<CValueObject, T>::value, bool>::type
    operator!=(const T &valueObject, const QVariant &variant)
    {
        return variant != valueObject;
    }

    //! qHash overload, needed for storing CValueObject in a QSet.
    inline uint qHash(const BlackMisc::CValueObject &value, uint seed = 0)
    {
        return ::qHash(value.getValueHash(), seed);
    }

    // Needed so that our qHash overload doesn't hide the qHash overloads in the global namespace.
    // This will be safe as long as no global qHash has the same signature as ours.
    // Alternative would be to qualify all our invokations of the global qHash as ::qHash.
    using ::qHash;

} // namespace

#endif // guard
