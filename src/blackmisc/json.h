/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

/*!
    \file
*/

#ifndef BLACKMISC_JSON_H
#define BLACKMISC_JSON_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/tuple.h"
#include "blackmisc/inheritance_traits.h"
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QJsonArray>
#include <QDateTime>
#include <QStringList>
#include <utility>

/*!
 * \defgroup JSON Streaming operators for JSON
 */

//! \name Streaming operators for QJsonValue (to value)
//! \ingroup JSON
//! @{
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, int &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, qlonglong &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, qulonglong &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, uint &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, qint16 &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QString &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, double &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, bool &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QDateTime &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, int &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, qlonglong &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, qulonglong &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, uint &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, qint16 &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, QString &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, double &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, bool &value);
BLACKMISC_EXPORT const QJsonValueRef &operator >>(const QJsonValueRef &json, QDateTime &value);
//! @}

//! \brief Specialized JSON serialization for enum
//! \remarks needs to be in global namespace
//! \ingroup JSON
template<class ENUM> typename
std::enable_if<std::is_enum<ENUM>::value, QJsonObject>::type
&operator<<(QJsonObject &json, std::pair<QString, const ENUM &> value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template<class ENUM> typename
std::enable_if<std::is_enum<ENUM>::value, QJsonValue>::type
const &operator>>(const QJsonValue &json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template<class ENUM> typename
std::enable_if<std::is_enum<ENUM>::value, QJsonValueRef>::type
const &operator>>(const QJsonValueRef &json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \name Streaming operators for QJsonArray (from value)
//! \ingroup JSON
//! @{
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const int value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const std::pair<QString, qint16> &value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const qlonglong value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const uint value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const qulonglong value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QString &value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const double value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const bool value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QDateTime &value);
//! @}

//! \name Streaming operators for QJsonObject (from value)
//! \ingroup JSON
//! @{
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const int &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qint16 &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qlonglong &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const uint &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qulonglong &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QString &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const double &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const bool &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QDateTime &> &value);
//! @}

namespace BlackMisc
{
    namespace Json
    {

        //! \brief Append to first JSON object (concatenate)
        //! \ingroup JSON
        inline QJsonObject &appendJsonObject(QJsonObject &target, const QJsonObject &toBeAppended)
        {
            if (toBeAppended.isEmpty()) return target;
            QStringList keys = toBeAppended.keys();
            foreach(const QString & key, keys)
            {
                target.insert(key, toBeAppended.value(key));
            }
            return target;
        }

    } // Json

    namespace Mixin
    {

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
         * When a derived class and a base class both inherit from Mixin::JsonByTuple,
         * the derived class uses this macro to disambiguate the inherited members.
         */
#       define BLACKMISC_DECLARE_USING_MIXIN_JSON(DERIVED)                          \
            using ::BlackMisc::Mixin::JsonByTuple<DERIVED>::toJson;                 \
            using ::BlackMisc::Mixin::JsonByTuple<DERIVED>::convertFromJson;

    } // Mixin
} // BlackMisc

#endif // guard
