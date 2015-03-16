/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_VALUEOBJECT_POLICY_H
#define BLACKMISC_VALUEOBJECT_POLICY_H

#include "tuple.h"
#include <QMetaType>
#include <QDBusMetaType>

namespace BlackMisc
{
    class CPropertyIndexList;
    class CPropertyIndexVariantMap;

    template <class>
    struct CValueObjectStdTuplePolicy;

    template <typename T>
    void registerMetaValueType();

    namespace Policy
    {
        namespace Private
        {
            //! \private Alias for the policy of the base class of T
            template <class T>
            using Inherit = CValueObjectStdTuplePolicy<typename T::base_type>;

            //! \private
            using BlackMisc::Private::EncapsulationBreaker;
        }

        namespace MetaType
        {
            //! CValueObjectStdTuple registerMetadata policy which only registers with QMetaType and QtDBus
            struct QMetaTypeAndDBusOnly
            {
                //! Register with QMetaType
                template <class T, class...>
                static void registerImpl() { qRegisterMetaType<T>(); qDBusRegisterMetaType<T>(); }
            };

            //! CValueObjectStdTuple default registerMetadata policy
            struct Default
            {
                //! Register with QMetaType
                template <class T, class...>
                static void registerImpl() { QMetaTypeAndDBusOnly::registerImpl<T>(); maybeRegisterMetaValueType<T>(); }

            private:
                template <class T>
                static void maybeRegisterMetaValueType() { maybeRegisterMetaValueType<T>(std::is_base_of<CValueObject, T>()); }
                template <class T>
                static void maybeRegisterMetaValueType(std::true_type) { BlackMisc::registerMetaValueType<T>(); }
                template <class T>
                static void maybeRegisterMetaValueType(std::false_type) {}
            };

            //! CValueObjectStdTuple registerMetadata policy which inherits the policy of the base class
            struct Inherit
            {
                //! Register with QMetaType
                template <class T, class Base = T>
                static void registerImpl() { Private::Inherit<Base>::MetaType::template registerImpl<T, typename Base::base_type>(); }
            };

            //! CValueObjectStdTuple registerMetadata policy which also registers QList<T>
            struct DefaultAndQList
            {
                //! Register with QMetaType
                template <class T, class...>
                static void registerImpl() { Default::registerImpl<T>(); Default::registerImpl<QList<T>>(); }
            };
        }

        namespace Equals
        {
            //! CValueObjectStdTuple policy for a class which should not have an equals operator
            struct None
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops {};
            };

            //! CValueObjectStdTuple equals operator policy which inherits the policy of the base class
            struct Inherit
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class Base>
                struct Ops : public CValueObjectStdTuplePolicy<Base>::Equals::template Ops<T, typename Base::base_type> {};
            };

