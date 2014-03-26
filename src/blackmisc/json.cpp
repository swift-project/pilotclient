/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#include "json.h"

namespace BlackMisc
{
    QJsonArray &operator<<(QJsonArray &json, const int value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const qint16 value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const qlonglong value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const uint value)
    {
        json.append(QJsonValue(static_cast<int>(value)));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const qulonglong value)
    {
        json.append(QJsonValue(static_cast<int>(value)));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const QString &value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const double value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const bool value)
    {
        json.append(QJsonValue(value));
        return json;
    }

    QJsonArray &operator<<(QJsonArray &json, const QDateTime &value)
    {
        json.append(QJsonValue(value.toString()));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, int> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qint16> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qulonglong> &value)
    {
        json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, qlonglong> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, uint> &value)
    {
        json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, QString> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, double> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, bool> &value)
    {
        json.insert(value.first, QJsonValue(value.second));
        return json;
    }

    QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, QDateTime> &value)
    {
        json.insert(value.first, QJsonValue(value.second.toString()));
        return json;
    }


} // BlackMisc
