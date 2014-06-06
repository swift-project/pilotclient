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

#include <QtGlobal>
#include <QDBusArgument>
#include <QHash>
#include <QJsonObject>
#include <QDateTime>
#include <QString>
#include <QJsonArray>
#include <QDebug>
#include <tuple>
#include <type_traits>
#include <functional>

namespace BlackMisc
{
    class CValueObject;

    namespace Private
    {

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
            return compareHelper(std::get<N>(a), std::get<N>(b), typename std::is_base_of<CValueObject, Element>::type());
        }
        //! @}

        // Helper which will allow us to hook in our own customizations into BlackMisc::tie
        //! \private
        //! @{
        template <class T>
        std::reference_wrapper<T> tieHelper(T &obj)
        {
            return obj;
        }
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
                json << std::make_pair(members.at(N - 1), std::get < N - 1 >(tu));
            }

            template <class Tu>
            static void deserializeJson(const QJsonObject &json, const QStringList &members, Tu &tu)
            {
                TupleHelper < N - 1 >::deserializeJson(json, members, tu);
                json.value(members.at(N - 1)) >> std::get < N - 1 >(tu);
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

    } // namespace Private

} // namespace BlackMisc

#endif // guard
