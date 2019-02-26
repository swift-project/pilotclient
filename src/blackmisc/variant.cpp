/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \cond PRIVATE

#include "blackmisc/imageutils.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/icon.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variantlist.h"
#include "blackmisc/variant.h"

#include <QByteArray>
#include <QChar>
#include <QDBusArgument>
#include <QDBusVariant>
#include <QDate>
#include <QFlags>
#include <QHash>
#include <QJsonArray>
#include <QJsonValue>
#include <QStringList>
#include <QTime>
#include <Qt>
#include <QtDebug>

namespace BlackMisc
{
    Private::IValueObjectMetaInfo *Private::getValueObjectMetaInfo(int typeId)
    {
        return getValueObjectMetaInfo(QVariant(typeId, nullptr));
    }

    Private::IValueObjectMetaInfo *Private::getValueObjectMetaInfo(const QVariant &v)
    {
        return v.value<IValueObjectMetaInfo *>();
    }

    bool CVariant::canConvert(int typeId) const
    {
        if (m_v.canConvert(typeId))
        {
            return true;
        }
        if (typeId == qMetaTypeId<CVariantList>())
        {
            return m_v.canConvert<QVector<CVariant>>() || m_v.canConvert<QVariantList>();
        }
        if (userType() == qMetaTypeId<CVariantList>())
        {
            return QVariant::fromValue(QVector<CVariant>()).canConvert(typeId)
                || QVariant(typeId, nullptr).canConvert<QVariantList>();
        }
        return false;
    }

    bool CVariant::convert(int typeId)
    {
        if (!m_v.canConvert(typeId))
        {
            if (!canConvert(typeId)) { return false; }
            if (typeId == qMetaTypeId<CVariantList>())
            {
                if (m_v.canConvert<QVector<CVariant>>())
                {
                    if (!m_v.convert(qMetaTypeId<QVector<CVariant>>())) { return false; }
                }
                else if (m_v.canConvert<QVariantList>())
                {
                    m_v.setValue(CVariantList(m_v.value<QSequentialIterable>()));
                }
                else { return false; }
            }
            if (userType() == qMetaTypeId<CVariantList>())
            {
                if (QVariant::fromValue(QVector<CVariant>()).canConvert(typeId))
                {
                    if (!m_v.convert(qMetaTypeId<QVector<CVariant>>())) { return false; }
                }
                else { return false; }
            }
        }
        return m_v.convert(typeId);
    }

    bool CVariant::isVariantList() const
    {
        return userType() == qMetaTypeId<CVariantList>();
    }

    QString CVariant::convertToQString(bool i18n) const
    {
        auto *meta = getValueObjectMetaInfo();
        if (meta)
        {
            return meta->toQString(data(), i18n);
        }
        return m_v.toString();
    }

    bool CVariant::isIntegral() const
    {
        switch (type())
        {
        case QMetaType::Bool: case QMetaType::Char: case QMetaType::UChar: case QMetaType::SChar: case QMetaType::Short: case QMetaType::UShort:
        case QMetaType::Int: case QMetaType::UInt: case QMetaType::Long: case QMetaType::ULong: case QMetaType::LongLong: case QMetaType::ULongLong:
            return true;
        default:
            return false;
        }
    }

    bool CVariant::isArithmetic() const
    {
        return isIntegral() || type() == QMetaType::Float || type() == QMetaType::Double;
    }

    qint64 CVariant::toQInt64(bool *ok) const
    {
        if (this->type() == QMetaType::LongLong) { return this->toLongLong(ok); }
        return this->toInt(ok);
    }

    int CVariant::compareImpl(const CVariant &a, const CVariant &b)
    {
        if (a.userType() < b.userType()) { return -1; }
        if (a.userType() > b.userType()) { return 1; }
        auto *aMeta = a.getValueObjectMetaInfo();
        auto *bMeta = b.getValueObjectMetaInfo();
        if (aMeta && bMeta)
        {
            try
            {
                const void *casted = nullptr;
                if ((casted = aMeta->upCastTo(a.data(), bMeta->getMetaTypeId())))
                {
                    return bMeta->compareImpl(casted, b.data());
                }
                else if ((casted = bMeta->upCastTo(b.data(), aMeta->getMetaTypeId())))
                {
                    return aMeta->compareImpl(a.data(), casted);
                }
                else
                {
                    CLogMessage(&a).warning(u"Comparing two CVariants containing unrelated value objects: %1 (%2) and %3 (%4)")
                            << a.typeName() << a.userType() << b.typeName() << b.userType();
                    return 0;
                }
            }
            catch (const Private::CVariantException &ex)
            {
                CLogMessage(&a).debug() << ex.what();
                return 0;
            }
        }
        if (a.m_v < b.m_v) { return -1; }
        if (a.m_v > b.m_v) { return 1; }
        return 0;
    }

