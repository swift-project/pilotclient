/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_JSON_H
#define BLACKMISC_JSON_H

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

namespace BlackMisc
{
    //! \name Streaming operators for QJsonValue (to value)
    //! \ingroup JSON
    //! @{
    const QJsonValue &operator >>(const QJsonValue &json, int &value);
    const QJsonValue &operator >>(const QJsonValue &json, qlonglong &value);
    const QJsonValue &operator >>(const QJsonValue &json, qulonglong &value);
    const QJsonValue &operator >>(const QJsonValue &json, uint &value);
    const QJsonValue &operator >>(const QJsonValue &json, qint16 &value);
    const QJsonValue &operator >>(const QJsonValue &json, QString &value);
    const QJsonValue &operator >>(const QJsonValue &json, double &value);
    const QJsonValue &operator >>(const QJsonValue &json, bool &value);
    const QJsonValue &operator >>(const QJsonValue &json, QDateTime &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, int &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, qlonglong &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, qulonglong &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, uint &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, qint16 &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, QString &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, double &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, bool &value);
    const QJsonValueRef &operator >>(const QJsonValueRef &json, QDateTime &value);
    //! @}

    //! \brief Specialized JSON serialization for enum
    //! \remarks needs to be in global namespace
    //! \ingroup JSON
    template<class ENUM> typename
    std::enable_if<std::is_enum<ENUM>::value, QJsonObject>::type
    &operator<<(QJsonObject &json, std::pair<QString, ENUM> value)
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
    QJsonArray &operator<<(QJsonArray &json, const int value);
    QJsonArray &operator<<(QJsonArray &json, const std::pair<QString, qint16> &value);
    QJsonArray &operator<<(QJsonArray &json, const qlonglong value);
    QJsonArray &operator<<(QJsonArray &json, const uint value);
    QJsonArray &operator<<(QJsonArray &json, const qulonglong value);
    QJsonArray &operator<<(QJsonArray &json, const QString &value);
    QJsonArray &operator<<(QJsonArray &json, const double value);
    QJsonArray &operator<<(QJsonArray &json, const bool value);
    QJsonArray &operator<<(QJsonArray &json, const QDateTime &value);
    //! @}

    //! \name Streaming operators for QJsonObject (from value)
    //! \ingroup JSON
    //! @{
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, int> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qint16> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qlonglong> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, uint> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qulonglong> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, QString> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, double> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, bool> &value);
    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, QDateTime> &value);
    //! @}

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
