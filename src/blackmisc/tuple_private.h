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

        // Our own implementation of std::index_sequence (because not implemented by MSVC2013)
        //! \private
        //! @{
        template <size_t... Is>
        struct index_sequence
        {
            static const size_t size = sizeof...(Is);
            typedef std::tuple<std::integral_constant<size_t, Is>...> tuple_type;
        };
        template <size_t I, size_t C, size_t... Is>
        struct GenSequence
        {
            typedef typename GenSequence<I + 1, C, Is..., I>::type type;
        };
        template <size_t C, size_t... Is>
        struct GenSequence<C, C, Is...>
        {
            typedef index_sequence<Is...> type;
        };
        template <size_t C>
        using make_index_sequence = typename GenSequence<0, C>::type;
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

        // Applying operations to all elements in a tuple, using index_sequence instead of recursion
        //! \private
        class TupleHelper
        {
        public:
            template <class Tu, size_t... Is>
            static int compare(const Tu &a, const Tu &b, index_sequence<Is...>)
            {
                return compareImpl(std::make_pair(get_ref<Is>(a), get_ref<Is>(b))...);
            }

            template <class Tu, size_t... Is>
            static QDBusArgument &marshall(QDBusArgument &arg, const Tu &tu, index_sequence<Is...>)
            {
                marshallImpl(arg, std::get<Is>(tu)...);
                return arg;
            }

            template <class Tu, size_t... Is>
            static const QDBusArgument &unmarshall(const QDBusArgument &arg, Tu &tu, index_sequence<Is...>)
            {
                unmarshallImpl(arg, std::get<Is>(tu)...);
                return arg;
            }

            template <class Tu, size_t... Is>
            static QDebug debug(QDebug dbg, const Tu &tu, index_sequence<Is...>)
            {
                debugImpl(dbg, std::get<Is>(tu)...);
                return dbg;
            }

            template <class Tu, size_t... Is>
            static uint hash(const Tu &tu, index_sequence<Is...>)
            {
                return hashImpl(qHash(std::get<Is>(tu))...);
            }

            template <class Tu, size_t... Is>
            static void serializeJson(QJsonObject &json, const QStringList &names, const Tu &tu, index_sequence<Is...>)
            {
                serializeJsonImpl(json, std::make_pair(names[Is], std::get<Is>(tu))...);
            }

            template <class Tu, size_t... Is>
            static void deserializeJson(const QJsonObject &json, const QStringList &names, Tu &tu, index_sequence<Is...>)
            {
                deserializeJsonImpl(json, std::make_pair(names[Is], get_ref<Is>(tu))...);
            }

        private:
            template <size_t I, class T>
            static auto get_ref(T &&tu) -> decltype(std::ref(std::get<I>(std::forward<T>(tu))))
            {
                return std::ref(std::get<I>(std::forward<T>(tu)));
            }

            static int compareImpl() { return 0; }
            template <class T, class... Ts>
            static int compareImpl(const std::pair<T, T> &head, const Ts &... tail)
            {
                int result = compareHelper(head.first, head.second, typename std::is_base_of<CValueObject, typename std::decay<T>::type>::type());
                if (result) return result;
                return compareImpl(tail...);
            }

            static void marshallImpl(QDBusArgument &) {}
            template <class T, class... Ts>
            static void marshallImpl(QDBusArgument &arg, const T &head, const Ts &... tail)
            {
                arg << head;
                marshallImpl(arg, tail...);
            }

            static void unmarshallImpl(const QDBusArgument &) {}
            template <class T, class... Ts>
            static void unmarshallImpl(const QDBusArgument &arg, T &head, Ts &... tail)
            {
                arg >> head;
                unmarshallImpl(arg, tail...);
            }

            static void debugImpl(QDebug) {}
            template <class T, class... Ts>
            static void debugImpl(QDebug dbg, const T &head, const Ts &... tail)
            {
                dbg << head;
                debugImpl(dbg, tail...);
            }

            static void serializeJsonImpl(QJsonObject &) {}
            template <class T, class... Ts>
            static void serializeJsonImpl(QJsonObject &json, std::pair<QString, T> head, Ts... tail)
            {
                json << head;
                serializeJsonImpl(json, tail...);
            }

            static void deserializeJsonImpl(const QJsonObject &) {}
            template <class T, class... Ts>
            static void deserializeJsonImpl(const QJsonObject &json, std::pair<QString, T> head, Ts... tail)
            {
                json.value(head.first) >> head.second;
                deserializeJsonImpl(json, tail...);
            }

            static uint hashImpl() { return 0; }
            template <class... Ts>
            static uint hashImpl(uint head, Ts... tail) { return head ^ hashImpl(tail...); }
        };

    } // namespace Private

} // namespace BlackMisc

#endif // guard
