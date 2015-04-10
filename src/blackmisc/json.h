/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_JSON_H
#define BLACKMISC_JSON_H

#include "blackmiscexport.h"
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
    } // ns
} // ns

#endif // guard
