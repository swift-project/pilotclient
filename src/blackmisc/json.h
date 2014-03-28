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
    //! \remarks JSONVALUE: QJsonValue / QJsonValueRef

    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, int &value)
    {
        value = json.toInt();
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, qlonglong &value)
    {
        value = static_cast<qlonglong>(json.toInt());
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, qulonglong &value)
    {
        value = static_cast<qulonglong>(json.toInt());
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, uint &value)
    {
        value = static_cast<uint>(json.toInt());
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, qint16 &value)
    {
        value = static_cast<qint16>(json.toInt());
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, QString &value)
    {
        value = json.toString();
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, double &value)
    {
        value = json.toDouble();
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, bool &value)
    {
        value = json.toBool();
        return json;
    }
    template<class JSONVALUE> typename
    std::enable_if < std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value, JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, QDateTime &value)
    {
        value = QDateTime::fromString(json.toString());
        return json;
    }

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
    template<class JSONVALUE, class ENUM> typename
    std::enable_if < std::is_enum<ENUM>::value  &&(std::is_same<JSONVALUE, QJsonValue>::value || std::is_same<JSONVALUE, QJsonValueRef>::value), JSONVALUE >::type
    const &operator>>(const JSONVALUE &json, ENUM &value)
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
