/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "json.h"

const QJsonValue &operator >>(const QJsonValue &json, int &value)
{
    value = json.toInt();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, qlonglong &value)
{
    value = static_cast<qlonglong>(json.toInt());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, qulonglong &value)
{
    value = static_cast<qulonglong>(json.toInt());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, uint &value)
{
    value = static_cast<uint>(json.toInt());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, qint16 &value)
{
    value = static_cast<qint16>(json.toInt());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QString &value)
{
    value = json.toString();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, double &value)
{
    value = json.toDouble();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, bool &value)
{
    value = json.toBool();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QDateTime &value)
{
    value = QDateTime::fromString(json.toString());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, int &value)
{
    value = json.toInt();
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, qlonglong &value)
{
    value = static_cast<qlonglong>(json.toInt());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, qulonglong &value)
{
    value = static_cast<qulonglong>(json.toInt());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, uint &value)
{
    value = static_cast<uint>(json.toInt());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, qint16 &value)
{
    value = static_cast<qint16>(json.toInt());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, QString &value)
{
    value = json.toString();
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, double &value)
{
    value = json.toDouble();
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, bool &value)
{
    value = json.toBool();
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, QDateTime &value)
{
    value = QDateTime::fromString(json.toString());
    return json;
}

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