            //! CValueObjectStdTuple policy for a class whose meta tuple members can be compared by the equals operator
            struct MetaTuple
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops : private Private::EncapsulationBreaker
                {
                    //! Equals operator
                    friend bool operator ==(const T &a, const T &b) { return Private::EncapsulationBreaker::toMetaTuple(a) == Private::EncapsulationBreaker::toMetaTuple(b) && baseEquals<typename T::base_type>(a, b); }

                    //! Not equals operator
                    friend bool operator !=(const T &a, const T &b) { return !(a == b); }

                private:
                    template <class U> static bool baseEquals(const U &a, const U &b) { return a == b; }
                    template <class U> static bool baseEquals(const CValueObject &, const CValueObject &) { return true; }
                };
            };

            //! Some classes define their own custom equals operator; this policy provides a not equals operator which simply negates the result
            struct OwnEquals
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops
                {
                    //! Not equals operator; class T already has its own equals operator
                    friend bool operator !=(const T &a, const T &b) { return !(a == b); }
                };
            };
        }

        namespace LessThan
        {
            //! CValueObjectStdTuple policy for a class which should not have a less than operator
            struct None
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops {};
            };

            //! CValueObjectStdTuple less than operator policy which inherits the policy of the base class
            struct Inherit
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class Base>
                struct Ops : public CValueObjectStdTuplePolicy<Base>::LessThan::template Ops<T, typename Base::base_type> {};
            };

            //! CValueObjectStdTuple policy for a class whose meta tuple members can be compared by the less than operator
            struct MetaTuple
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops : private Private::EncapsulationBreaker
                {
                    //! Less than operator
                    friend bool operator <(const T &a, const T &b) { return Private::EncapsulationBreaker::toMetaTuple(a) < Private::EncapsulationBreaker::toMetaTuple(b); }

                    //! Greater than operator
                    friend bool operator >(const T &a, const T &b) { return b < a; }

                    //! Greater or equal operator
                    friend bool operator >=(const T &a, const T &b) { return !(a < b); }

                    //! Less or equal operator
                    friend bool operator <=(const T &a, const T &b) { return !(b < a); }
                };
            };

            //! Some classes define their own custom less than operator; this policy implements other comparison operators in terms of this one
            struct OwnLessThan
            {
                //! Inner class template which actually bestows the operators via the Barton-Nackman trick
                template <class T, class>
                struct Ops
                {
                    //! Greater than operator; class T already has its own less than operator
                    friend bool operator >(const T &a, const T &b) { return b < a; }

                    //! Greater or equal operator; class T already has its own less than operator
                    friend bool operator >=(const T &a, const T &b) { return !(a < b); }

                    //! Less or equal operator; class T already has its own less than operator
                    friend bool operator <=(const T &a, const T &b) { return !(b < a); }
                };
            };
        }

        namespace Compare
        {
            //! CValueObjectStdTuple policy for a class without polymorphic comparison support
            struct None
            {
                //! Policy implementation of CValueObject::compareImpl
                template <class T, class...>
                static bool compareImpl(const T &, const T &) { qFatal("Not implemented"); return 0; }
            };

            //! CValueObjectStdTuple polymorphic comparison policy which inherits the policy of the base class
            struct Inherit
            {
                //! Policy implementation of CValueObject::compareImpl
                template <class T, class Base = T>
                static bool compareImpl(const T &a, const T &b) { return Private::Inherit<Base>::Compare::template compareImpl<T, typename Base::base_type>(a, b); }
            };

            //! CValueObjectStdTuple policy for a class with default metatuple-based polymorphic comparison support
            struct MetaTuple : private Private::EncapsulationBreaker
            {
                //! Policy implementation of CValueObject::compareImpl
                template <class T, class...>
                static bool compareImpl(const T &a, const T &b) { return compare(Private::EncapsulationBreaker::toMetaTuple(a), Private::EncapsulationBreaker::toMetaTuple(b)); }
            };

            //! CValueObjectStdTuple policy for a class which implements its own custom poylymorphic comparison support
            struct Own
            {
                //! Policy implementation of CValueObject::compareImpl
                template <class T, class...>
                static bool compareImpl(const T &, const T &) { return 0; }
            };
        }

        namespace Hash
        {
            //! CValueObjectStdTuple policy for a class without hashing support
            struct None
            {
                //! \copydoc BlackMisc::CValueObject::getHashValue
                template <class T, class...>
                static uint hashImpl(const T &) { qFatal("Not implemented"); return 0; }
            };

            //! CValueObjectStdTuple hashing policy which inherits the policy of the base class
            struct Inherit
            {
                //! \copydoc BlackMisc::CValueObject::getHashValue
                template <class T, class Base = T>
                static uint hashImpl(const T &obj) { return Private::Inherit<Base>::Hash::template hashImpl<T, typename Base::base_type>(obj); }
            };

            //! CValueObjectStdTuple policy for a class with default metatuple-based hashing support
            struct MetaTuple : private Private::EncapsulationBreaker
            {
                //! \copydoc BlackMisc::CValueObject::getHashValue
                template <class T, class...>
                static uint hashImpl(const T &obj) { return qHash(Private::EncapsulationBreaker::toMetaTuple(obj)); }
            };

            //! CValueObjectStdTuple policy for a class which implements its own custom hashing support
            struct Own
            {
                //! \copydoc BlackMisc::CValueObject::getHashValue
                template <class T, class...>
                static uint hashImpl(const T &) { return 0; }
            };
        }

        namespace DBus
        {
            //! CValueObjectStdTuple policy for a class without DBus marshalling support
            struct None
            {
                //! \copydoc BlackMisc::CValueObject::marshallToDbus
                template <class T, class...>
                static void marshallImpl(QDBusArgument &, const T &) { qFatal("Not implemented"); }

                //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
                template <class T, class...>
                static void unmarshallImpl(const QDBusArgument &, T &) { qFatal("Not implemented"); }
            };

            //! CValueObjectStdTuple marshalling policy which inherits the policy of the base class
            struct Inherit
            {
                //! \copydoc BlackMisc::CValueObject::marshallToDbus
                template <class T, class Base = T>
                static void marshallImpl(QDBusArgument &arg, const T &obj) { Private::Inherit<Base>::DBus::template marshallImpl<T, typename Base::base_type>(arg, obj); }

                //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
                template <class T, class Base = T>
                static void unmarshallImpl(const QDBusArgument &arg, T &obj) { return Private::Inherit<Base>::DBus::template unmarshallImpl<T, typename Base::base_type>(arg, obj); }
            };

            //! CValueObjectStdTuple policy for a class with default metatuple-based DBus marshalling support
            struct MetaTuple : private Private::EncapsulationBreaker
            {
                //! \copydoc BlackMisc::CValueObject::marshallToDbus
                template <class T, class...>
                static void marshallImpl(QDBusArgument &arg, const T &obj) { arg << Private::EncapsulationBreaker::toMetaTuple(obj); }

                //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
                template <class T, class...>
                static void unmarshallImpl(const QDBusArgument &arg, T &obj) { arg >> Private::EncapsulationBreaker::toMetaTuple(obj); }
            };

            //! CValueObjectStdTuple policy for a class which implements its own custom DBus marshalling support
            struct Own
            {
                //! \copydoc BlackMisc::CValueObject::marshallToDbus
                template <class T, class...>
                static void marshallImpl(QDBusArgument &, const T &) {}

                //! \copydoc BlackMisc::CValueObject::unmarshallFromDbus
                template <class T, class...>
                static void unmarshallImpl(const QDBusArgument &, T &) {}
            };
        }

        namespace Json
        {
            //! CValueObjectStdTuple policy for a class without JSON support
            struct None
            {
                //! \copydoc BlackMisc::serializeJson
                template <class T, class...>
                static QJsonObject serializeImpl(const T &) { qFatal("Not implemented"); return {}; }

                //! \copydoc BlackMisc::deserializeJson
                template <class T, class...>
                static void deserializeImpl(const QJsonObject &, T &) { qFatal("Not implemented"); }
            };

            //! CValueObjectStdTuple JSON policy which inherits the policy of the base class
            struct Inherit
            {
                //! \copydoc BlackMisc::serializeJson
                template <class T, class Base = T>
                static QJsonObject serializeImpl(const T &obj) { return Private::Inherit<Base>::Json::template serializeImpl<T, typename Base::base_type>(obj); }

                //! \copydoc BlackMisc::deserializeJson
                template <class T, class Base = T>
                static void deserializeImpl(const QJsonObject &json, T &obj) { Private::Inherit<Base>::Json::template deserializeImpl<T, typename Base::base_type>(json, obj); }
            };

            //! CValueObjectStdTuple policy for a class with default metatuple-based JSON support
            struct MetaTuple : private Private::EncapsulationBreaker
            {
                //! \copydoc BlackMisc::serializeJson
                template <class T, class...>
                static QJsonObject serializeImpl(const T &obj) { return BlackMisc::serializeJson(Private::EncapsulationBreaker::toMetaTuple(obj)); }

                //! \copydoc BlackMisc::deserializeJson
                template <class T, class...>
                static void deserializeImpl(const QJsonObject &json, T &obj) { BlackMisc::deserializeJson(json, Private::EncapsulationBreaker::toMetaTuple(obj)); }
            };

            //! CValueObjectStdTuple policy for a class which implements its own custom JSON support
            struct Own
            {
                //! \copydoc BlackMisc::serializeJson
                template <class T, class...>
                static QJsonObject serializeImpl(const T &) { return {}; }

                //! \copydoc BlackMisc::deserializeJson
                template <class T, class...>
                static void deserializeImpl(const QJsonObject &, T &) {}
            };
        }

        namespace PropertyIndex
        {
            //! CValueObjectStdTuple policy for PropertyIndex related methods
            struct Default
            {
                //! \private
                template <class T>
                using EnableIfEmptyBase = typename std::enable_if<std::is_same<typename T::base_type, CEmpty>::value>::type *;

                //! \private
                template <class T>
                using DisableIfEmptyBase = typename std::enable_if<! std::is_same<typename T::base_type, CEmpty>::value>::type *;

                //! \copydoc CValueObjectStdTuple::apply
                //! @{
                template <class T, class...>
                static void apply(T &obj, const CPropertyIndexVariantMap &indexMap, CPropertyIndexList &o_changed, bool skipEqualValues, DisableIfEmptyBase<T> = nullptr) { o_changed = obj.T::base_type::apply(indexMap, skipEqualValues); }
                template <class T, class...>
                static void apply(T &obj, const CPropertyIndexVariantMap &indexMap, CPropertyIndexList &o_changed, bool skipEqualValues, EnableIfEmptyBase<T> = nullptr)
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
                //! @}

                //! \copydoc CValueObjectStdTuple::setPropertyByIndex
                //! @{
                template <class T, class...>
                static void setPropertyByIndex(T &obj, const CVariant &variant, const CPropertyIndex &index, DisableIfEmptyBase<T> = nullptr) { return obj.T::base_type::setPropertyByIndex(variant, index); }
                template <class T, class...>
                static void setPropertyByIndex(T &obj, const CVariant &variant, const CPropertyIndex &index, EnableIfEmptyBase<T> = nullptr)
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
                //! @}

                //! \copydoc CValueObjectStdTuple::propertyByIndex
                //! @{
                template <class T, class...>
                static void propertyByIndex(const T &obj, const CPropertyIndex &index, CVariant &o_property, DisableIfEmptyBase<T> = nullptr) { o_property = obj.T::base_type::propertyByIndex(index); }
                template <class T, class...>
                static void propertyByIndex(const T &obj, const CPropertyIndex &index, CVariant &o_property, EnableIfEmptyBase<T> = nullptr)
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
                        break;
                    case Base::IndexPixmap:
                        o_property = CVariant::from(obj.toPixmap());
                        break;
                    case Base::IndexString:
                        o_property = CVariant(obj.toQString());
                        break;
                    default:
                        break;
                    }

                    // not all classes have implemented nesting
                    const QString m = QString("Property by index not found, index: ").append(index.toQString());
                    qFatal("%s", qPrintable(m));
                }
                //! @}

                //! \copydoc CValueObjectStdTuple::propertyByIndexAsString
                //! @{
                template <class T, class...>
                static QString propertyByIndexAsString(const T &obj, const CPropertyIndex &index, bool i18n, DisableIfEmptyBase<T> = nullptr) { return obj.T::base_type::propertyByIndexAsString(index, i18n); }
                template <class T, class...>
                static QString propertyByIndexAsString(const T &obj, const CPropertyIndex &index, bool i18n, EnableIfEmptyBase<T> = nullptr)
                {
                    // default implementation, requires propertyByIndex
                    return obj.propertyByIndex(index).toQString(i18n);
                }
                //! @}

                //! \copydoc CValueObjectStdTuple::equalsPropertyByIndex
                //! @{
                template <class T, class...>
                static bool equalsPropertyByIndex(const T &obj, const CVariant &compareValue, const CPropertyIndex &index, DisableIfEmptyBase<T> = nullptr) { return obj.T::base_type::equalsPropertyByIndex(compareValue, index); }
                template <class T, class...>
                static bool equalsPropertyByIndex(const T &obj, const CVariant &compareValue, const CPropertyIndex &index, EnableIfEmptyBase<T> = nullptr)
                {
                    return obj.propertyByIndex(index) == compareValue;
                }
                //! @}
            };
        }
    }
}

#endif
