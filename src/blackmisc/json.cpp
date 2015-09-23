/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "json.h"
#include "blackmiscfreefunctions.h"
#include <QJsonDocument>

const QJsonValue &operator >>(const QJsonValue &json, int &value)
{
    value = json.toInt();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, qlonglong &value)
{
    value = static_cast<qlonglong>(json.toDouble());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, qulonglong &value)
{
    value = static_cast<qulonglong>(json.toDouble());
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
    value = static_cast<qlonglong>(json.toDouble());
    return json;
}

const QJsonValueRef &operator >>(const QJsonValueRef &json, qulonglong &value)
{
    value = static_cast<qulonglong>(json.toDouble());
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

const QJsonValueRef &operator >>(const QJsonValueRef &json, QPixmap &value)
{
    const QString hex(json.toString());
    BlackMisc::pngHexStringToPixmapRef(hex, value);
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QPixmap &value)
{
    const QString hex(json.toString());
    BlackMisc::pngHexStringToPixmapRef(hex, value);
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

QJsonArray &operator<<(QJsonArray &json, const QPixmap &value)
{
    QString pm(BlackMisc::pixmapToPngHexString(value));
    json.append(QJsonValue(pm));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const int &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qint16 &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qulonglong &> &value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qlonglong &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const uint &> &value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QString &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const double &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const bool &> &value)
{
    json.insert(value.first, QJsonValue(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QDateTime &> &value)
{
    json.insert(value.first, QJsonValue(value.second.toString()));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QPixmap &> &value)
{
    QString pm(BlackMisc::pixmapToPngHexString(value.second));
    json.insert(value.first, pm);
    return json;
}

QJsonObject BlackMisc::Json::jsonObjectFromString(const QString &json)
{
    if (json.isEmpty()) { return QJsonObject();}
    QJsonDocument jsonDoc(QJsonDocument::fromJson(json.toUtf8()));
    return jsonDoc.object();
}

QJsonArray BlackMisc::Json::jsonArrayFromString(const QString &json)
{
    if (json.isEmpty()) { return QJsonArray();}
    QJsonDocument jsonDoc(QJsonDocument::fromJson(json.toUtf8()));
    return jsonDoc.array();
}

QJsonObject &BlackMisc::Json::appendJsonObject(QJsonObject &target, const QJsonObject &toBeAppended)
{
    if (toBeAppended.isEmpty()) return target;
    QStringList keys = toBeAppended.keys();
    foreach(const QString & key, keys)
    {
        target.insert(key, toBeAppended.value(key));
    }
    return target;
}
