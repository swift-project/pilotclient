/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
    Private implementation details used by tuple.h
*/

#ifndef BLACKMISC_TUPLE_PRIVATE_H
#define BLACKMISC_TUPLE_PRIVATE_H

#include "json.h"
#include <QtGlobal>
#include <QDBusArgument>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>
#include <QString>
#include <QDebug>
#include <tuple>
#include <type_traits>
#include <functional>

namespace BlackMisc
{
    class CValueObject;

    namespace Private
    {

#ifdef Q_COMPILER_VARIADIC_TEMPLATES

        // Helper trait to detect whether a class is a tuple.
        //! \private
        //! @{
        template <class T>
        struct IsTuple : public std::false_type {};
        template <class... Ts>
        struct IsTuple<std::tuple<Ts...>> : public std::true_type {};
        //! @}

#else // !Q_COMPILER_VARIADIC_TEMPLATES

        template <class T>
        struct IsTuple : public std::false_type {};
        template <>
        struct IsTuple<std::tuple<>> : public std::true_type {};
        template <class T0>
        struct IsTuple<std::tuple<T0>> : public std::true_type {};
        template <class T0, class T1>
        struct IsTuple<std::tuple<T0, T1>> : public std::true_type {};
        template <class T0, class T1, class T2>
        struct IsTuple<std::tuple<T0, T1, T2>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3>
        struct IsTuple<std::tuple<T0, T1, T2, T3>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4, class T5>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4, T5>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4, class T5, class T6>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4, T5, T6>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4, T5, T6, T7>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8>> : public std::true_type {};
        template <class T0, class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
        struct IsTuple<std::tuple<T0, T1, T2, T3, T4, T5, T6, T7, T8, T9>> : public std::true_type {};

#endif // !Q_COMPILER_VARIADIC_TEMPLATES

        // Using SFINAE to help detect missing BLACK_ENABLE_TUPLE_CONVERSION macro in static_assert
        //! \private
        std::false_type hasEnabledTupleConversionHelper(...);

        //! \private
        template <class T>
        typename T::EnabledTupleConversion hasEnabledTupleConversionHelper(T *);

        //! \private
        template <class T>
        struct HasEnabledTupleConversion
        {
            typedef decltype(hasEnabledTupleConversionHelper(static_cast<T *>(nullptr))) type;
            static const bool value = type::value;
        };

        // Using tag dispatch to select which implementation of compare() to use
        //! \private
        //! @{
        template <class T>
        int compareHelper(const T &a, const T &b, std::true_type isCValueObjectTag)
        {
            Q_UNUSED(isCValueObjectTag);
            return compare(a, b);
        }

        template <class T>
        int compareHelper(const T &a, const T &b, std::false_type isCValueObjectTag)
        {
            Q_UNUSED(isCValueObjectTag);
            if (a < b) { return -1; }
            if (a > b) { return 1; }
            return 0;
        }

        template <int N, class Tu>
        int compareHelper(const Tu &a, const Tu &b)
        {
            typedef typename std::decay<typename std::tuple_element<N, Tu>::type>::type Element;
            typedef std::integral_constant<bool, std::is_base_of<CValueObject, Element>::value || IsTuple<Element>::value> isCValueObjectTag;

            return compareHelper(std::get<N>(a), std::get<N>(b), isCValueObjectTag());
        }
        //! @}

        // Helper which returns a copy of its argument if it's a tuple,
        // otherwise returns a reference to its argument.
        //! \private
        //! @{
        template <class T>
        auto tieHelper(T &obj, std::false_type isTupleTag) -> std::reference_wrapper<T>
        {
            Q_UNUSED(isTupleTag);
            return obj;
        }
        template <class T>
        auto tieHelper(T &obj, std::true_type isTupleTag) -> T
        {
            Q_UNUSED(isTupleTag);
            return obj;
        }
        template <class T>
        auto tieHelper(T &obj) -> decltype(tieHelper(obj, IsTuple<T>()))
        {
            return tieHelper(obj, IsTuple<T>());
        }
        //! @}

