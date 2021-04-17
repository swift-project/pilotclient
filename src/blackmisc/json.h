/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_JSON_H
#define BLACKMISC_JSON_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/inheritancetraits.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/jsonexception.h"

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonValueRef>
#include <QStringList>
#include <QtGlobal>
#include <type_traits>
#include <utility>

class QDateTime;
class QPixmap;
class QStringList;

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
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QStringList &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, std::string &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, double &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, bool &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QDateTime &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QPixmap &value);
BLACKMISC_EXPORT const QJsonValue &operator >>(const QJsonValue &json, QByteArray &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, int &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, qlonglong &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, qulonglong &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, uint &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, qint16 &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, QString &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, std::string &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, QStringList &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, double &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, bool &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, QDateTime &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, QPixmap &value);
BLACKMISC_EXPORT QJsonValueRef operator >>(QJsonValueRef json, QByteArray &value);

//! @}

//! \brief Specialized JSON serialization for enum
//! \remarks needs to be in global namespace
//! \ingroup JSON
//! @{
template<class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonObject>
&operator<<(QJsonObject &json, std::pair<QString, const ENUM &> value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}
template<class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonObject>
&operator<<(QJsonObject &json, std::pair<BlackMisc::CExplicitLatin1String, const ENUM &> value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}
//! @}