    QJsonObject CVariant::toJson() const
    {
        QJsonObject json;
        json.insert("type", this->typeName());

        switch (m_v.type())
        {
        case QVariant::Invalid:     json.insert("value", 0); break;
        case QVariant::Int:         json.insert("value", m_v.toInt()); break;
        case QVariant::UInt:        json.insert("value", m_v.toInt()); break;
        case QVariant::Bool:        json.insert("value", m_v.toBool()); break;
        case QVariant::Double:      json.insert("value", m_v.toDouble()); break;
        case QVariant::LongLong:    json.insert("value", m_v.toLongLong()); break;
        case QVariant::ULongLong:   json.insert("value", m_v.toLongLong()); break;
        case QVariant::String:      json.insert("value", m_v.toString()); break;
        case QVariant::Char:        json.insert("value", m_v.toString()); break;
        case QVariant::ByteArray:   json.insert("value", m_v.toString()); break;
        case QVariant::DateTime:    json.insert("value", m_v.toDateTime().toString(Qt::ISODate)); break;
        case QVariant::Date:        json.insert("value", m_v.toDate().toString(Qt::ISODate)); break;
        case QVariant::Time:        json.insert("value", m_v.toTime().toString(Qt::ISODate)); break;
        case QVariant::StringList:  json.insert("value", QJsonArray::fromStringList(m_v.toStringList())); break;
        default:
            try
            {
                auto *meta = getValueObjectMetaInfo();
                if (meta)
                {
                    json.insert("value", meta->toJson(data()));
                }
                else if (m_v.canConvert<QString>())
                {
                    json.insert("value", m_v.toString());
                }
                else
                {
                    CLogMessage(this).warning(u"Unsupported CVariant type for toJson: %1 (%2)") << typeName() << userType();
                }
            }
            catch (const Private::CVariantException &ex)
            {
                CLogMessage(this).debug() << ex.what();
            }
        }
        return json;
    }

    QString CVariant::toJsonString(QJsonDocument::JsonFormat format) const
    {
        QJsonDocument jsonDoc(toJson());
        return jsonDoc.toJson(format);
    }