        // Helper which (de)serializes its argument to/from JSON,
        // whether it is a simple value or a nested tuple.
        // Defined later because it uses TupleHelper;
        // forward-declared because TupleHelper uses it.
        //! \private
        //! @{
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index, std::false_type isNestedTag);
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index, std::true_type isNestedTag);
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index);
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index, std::false_type isNestedTag);
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index, std::true_type isNestedTag);
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index);
        //! @}

        // Applying operations to all elements in a tuple, using recursion
        //! \private
        template <int N>
        struct TupleHelper
        {
            template <class Tu>
            static int compare(const Tu &a, const Tu &b)
            {
                const int head = TupleHelper < N - 1 >::compare(a, b);
                if (head) { return head; }
                return compareHelper < N - 1 > (a, b);
            }

            template <class Tu>
            static QDBusArgument &marshall(QDBusArgument &arg, const Tu &tu)
            {
                return TupleHelper < N - 1 >::marshall(arg, tu) << std::get < N - 1 > (tu);
            }

            template <class Tu>
            static const QDBusArgument &unmarshall(const QDBusArgument &arg, Tu &tu)
            {
                return TupleHelper < N - 1 >::unmarshall(arg, tu) >> std::get < N - 1 > (tu);
            }

            template <class Tu>
            static QDebug debug(QDebug dbg, Tu &tu)
            {
                return TupleHelper < N - 1 >::debug(dbg, tu) << std::get < N - 1 > (tu);
            }

            template <class Tu>
            static uint hash(const Tu &tu)
            {
                return TupleHelper < N - 1 >::hash(tu) ^ qHash(std::get < N - 1 > (tu));
            }

            template <class Tu>
            static void serializeJson(QJsonObject &json, const QStringList &members, const Tu &tu)
            {
                TupleHelper < N - 1 >::serializeJson(json, members, tu);
                serializeJsonHelper(json, members, std::get < N - 1 > (tu), N - 1);
            }

            template <class Tu>
            static void deserializeJson(const QJsonObject &json, const QStringList &members, Tu &tu)
            {
                TupleHelper < N - 1 >::deserializeJson(json, members, tu);
                deserializeJsonHelper(json, members, std::get < N - 1 > (tu), N - 1);
            }
        };

        //! \private
        template <>
        struct TupleHelper<0>
        {
            template <class Tu>
            static int compare(const Tu &, const Tu &) { return 0; }
            template <class Tu>
            static QDBusArgument &marshall(QDBusArgument &arg, const Tu &) { return arg; }
            template <class Tu>
            static const QDBusArgument &unmarshall(const QDBusArgument &arg, Tu &) { return arg; }
            template <class Tu>
            static QDebug debug(QDebug dbg, Tu &) { return dbg; }
            template <class Tu>
            static uint hash(const Tu &) { return 0; }
            template <class Tu>
            static void serializeJson(QJsonObject &, const QStringList &, const Tu &) {}
            template <class Tu>
            static void deserializeJson(const QJsonObject &, const QStringList &, Tu &) {}
        };

        // definitions of helpers declared earlier
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index, std::false_type isNestedTag)
        {
            Q_UNUSED(isNestedTag);
            json << std::make_pair(members.at(index), obj);
        }
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index, std::true_type isNestedTag)
        {
            Q_UNUSED(isNestedTag);
            TupleHelper<std::tuple_size<T>::value>::serializeJson(json, members.mid(index), obj);
        }
        template <class T>
        static void serializeJsonHelper(QJsonObject &json, const QStringList &members, const T &obj, int index)
        {
            serializeJsonHelper(json, members, obj, index, IsTuple<T>());
        }
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index, std::false_type isNestedTag)
        {
            Q_UNUSED(isNestedTag);
            json.value(members.at(index)) >> obj;
        }
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index, std::true_type isNestedTag)
        {
            Q_UNUSED(isNestedTag);
            TupleHelper<std::tuple_size<T>::value>::deserializeJson(json, members.mid(index), obj);
        }
        template <class T>
        static void deserializeJsonHelper(const QJsonObject &json, const QStringList &members, T &obj, int index)
        {
            deserializeJsonHelper(json, members, obj, index, IsTuple<T>());
        }

    } // namespace Private

} // namespace BlackMisc

#endif // guard
