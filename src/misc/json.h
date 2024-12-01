// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_JSON_H
#define SWIFT_MISC_JSON_H

#include <type_traits>
#include <utility>

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QStringList>
#include <QtGlobal>

#include "misc/fileutils.h"
#include "misc/metaclass.h"
#include "misc/swiftmiscexport.h"

class QDateTime;
class QPixmap;

/*!
 * \defgroup JSON Streaming operators for JSON
 */

//! \ingroup JSON
//! @{
//! Streaming operators for QJsonValue (to value)
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, int &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, qlonglong &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, qulonglong &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, uint &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, qint16 &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, QString &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, QStringList &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, std::string &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, double &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, bool &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, QDateTime &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, QPixmap &value);
SWIFT_MISC_EXPORT const QJsonValue &operator>>(const QJsonValue &json, QByteArray &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, int &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, qlonglong &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, qulonglong &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, uint &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, qint16 &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, QString &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, std::string &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, QStringList &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, double &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, bool &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, QDateTime &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, QPixmap &value);
SWIFT_MISC_EXPORT QJsonValueRef operator>>(QJsonValueRef json, QByteArray &value);

//! @}

//! @{
//! \brief Specialized JSON serialization for enum
//! \remarks needs to be in global namespace
//! \ingroup JSON
template <class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonObject> &operator<<(QJsonObject &json,
                                                                std::pair<QString, const ENUM &> value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}
template <class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonObject> &
operator<<(QJsonObject &json, std::pair<swift::misc::CExplicitLatin1String, const ENUM &> value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}
//! @}

//! @{
//! \brief Specialized JSON serialization for QFlags generated enum
//! \ingroup JSON
template <class ENUM>
QJsonObject &operator<<(QJsonObject &json, std::pair<QString, const QFlags<ENUM> &> value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}
template <class ENUM>
QJsonObject &operator<<(QJsonObject &json, std::pair<swift::misc::CExplicitLatin1String, const QFlags<ENUM> &> value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}
//! @}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template <class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonValue> const &operator>>(const QJsonValue &json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for QFlags enum
//! \ingroup JSON
template <class ENUM>
const QJsonValue &operator>>(const QJsonValue &json, QFlags<ENUM> &value)
{
    value = static_cast<QFlags<ENUM>>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template <class ENUM, typename = std::enable_if_t<std::is_enum_v<ENUM>>>
QJsonValueRef operator>>(QJsonValueRef json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for QFlags enum
//! \ingroup JSON
template <class ENUM>
QJsonValueRef operator>>(QJsonValueRef json, QFlags<ENUM> &value)
{
    value = static_cast<QFlags<ENUM>>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for pair
//! \ingroup JSON
template <class FIRST, class SECOND>
QJsonValueRef operator>>(QJsonValueRef json, std::pair<FIRST, SECOND> &pair)
{
    json.toArray() >> pair.first >> pair.second;
    return json;
}

//! \brief Specialized JSON serialization for pair
//! \ingroup JSON
template <class FIRST, class SECOND>
QJsonArray &operator<<(QJsonArray &json, const std::pair<FIRST, SECOND> &pair)
{
    QJsonArray array;
    return json << QJsonValue(array << pair.first << pair.second);
}

//! @{
//! Streaming operators for QJsonArray (from value)
//! \ingroup JSON
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const int value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const std::pair<QString, qint16> &value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const qlonglong value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const uint value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const qulonglong value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QString &value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const std::string &value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const double value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const bool value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QDateTime &value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QPixmap &value);
SWIFT_MISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QByteArray &value);
//! @}

//! @{
//! Streaming operators for QJsonObject (from value)
//! \ingroup JSON
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const int &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qint16 &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qlonglong &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const uint &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const qulonglong &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QString &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const std::string &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QStringList &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const double &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const bool &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QDateTime &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QPixmap &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QByteArray &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const int &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const qint16 &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const qlonglong &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const uint &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const qulonglong &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const QString &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const std::string &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const QStringList &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const double &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const bool &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const QDateTime &> &value);
SWIFT_MISC_EXPORT QJsonObject &operator<<(QJsonObject &json,
                                          const std::pair<swift::misc::CExplicitLatin1String, const QPixmap &> &value);
SWIFT_MISC_EXPORT QJsonObject &
operator<<(QJsonObject &json, const std::pair<swift::misc::CExplicitLatin1String, const QByteArray &> &value);
//! @}

//! @{
//! Streaming operators for QDataStream
//! \ingroup JSON
QDataStream &operator<<(QDataStream &s, const std::string &v);
QDataStream &operator>>(QDataStream &s, std::string &v);
//! @}

namespace swift::misc::json
{
    //! Append to first JSON object (concatenate)
    //! \ingroup JSON
    SWIFT_MISC_EXPORT QJsonObject &appendJsonObject(QJsonObject &target, const QJsonObject &toBeAppended);

