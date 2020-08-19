/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/json.h"
#include "blackmisc/imageutils.h"
#include "blackmisc/stringutils.h"

#include <QDateTime>
#include <QJsonDocument>
#include <QList>
#include <QStringList>

using namespace BlackMisc;

class QPixmap;

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

const QJsonValue &operator >>(const QJsonValue &json, std::string &value)
{
    value = json.toString().toStdString();
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QStringList &value)
{
    for (auto &&element : json.toArray()) { value << element.toString(); }
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
    value = fromStringUtc(json.toString());
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QPixmap &value)
{
    const QString hex(json.toString());
    BlackMisc::pngHexStringToPixmapRef(hex, value);
    return json;
}

const QJsonValue &operator >>(const QJsonValue &json, QByteArray &value)
{
    const QString hex(json.toString());
    value = BlackMisc::byteArrayFromHexString(hex);
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, int &value)
{
    value = json.toInt();
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, qlonglong &value)
{
    value = static_cast<qlonglong>(json.toDouble());
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, qulonglong &value)
{
    value = static_cast<qulonglong>(json.toDouble());
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, uint &value)
{
    value = static_cast<uint>(json.toInt());
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, qint16 &value)
{
    value = static_cast<qint16>(json.toInt());
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, QString &value)
{
    value = json.toString();
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, std::string &value)
{
    value = json.toString().toStdString();
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, QStringList &value)
{
    for (auto &&element : json.toArray()) { value << element.toString(); }
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, double &value)
{
    value = json.toDouble();
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, bool &value)
{
    value = json.toBool();
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, QDateTime &value)
{
    value = fromStringUtc(json.toString());
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, QPixmap &value)
{
    const QString hex(json.toString());
    BlackMisc::pngHexStringToPixmapRef(hex, value);
    return json;
}