//! \brief Specialized JSON serialization for QFlags generated enum
//! \ingroup JSON
//! @{
template<class ENUM>
QJsonObject &operator<<(QJsonObject &json, std::pair<QString, const QFlags<ENUM> &> value)
{
    json.insert(value.first, QJsonValue(static_cast<int>(value.second)));
    return json;
}
template<class ENUM>
QJsonObject &operator<<(QJsonObject &json, std::pair<BlackMisc::CExplicitLatin1String, const QFlags<ENUM> &> value)
{
    json[value.first] = QJsonValue(static_cast<int>(value.second));
    return json;
}
//! @}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template<class ENUM>
std::enable_if_t<std::is_enum_v<ENUM>, QJsonValue>
const &operator>>(const QJsonValue &json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for QFlags enum
//! \ingroup JSON
template<class ENUM>
const QJsonValue &operator>>(const QJsonValue &json, QFlags<ENUM> &value)
{
    value = static_cast<QFlags<ENUM>>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for enum
//! \ingroup JSON
template<class ENUM, typename = std::enable_if_t<std::is_enum_v<ENUM>>>
QJsonValueRef operator>>(QJsonValueRef json, ENUM &value)
{
    value = static_cast<ENUM>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for QFlags enum
//! \ingroup JSON
template<class ENUM>
QJsonValueRef operator>>(QJsonValueRef json, QFlags<ENUM> &value)
{
    value = static_cast<QFlags<ENUM>>(json.toInt());
    return json;
}

//! \brief Specialized JSON deserialization for pair
//! \ingroup JSON
template<class FIRST, class SECOND>
QJsonValueRef operator>>(QJsonValueRef json, std::pair<FIRST, SECOND> &pair)
{
    json.toArray() >> pair.first >> pair.second;
    return json;
}

//! \brief Specialized JSON serialization for pair
//! \ingroup JSON
template<class FIRST, class SECOND>
QJsonArray &operator<<(QJsonArray &json, const std::pair<FIRST, SECOND> &pair)
{
    QJsonArray array;
    return json << QJsonValue(array << pair.first << pair.second);
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
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const std::string &value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const double value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const bool value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QDateTime &value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QPixmap &value);
BLACKMISC_EXPORT QJsonArray &operator<<(QJsonArray &json, const QByteArray &value);
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
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const std::string &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QStringList &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const double &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const bool &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QDateTime &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QPixmap &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<QString, const QByteArray &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const int &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const qint16 &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const qlonglong &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const uint &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const qulonglong &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const QString &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const std::string &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const QStringList &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const double &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const bool &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const QDateTime &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const QPixmap &> &value);
BLACKMISC_EXPORT QJsonObject &operator<<(QJsonObject &json, const std::pair<BlackMisc::CExplicitLatin1String, const QByteArray &> &value);
//! @}

//! \name Streaming operators for QDataStream
//! \ingroup JSON
//! @{
QDataStream &operator<<(QDataStream &s, const std::string &v);
QDataStream &operator>>(QDataStream &s, std::string &v);
//! @}

namespace BlackMisc
{
    namespace Json
    {
        //! Append to first JSON object (concatenate)
        //! \ingroup JSON
        BLACKMISC_EXPORT QJsonObject &appendJsonObject(QJsonObject &target, const QJsonObject &toBeAppended);

        //! JSON Object from string
        //! \ingroup JSON
        BLACKMISC_EXPORT QJsonObject jsonObjectFromString(const QString &json, bool acceptCacheFormat = false);

        //! JSON Object from string
        //! \ingroup JSON
        BLACKMISC_EXPORT QString stringFromJsonObject(const QJsonObject &jsonObject, QJsonDocument::JsonFormat format = QJsonDocument::Indented);

        //! JSON Array from string
        //! \ingroup JSON
        BLACKMISC_EXPORT QJsonArray jsonArrayFromString(const QString &json);

        //! First JSON string object marked as "value"
        BLACKMISC_EXPORT QString firstJsonValueAsString(const QString &json);

        //! First JSON string object marked as "value"
        BLACKMISC_EXPORT QString firstJsonValueAsString(const QJsonObject &json);

        //! First JSON string object marked as "value"
        BLACKMISC_EXPORT int firstJsonValueAsInt(const QString &json, int defaultValue = -1, bool *ok = nullptr);

        //! First JSON string object marked as "value"
        BLACKMISC_EXPORT int firstJsonValueAsInt(const QJsonObject &json, int defaultValue = -1, bool *ok = nullptr);

        //! First JSON string list object marked as "value"
        BLACKMISC_EXPORT QStringList firstJsonValueAsStringList(const QString &json);

        //! First JSON string list object marked as "value"
        BLACKMISC_EXPORT QStringList firstJsonValueAsStringList(const QJsonObject &json);

        //! JSON array to QStringList
        BLACKMISC_EXPORT QStringList arrayToQStringList(const QJsonArray &array);

        //! Creates an incremental json object from two existing objects
        BLACKMISC_EXPORT QJsonObject getIncrementalObject(const QJsonObject &previousObject, const QJsonObject &currentObject);

        //! Merges an incremental json object into an existing one
        BLACKMISC_EXPORT QJsonObject applyIncrementalObject(const QJsonObject &previousObject, const QJsonObject &incrementalObject);

        //! Looks like swift JSON?
        //! \remark Quick check if the string could be a valid JSON string
        BLACKMISC_EXPORT bool looksLikeJson(const QString &json);

        //! Looks like swift JSON?
        //! \remark Quick check if the string could be a valid swift JSON string
        BLACKMISC_EXPORT bool looksLikeSwiftJson(const QString &json);

        //! Looks like a valid swift container JSON object?
        BLACKMISC_EXPORT bool looksLikeSwiftContainerJson(const QJsonObject &object);

        //! Looks like a cache/setting object?
        BLACKMISC_EXPORT bool looksLikeSwiftDataObjectJson(const QJsonObject &object);

        //! Looks like a swift type/value pair?
        BLACKMISC_EXPORT bool looksLikeSwiftTypeValuePairJson(const QJsonObject &object);

        //! Looks like a swift DB format?
        BLACKMISC_EXPORT bool looksLikeSwiftDbJson(const QJsonObject &object);

        //! The value part of a cache/setting object
        //! \remark if data object unstrip from that, otherwise leave unchanged
        //! \remark this is the value ONLY!
        BLACKMISC_EXPORT QJsonObject swiftDataObjectValue(const QJsonObject &object);

        //! The value of a cache/setting object
        //! \remark if data object unstrip from that, otherwise leave unchanged
        //! \remark this is the value ONLY!
        BLACKMISC_EXPORT QJsonObject swiftDataObjectValue(const QString &jsonString);

        //! The type/value of a cache/setting object
        //! \remark if cache object unstrip from that, otherwise leave unchanged
        //! \remark format is type/value
        BLACKMISC_EXPORT QJsonObject unwrapCache(const QJsonObject &object);

        //! The type/value object of a cache/setting object
        //! \remark if cache object unstrip from that, otherwise leave unchanged
        //! \remark format is type/value
        BLACKMISC_EXPORT QJsonObject unwrapCache(const QString &jsonString);

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
    }
}

#endif // guard