    void CVariant::convertFromJson(const QJsonObject &json)
    {
        // Remark: Names "type" and "value" are also used for drag and drop
        // Changing the names here requires the change for drag and drop too
        const QJsonValue typeValue = json.value("type");
        if (typeValue.isUndefined()) { throw CJsonException("Missing 'type'"); }
        const QString typeName = typeValue.toString();
        if (typeName.isEmpty()) { m_v.clear(); return; }
        const int typeId = QMetaType::type(qPrintable(typeName));

        const QJsonValue value = json.value("value");
        if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }
        switch (typeId)
        {
        case QVariant::Invalid:     throw CJsonException("Type not recognized by QMetaType");
        case QVariant::Int:         m_v.setValue(value.toInt()); break;
        case QVariant::UInt:        m_v.setValue<uint>(static_cast<uint>(value.toInt())); break;
        case QVariant::Bool:        m_v.setValue(value.toBool()); break;
        case QVariant::Double:      m_v.setValue(value.toDouble()); break;
        case QVariant::LongLong:    m_v.setValue(static_cast<qlonglong>(value.toDouble())); break;
        case QVariant::ULongLong:   m_v.setValue(static_cast<qulonglong>(value.toDouble())); break;
        case QVariant::String:      m_v.setValue(value.toString()); break;
        case QVariant::Char:        m_v.setValue(value.toString().size() > 0 ? value.toString().at(0) : '\0'); break;
        case QVariant::ByteArray:   m_v.setValue(value.toString().toLatin1()); break;
        case QVariant::DateTime:    m_v.setValue(fromStringUtc(value.toString(), Qt::ISODate)); break;
        case QVariant::Date:        m_v.setValue(QDate::fromString(value.toString(), Qt::ISODate)); break;
        case QVariant::Time:        m_v.setValue(QTime::fromString(value.toString(), Qt::ISODate)); break;
        case QVariant::StringList:  m_v.setValue(QVariant(value.toArray().toVariantList()).toStringList()); break;
        default:
            try
            {
                auto *meta = Private::getValueObjectMetaInfo(typeId);
                if (meta)
                {
                    CJsonScope scope("value"); // tracker
                    Q_UNUSED(scope);
                    m_v = QVariant(typeId, nullptr);

                    // this will call convertFromJson if there is no MemoizedJson
                    meta->convertFromMemoizedJson(value.toObject(), data(), true);
                }
                else if (QMetaType::hasRegisteredConverterFunction(qMetaTypeId<QString>(), typeId))
                {
                    m_v.setValue(value.toString());
                    if (! m_v.convert(typeId))
                    {
                        throw CJsonException("Failed to convert from JSON string");
                    }
                }
                else
                {
                    throw CJsonException("Type not supported by convertFromJson");
                }
            }
            catch (const Private::CVariantException &ex)
            {
                throw CJsonException(ex.what());
            }
        }
    }

    CStatusMessage CVariant::convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromJson(json);
        }
        catch (const CJsonException &ex)
        {
            return ex.toStatusMessage(categories, prefix);
        }
        return {};
    }

    QJsonObject CVariant::toMemoizedJson() const
    {
        auto *meta = getValueObjectMetaInfo();
        if (meta)
        {
            try
            {
                QJsonObject json;
                json.insert("type", this->typeName());
                json.insert("value", meta->toMemoizedJson(data()));
                return json;
            }
            catch (const Private::CVariantException &ex)
            {
                CLogMessage(this).debug() << ex.what();
                return {};
            }
        }
        else
        {
            return toJson();
        }
    }

    void CVariant::convertFromMemoizedJson(const QJsonObject &json, bool allowFallbackToJson)
    {
        QJsonValue typeValue = json.value("type");
        if (typeValue.isUndefined()) { throw CJsonException("Missing 'type'"); }
        QString typeName = typeValue.toString();
        if (typeName.isEmpty()) { m_v.clear(); return; }
        int typeId = QMetaType::type(qPrintable(typeName));

        auto *meta = Private::getValueObjectMetaInfo(typeId);
        if (meta)
        {
            try
            {
                QJsonValue value = json.value("value");
                if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }

                CJsonScope scope("value");
                m_v = QVariant(typeId, nullptr);
                meta->convertFromMemoizedJson(value.toObject(), data(), allowFallbackToJson);
            }
            catch (const Private::CVariantException &ex)
            {
                throw CJsonException(ex.what());
            }
        }
        else
        {
            convertFromJson(json);
        }
    }

    CStatusMessage CVariant::convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories, const QString &prefix)
    {
        try
        {
            convertFromMemoizedJson(json, false);
        }
        catch (const CJsonException &ex)
        {
            return ex.toStatusMessage(categories, prefix);
        }
        return {};
    }

    uint CVariant::getValueHash() const
    {
        switch (m_v.type())
        {
        case QVariant::Invalid:     return 0;
        case QVariant::Int:         return qHash(m_v.toInt());
        case QVariant::UInt:        return qHash(m_v.toUInt());
        case QVariant::Bool:        return qHash(m_v.toUInt());
        case QVariant::Double:      return qHash(m_v.toUInt());
        case QVariant::LongLong:    return qHash(m_v.toLongLong());
        case QVariant::ULongLong:   return qHash(m_v.toULongLong());
        case QVariant::String:      return qHash(m_v.toString());
        case QVariant::Char:        return qHash(m_v.toChar());
        case QVariant::ByteArray:   return qHash(m_v.toByteArray());
        default:
            try
            {
                auto *meta = getValueObjectMetaInfo();
                if (meta)
                {
                    return meta->getValueHash(data());
                }
                else if (m_v.canConvert<QString>())
                {
                    return qHash(m_v.toString());
                }
                else
                {
                    CLogMessage(this).warning(u"Unsupported CVariant type for getValueHash: %1 (%2)") << typeName() << userType();
                    return 0;
                }
            }
            catch (const Private::CVariantException &ex)
            {
                CLogMessage(this).debug() << ex.what();
                return 0;
            }
        }
    }

    void CVariant::marshallToDbus(QDBusArgument &arg) const
    {
        if (isValid())
        {
            arg << QString(typeName()) << QDBusVariant(getQVariant());
        }
        else
        {
            arg << QString() << QDBusVariant(QVariant(0));
        }
    }

    /*!
     * 2 functions required for unmarshallFromDbus
     * \internal
     */
    //! @{
    QVariant fixQVariantFromDbusArgument(const QVariant &variant, int localUserType, const QString &typeName);
    QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);
    //! @}

    void CVariant::unmarshallFromDbus(const QDBusArgument &arg)
    {
        QString typeName;
        QDBusVariant dbusVar;
        arg >> typeName >> dbusVar;

        if (typeName.isEmpty())
        {
            *this = CVariant();
        }
        else
        {
            *this = fixQVariantFromDbusArgument(dbusVar.variant(), QMetaType::type(qPrintable(typeName)), typeName);
        }
    }

    void CVariant::marshalToDataStream(QDataStream &stream) const
    {
        stream << m_v;
    }

    void CVariant::unmarshalFromDataStream(QDataStream &stream)
    {
        stream >> m_v;
    }

    void CVariant::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            meta->setPropertyByIndex(data(), variant, index);
        }
        catch (const Private::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
        }
    }

    CVariant CVariant::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            CVariant result;
            meta->propertyByIndex(data(), result, index);
            return result;
        }
        catch (const Private::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return {};
        }
    }

    QString CVariant::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            return meta->propertyByIndexAsString(data(), index, i18n);
        }
        catch (const Private::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return {};
        }
    }

    bool CVariant::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            return meta->equalsPropertyByIndex(data(), compareValue, index);
        }
        catch (const Private::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return false;
        }
    }

    CIcon CVariant::toIcon() const
    {
        auto *meta = getValueObjectMetaInfo();
        if (! meta) { return {}; }
        try
        {
            CIcon result;
            meta->toIcon(data(), result);
            return result;
        }
        catch (const Private::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return {};
        }
    }

    QPixmap CVariant::toPixmap() const
    {
        if (m_v.type() == QVariant::Pixmap) { return m_v.value<QPixmap>(); }
        if (m_v.type() == QVariant::Image)  { return QPixmap::fromImage(m_v.value<QImage>()); }
        if (m_v.type() == QVariant::Icon)   { return iconToPixmap(m_v.value<QIcon>()); }

        return toIcon().toPixmap();
    }

    QVariant fixQVariantFromDbusArgument(const QVariant &variant, int localUserType, const QString &typeName)
    {
        if (localUserType == static_cast<int>(QVariant::Invalid))
        {
            CLogMessage(&variant).warning(u"Invalid type for unmarshall: %1") << typeName;
        }

        // my business?
        if (!variant.canConvert<QDBusArgument>()) { return variant; }

        // complex, user type
        // it has to be made sure, that the cast works
        const QDBusArgument arg = variant.value<QDBusArgument>();
        constexpr int userType = static_cast<int>(QVariant::UserType);
        if (localUserType < userType)
        {
            // complex Qt type, e.g. QDateTime
            return complexQtTypeFromDbusArgument(arg, localUserType);
        }
        else if (QMetaType(localUserType).flags() & QMetaType::IsEnumeration)
        {
            arg.beginStructure();
            int i;
            arg >> i;
            arg.endStructure();

            QVariant valueVariant = QVariant::fromValue(i);
            bool ok = valueVariant.convert(localUserType);
            Q_ASSERT_X(ok, Q_FUNC_INFO, "int could not be converted to enum");
            Q_UNUSED(ok);
            return valueVariant;
        }
        else
        {
            QVariant valueVariant(localUserType, nullptr);
            auto *meta = Private::getValueObjectMetaInfo(valueVariant);
            if (meta)
            {
                meta->unmarshall(arg, valueVariant.data());
                return valueVariant;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "no meta");
            return valueVariant;
        }
    }

    QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type)
    {
        // QDate = 14, QTime = 15, QDateTime = 16, QUrl = 17,

        switch (type)
        {
        case QMetaType::QDateTime:
            {
                QDateTime dt;
                argument >> dt;
                return QVariant::fromValue(dt);
            }
        case QMetaType::QDate:
            {
                QDate date;
                argument >> date;
                return QVariant::fromValue(date);
            }
        case QMetaType::QTime:
            {
                QTime time;
                argument >> time;
                return QVariant::fromValue(time);
            }
        default:
            {
                const char *name = QMetaType::typeName(type);
                qFatal("Type cannot be resolved: %s (%d)", name ? name : "", type);
            }
        }
        return QVariant(); // suppress compiler warning
    }

} // namespace

//! \endcond