QJsonValueRef operator >>(QJsonValueRef json, QByteArray &value)
{
    const QString hex(json.toString());
    value = BlackMisc::byteArrayFromHexString(hex);
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

QJsonArray &operator<<(QJsonArray &json, const std::string &value)
{
    json.append(QJsonValue(QString::fromStdString(value)));
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

QJsonArray &operator<<(QJsonArray &json, const QByteArray &value)
{
    QString pm(BlackMisc::byteArrayFromHexString(value));
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

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const std::string &> &value)
{
    json[value.first] = QJsonValue(QString::fromStdString(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QStringList &> &value)
{
    json.insert(value.first, QJsonValue(QJsonArray::fromStringList(value.second)));
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

QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QByteArray &> &value)
{
    QString pm(BlackMisc::bytesToHexString(value.second));
    json.insert(value.first, pm);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const int &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const qint16 &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const qulonglong &> &value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const qlonglong &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const uint &> &value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const QString &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const std::string &> &value)
{
    json[value.first] = QJsonValue(QString::fromStdString(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const QStringList &> &value)
{
    json[value.first] = QJsonValue(QJsonArray::fromStringList(value.second));
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const double &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const bool &> &value)
{
    json[value.first] = QJsonValue(value.second);
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const QDateTime &> &value)
{
    json[value.first] = QJsonValue(value.second.toString());
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const QPixmap &> &value)
{
    QString pm(BlackMisc::pixmapToPngHexString(value.second));
    json[value.first] = pm;
    return json;
}

QJsonObject &operator<<(QJsonObject &json, const std::pair<CExplicitLatin1String, const QByteArray &> &value)
{
    QString pm(BlackMisc::bytesToHexString(value.second));
    json[value.first] = pm;
    return json;
}

namespace BlackMisc
{
    namespace Json
    {
        QJsonObject jsonObjectFromString(const QString &json, bool acceptCacheFormat)
        {
            if (json.isEmpty()) { return QJsonObject();}
            const QJsonDocument jsonDoc(QJsonDocument::fromJson(json.toUtf8()));
            return acceptCacheFormat ? Json::unwrapCache(jsonDoc.object()) : jsonDoc.object();
        }

        QString stringFromJsonObject(const QJsonObject &jsonObject, QJsonDocument::JsonFormat format)
        {
            const QJsonDocument doc(jsonObject);
            const QString strJson(doc.toJson(format));
            return strJson;
        }

        QJsonArray jsonArrayFromString(const QString &json)
        {
            if (json.isEmpty()) { return QJsonArray();}
            const QJsonDocument jsonDoc(QJsonDocument::fromJson(json.toUtf8()));
            return jsonDoc.array();
        }

        QJsonObject &appendJsonObject(QJsonObject &target, const QJsonObject &toBeAppended)
        {
            if (toBeAppended.isEmpty()) return target;
            const QStringList keys = toBeAppended.keys();
            foreach (const QString &key, keys)
            {
                target.insert(key, toBeAppended.value(key));
            }
            return target;
        }

        QJsonObject getIncrementalObject(const QJsonObject &previousObject, const QJsonObject &currentObject)
        {
            QJsonObject incrementalObject = currentObject;
            for (const auto &key : previousObject.keys()) // clazy:exclude=range-loop
            {
                if (previousObject.value(key).isObject())
                {
                    auto child = getIncrementalObject(previousObject.value(key).toObject(), currentObject.value(key).toObject());
                    if (child.isEmpty()) incrementalObject.remove(key);
                    else incrementalObject.insert(key, child);
                }
                else
                {
                    if (currentObject.value(key) == previousObject.value(key))
                        incrementalObject.remove(key);
                }
            }
            return incrementalObject;
        }

        QJsonObject applyIncrementalObject(const QJsonObject &previousObject, const QJsonObject &incrementalObject)
        {
            QJsonObject currentObject = previousObject;
            for (const auto &key : incrementalObject.keys()) // clazy:exclude=range-loop
            {
                // If it is not an object, just insert the value
                if (!incrementalObject.value(key).isObject())
                {
                    currentObject.insert(key, incrementalObject.value(key));
                }
                else
                {
                    auto child = applyIncrementalObject(currentObject.value(key).toObject(), incrementalObject.value(key).toObject());
                    currentObject.insert(key, child);
                }
            }
            return currentObject;
        }

        bool looksLikeJson(const QString &json)
        {
            if (json.isEmpty()) { return false; }
            const QString t = json.trimmed();
            return t.startsWith('{') && t.endsWith('}');
        }

        bool looksLikeSwiftJson(const QString &json)
        {
            // further checks would go here
            return looksLikeJson(json);
        }

        bool looksLikeSwiftDataObjectJson(const QJsonObject &object)
        {
            // key
            // - type
            // - value
            if (object.size() != 1) { return false; }
            const QStringList keys = object.keys();
            if (keys.size() != 1) { return false; }
            const QString key = keys.front();
            const QJsonObject cacheObject = object.value(key).toObject();
            return (cacheObject.contains("type") && cacheObject.contains("value"));
        }

        QJsonObject swiftDataObjectValue(const QString &jsonString)
        {
            const QJsonObject obj = jsonObjectFromString(jsonString);
            if (obj.isEmpty()) { return obj; }
            return swiftDataObjectValue(obj);
        }

        QJsonObject swiftDataObjectValue(const QJsonObject &object)
        {
            // key
            // - type
            // - value
            if (object.size() != 1) { return object; } // no cache format
            const QJsonObject cacheObject = object.constBegin()->toObject();
            if (cacheObject.contains("type") && cacheObject.contains("value"))
            {
#ifdef QT_DEBUG
                const QString key = object.constBegin().key(); // clazy:exclude=unused-non-trivial-variable
                const QString type = cacheObject.value("type").toString(); // clazy:exclude=unused-non-trivial-variable
                Q_UNUSED(key);
                Q_UNUSED(type);
#endif
                return cacheObject.value("value").toObject();
            }
            return object;
        }

        QJsonObject unwrapCache(const QJsonObject &object)
        {
            if (object.size() != 1) { return object; } // no cache format
            const QJsonObject cacheObject = object.constBegin()->toObject();
            if (cacheObject.contains("type") && cacheObject.contains("value"))
            {
                // return object in form type/value
                const QString type = cacheObject.value("type").toString(); // just to verify in debugger
                Q_UNUSED(type);
                return cacheObject;
            }
            return object;
        }


        QJsonObject unwrapCache(const QString &jsonString)
        {
            const QJsonObject obj = jsonObjectFromString(jsonString);
            if (obj.isEmpty()) { return obj; }
            return unwrapCache(obj);
        }

        bool looksLikeSwiftContainerJson(const QJsonObject &object)
        {
            // CContainerbase::convertFromJson
            return object.contains("containerbase");
        }

        bool looksLikeSwiftTypeValuePairJson(const QJsonObject &object)
        {
            return (object.contains("type") && object.contains("value"));
        }

        bool looksLikeSwiftDbJson(const QJsonObject &object)
        {
            return (object.contains("data"));
        }

        QString firstJsonValueAsString(const QString &json)
        {
            static const QString empty;
            if (json.isEmpty()) { return empty; }
            const QJsonObject obj = jsonObjectFromString(json);
            return firstJsonValueAsString(obj);
        }

        QString firstJsonValueAsString(const QJsonObject &json)
        {
            static const QString empty;
            if (json.isEmpty()) { return empty; }
            const QStringList keys1 = json.keys();
            if (keys1.isEmpty()) { return empty; }
            if (keys1.contains("value", Qt::CaseInsensitive))
            {
                return json.value("value").toString();
            }

            const QJsonObject jsonLevel1 = json.value(keys1.front()).toObject();
            return jsonLevel1.value("value").toString();
        }

        QStringList firstJsonValueAsStringList(const QString &json)
        {
            static const QStringList empty;
            if (json.isEmpty()) { return empty; }
            const QJsonObject obj = jsonObjectFromString(json);
            return firstJsonValueAsStringList(obj);
        }

        int firstJsonValueAsInt(const QString &json, int defaultValue, bool *ok)
        {
            if (ok) { *ok = false; }
            if (json.isEmpty()) { return defaultValue; }
            const QJsonObject obj = jsonObjectFromString(json);
            return firstJsonValueAsInt(obj, defaultValue, ok);
        }

        int firstJsonValueAsInt(const QJsonObject &json, int defaultValue, bool *ok)
        {
            if (ok) { *ok = false; }
            if (json.isEmpty()) { return defaultValue; }
            const QStringList keys1 = json.keys();
            if (keys1.isEmpty()) { return defaultValue; }
            if (keys1.contains("value", Qt::CaseInsensitive))
            {
                if (ok) { *ok = true; }
                return json.value("value").toInt(defaultValue);
            }

            const QJsonObject jsonLevel1 = json.value(keys1.front()).toObject();
            if (!jsonLevel1.contains("value")) { return defaultValue; }
            if (ok) { *ok = true; }
            return jsonLevel1.value("value").toInt(defaultValue);
        }

        QStringList firstJsonValueAsStringList(const QJsonObject &json)
        {
            static const QStringList empty;
            if (json.isEmpty()) { return empty; }
            const QStringList keys1 = json.keys();
            if (keys1.isEmpty()) { return empty; }
            if (keys1.contains("value", Qt::CaseInsensitive))
            {
                return arrayToQStringList(json.value("value").toArray());
            }

            const QJsonObject jsonLevel1 = json.value(keys1.front()).toObject();
            return arrayToQStringList(jsonLevel1.value("value").toArray());
        }

        QStringList arrayToQStringList(const QJsonArray &array)
        {
            QStringList sl;
            if (array.isEmpty()) { return sl; }
            for (int i = 0; i < array.size(); i++)
            {
                if (!array.at(i).isString()) { continue; }
                sl.push_back(array.at(i).toString());
            }
            return sl;
        }
    } // ns
} // ns

QDataStream &operator<<(QDataStream &s, const std::string &v) { s << QString::fromStdString(v); return s; }
QDataStream &operator>>(QDataStream &s, std::string &v) {  QString vs; s >> vs; v = vs.toStdString(); return s;  }