    //! JSON Object from string
    //! \ingroup JSON
    SWIFT_MISC_EXPORT QJsonObject jsonObjectFromString(const QString &json, bool acceptCacheFormat = false);

    //! JSON Object from string
    //! \ingroup JSON
    SWIFT_MISC_EXPORT QString stringFromJsonObject(const QJsonObject &jsonObject,
                                                   QJsonDocument::JsonFormat format = QJsonDocument::Indented);

    //! JSON Array from string
    //! \ingroup JSON
    SWIFT_MISC_EXPORT QJsonArray jsonArrayFromString(const QString &json);

    //! First JSON string object marked as "value"
    SWIFT_MISC_EXPORT QString firstJsonValueAsString(const QString &json);

    //! First JSON string object marked as "value"
    SWIFT_MISC_EXPORT QString firstJsonValueAsString(const QJsonObject &json);

    //! First JSON string object marked as "value"
    SWIFT_MISC_EXPORT int firstJsonValueAsInt(const QString &json, int defaultValue = -1, bool *ok = nullptr);

    //! First JSON string object marked as "value"
    SWIFT_MISC_EXPORT int firstJsonValueAsInt(const QJsonObject &json, int defaultValue = -1, bool *ok = nullptr);

    //! First JSON string list object marked as "value"
    SWIFT_MISC_EXPORT QStringList firstJsonValueAsStringList(const QString &json);

    //! First JSON string list object marked as "value"
    SWIFT_MISC_EXPORT QStringList firstJsonValueAsStringList(const QJsonObject &json);

    //! JSON array to QStringList
    SWIFT_MISC_EXPORT QStringList arrayToQStringList(const QJsonArray &array);

    //! Creates an incremental json object from two existing objects
    SWIFT_MISC_EXPORT QJsonObject getIncrementalObject(const QJsonObject &previousObject,
                                                       const QJsonObject &currentObject);

    //! Merges an incremental json object into an existing one
    SWIFT_MISC_EXPORT QJsonObject applyIncrementalObject(const QJsonObject &previousObject,
                                                         const QJsonObject &incrementalObject);

    //! Looks like swift JSON?
    //! \remark Quick check if the string could be a valid JSON string
    SWIFT_MISC_EXPORT bool looksLikeJson(const QString &json);

    //! Looks like swift JSON?
    //! \remark Quick check if the string could be a valid swift JSON string
    SWIFT_MISC_EXPORT bool looksLikeSwiftJson(const QString &json);

    //! Looks like a valid swift container JSON object?
    SWIFT_MISC_EXPORT bool looksLikeSwiftContainerJson(const QJsonObject &object);

    //! Looks like a cache/setting object?
    SWIFT_MISC_EXPORT bool looksLikeSwiftDataObjectJson(const QJsonObject &object);

    //! Looks like a swift type/value pair?
    SWIFT_MISC_EXPORT bool looksLikeSwiftTypeValuePairJson(const QJsonObject &object);

    //! Looks like a swift DB format?
    SWIFT_MISC_EXPORT bool looksLikeSwiftDbJson(const QJsonObject &object);

    //! The value part of a cache/setting object
    //! \remark if data object unstrip from that, otherwise leave unchanged
    //! \remark this is the value ONLY!
    SWIFT_MISC_EXPORT QJsonObject swiftDataObjectValue(const QJsonObject &object);

    //! The value of a cache/setting object
    //! \remark if data object unstrip from that, otherwise leave unchanged
    //! \remark this is the value ONLY!
    SWIFT_MISC_EXPORT QJsonObject swiftDataObjectValue(const QString &jsonString);

    //! The type/value of a cache/setting object
    //! \remark if cache object unstrip from that, otherwise leave unchanged
    //! \remark format is type/value
    SWIFT_MISC_EXPORT QJsonObject unwrapCache(const QJsonObject &object);

    //! The type/value object of a cache/setting object
    //! \remark if cache object unstrip from that, otherwise leave unchanged
    //! \remark format is type/value
    SWIFT_MISC_EXPORT QJsonObject unwrapCache(const QString &jsonString);

    /*!
     * Load JSON file and init by that
     */
    template <class T>
    bool loadFromJsonFile(T &object, const QString &fileNameAndPath, bool acceptCacheFormat = false)
    {
        const QString jsonString(CFileUtils::readFileToString(fileNameAndPath));
        if (jsonString.isEmpty()) { return false; }
        object.convertFromJson(jsonString, acceptCacheFormat);
        return true;
    }

    /*!
     * Save to JSON file
     */
    template <class T>
    bool saveToJsonFile(const T &object, const QString &fileNameAndPath)
    {
        const QString jsonString(object.toJsonString());
        if (jsonString.isEmpty()) { return false; }
        return CFileUtils::writeStringToFile(jsonString, fileNameAndPath);
    }
} // namespace swift::misc::json

#endif // guard
