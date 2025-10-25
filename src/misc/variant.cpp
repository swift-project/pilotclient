// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \cond PRIVATE

#include "misc/variant.h"

#include <QDBusArgument>
#include <QDBusVariant>
#include <QDataStream>
#include <QDate>
#include <QFlags>
#include <QHash>
#include <QJsonArray>
#include <QJsonValue>
#include <QString>
#include <QTime>
#include <Qt>
#include <QtDebug>

#include "misc/imageutils.h"
#include "misc/logmessage.h"
#include "misc/propertyindexref.h"
#include "misc/statusmessage.h"
#include "misc/stringutils.h"

namespace swift::misc
{
    //! \private
    int qMetaTypeId_CVariantList = -1; // referenced in variantlist.cpp

    private_ns::IValueObjectMetaInfo *private_ns::getValueObjectMetaInfo(QMetaType mt)
    {
        return getValueObjectMetaInfo(QVariant(mt, nullptr));
    }

    private_ns::IValueObjectMetaInfo *private_ns::getValueObjectMetaInfo(const QVariant &v)
    {
        return v.value<IValueObjectMetaInfo *>();
    }

    bool CVariant::canConvert(int typeId) const
    {
        const QMetaType mt(typeId);
        if (m_v.canConvert(mt)) { return true; }
        if (typeId == qMetaTypeId_CVariantList)
        {
            return m_v.canConvert<QVector<CVariant>>() || m_v.canConvert<QVariantList>();
        }
        if (userType() == qMetaTypeId_CVariantList)
        {
            return QVariant::fromValue(QVector<CVariant>()).canConvert(mt) ||
                   QVariant(QMetaType(typeId), nullptr).canConvert<QVariantList>();
        }
        return false;
    }

    bool CVariant::convert(int typeId)
    {
        const QMetaType mt(typeId);
        if (!m_v.canConvert(mt))
        {
            if (!canConvert(typeId)) { return false; }
            if (typeId == qMetaTypeId_CVariantList)
            {
                if (m_v.canConvert<QVector<CVariant>>())
                {
                    if (!m_v.convert(QMetaType(qMetaTypeId<QVector<CVariant>>()))) { return false; }
                }
                else if (m_v.canConvert<QVariantList>())
                {
                    QVector<CVariant> vec;
                    const auto seqit = m_v.value<QSequentialIterable>();
                    vec.reserve(seqit.size());
                    for (auto it = seqit.begin(); it != seqit.end(); ++it) { vec.push_back(*it); }
                    m_v.setValue(vec);
                }
                else { return false; }
            }
            if (userType() == qMetaTypeId_CVariantList)
            {
                if (QVariant::fromValue(QVector<CVariant>()).canConvert(mt))
                {
                    if (!m_v.convert(QMetaType(qMetaTypeId<QVector<CVariant>>()))) { return false; }
                }
                else { return false; }
            }
        }
        return m_v.convert(mt);
    }

    bool CVariant::isVariantList() const { return userType() == qMetaTypeId_CVariantList; }

    QString CVariant::convertToQString(bool i18n) const
    {
        auto *meta = getValueObjectMetaInfo();
        if (meta) { return meta->toQString(data(), i18n); }
        return m_v.toString();
    }

    bool CVariant::isIntegral() const
    {
        switch (type())
        {
        case QMetaType::Bool:
        case QMetaType::Char:
        case QMetaType::UChar:
        case QMetaType::SChar:
        case QMetaType::Short:
        case QMetaType::UShort:
        case QMetaType::Int:
        case QMetaType::UInt:
        case QMetaType::Long:
        case QMetaType::ULong:
        case QMetaType::LongLong:
        case QMetaType::ULongLong: return true;
        default: return false;
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
                if (casted = aMeta->upCastTo(a.data(), bMeta->getMetaTypeId()); casted)
                {
                    return bMeta->compareImpl(casted, b.data());
                }
                if (casted = bMeta->upCastTo(b.data(), aMeta->getMetaTypeId()); casted)
                {
                    return aMeta->compareImpl(a.data(), casted);
                }
                CLogMessage(&a).warning(
                    u"Comparing two CVariants containing unrelated value objects: %1 (%2) and %3 (%4)")
                    << a.typeName() << a.userType() << b.typeName() << b.userType();
                return 0;
            }
            catch (const private_ns::CVariantException &ex)
            {
                CLogMessage(&a).debug() << ex.what();
                return 0;
            }
        }
        const QPartialOrdering order = QVariant::compare(a.m_v, b.m_v);
        if (order == QPartialOrdering::Less) { return -1; }
        if (order == QPartialOrdering::Greater) { return 1; }
        return 0; // for "Equivalent" and "Unordered"
    }

    QJsonObject CVariant::toJson() const
    {
        QJsonObject json;
        json.insert("type", this->typeName());

        switch (m_v.metaType().id())
        {
        case QMetaType::UnknownType: json.insert("value", 0); break;
        case QMetaType::Int: json.insert("value", m_v.toInt()); break;
        case QMetaType::UInt: json.insert("value", m_v.toInt()); break;
        case QMetaType::Bool: json.insert("value", m_v.toBool()); break;
        case QMetaType::Double: json.insert("value", m_v.toDouble()); break;
        case QMetaType::LongLong: json.insert("value", m_v.toLongLong()); break;
        case QMetaType::ULongLong: json.insert("value", m_v.toLongLong()); break;
        case QMetaType::QString: json.insert("value", m_v.toString()); break;
        case QMetaType::Char: json.insert("value", m_v.toString()); break;
        case QMetaType::QByteArray: json.insert("value", m_v.toString()); break;
        case QMetaType::QDateTime: json.insert("value", m_v.toDateTime().toString(Qt::ISODate)); break;
        case QMetaType::QDate: json.insert("value", m_v.toDate().toString(Qt::ISODate)); break;
        case QMetaType::QTime: json.insert("value", m_v.toTime().toString(Qt::ISODate)); break;
        case QMetaType::QStringList: json.insert("value", QJsonArray::fromStringList(m_v.toStringList())); break;
        default:
            try
            {
                auto *meta = getValueObjectMetaInfo();
                if (meta) { json.insert("value", meta->toJson(data())); }
                else if (m_v.canConvert<QString>()) { json.insert("value", m_v.toString()); }
                else
                {
                    CLogMessage(this).warning(u"Unsupported CVariant type for toJson: %1 (%2)")
                        << typeName() << userType();
                }
            }
            catch (const private_ns::CVariantException &ex)
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
        if (typeName.isEmpty())
        {
            m_v.clear();
            return;
        }

        const QMetaType mt = QMetaType::fromName(qPrintable(typeName));

        const QJsonValue value = json.value("value");
        if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }
        switch (mt.id())
        {
        case QMetaType::UnknownType: throw CJsonException("Type not recognized by QMetaType");
        case QMetaType::Int: m_v.setValue(value.toInt()); break;
        case QMetaType::UInt: m_v.setValue<uint>(static_cast<uint>(value.toInt())); break;
        case QMetaType::Bool: m_v.setValue(value.toBool()); break;
        case QMetaType::Double: m_v.setValue(value.toDouble()); break;
        case QMetaType::LongLong: m_v.setValue(static_cast<qlonglong>(value.toDouble())); break;
        case QMetaType::ULongLong: m_v.setValue(static_cast<qulonglong>(value.toDouble())); break;
        case QMetaType::QString: m_v.setValue(value.toString()); break;
        case QMetaType::Char: m_v.setValue(value.toString().size() > 0 ? value.toString().at(0) : '\0'); break;
        case QMetaType::QByteArray: m_v.setValue(value.toString().toLatin1()); break;
        case QMetaType::QDateTime: m_v.setValue(fromStringUtc(value.toString(), Qt::ISODate)); break;
        case QMetaType::QDate: m_v.setValue(QDate::fromString(value.toString(), Qt::ISODate)); break;
        case QMetaType::QTime: m_v.setValue(QTime::fromString(value.toString(), Qt::ISODate)); break;
        case QMetaType::QStringList: m_v.setValue(QVariant(value.toArray().toVariantList()).toStringList()); break;
        default:
            try
            {
                auto *meta = private_ns::getValueObjectMetaInfo(mt);
                if (meta)
                {
                    CJsonScope scope("value"); // tracker
                    Q_UNUSED(scope);
                    m_v = QVariant(mt, nullptr);

                    // this will call convertFromJson if there is no MemoizedJson
                    meta->convertFromMemoizedJson(value.toObject(), data(), true);
                }
                else if (QMetaType::hasRegisteredConverterFunction(QMetaType(qMetaTypeId<QString>()), mt))
                {
                    m_v.setValue(value.toString());
                    if (!m_v.convert(mt)) { throw CJsonException("Failed to convert from JSON string"); }
                }
                else { throw CJsonException("Type not supported by convertFromJson"); }
            }
            catch (const private_ns::CVariantException &ex)
            {
                throw CJsonException(ex.what());
            }
        }
    }

    CStatusMessage CVariant::convertFromJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories,
                                                    const QString &prefix)
    {
        try
        {
            convertFromJson(json);
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, categories, prefix);
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
            catch (const private_ns::CVariantException &ex)
            {
                CLogMessage(this).debug() << ex.what();
                return {};
            }
        }
        else { return toJson(); }
    }

    void CVariant::convertFromMemoizedJson(const QJsonObject &json, bool allowFallbackToJson)
    {
        QJsonValue typeValue = json.value("type");
        if (typeValue.isUndefined()) { throw CJsonException("Missing 'type'"); }
        QString typeName = typeValue.toString();
        if (typeName.isEmpty())
        {
            m_v.clear();
            return;
        }
        const QMetaType mt = QMetaType::fromName(qPrintable(typeName));

        auto *meta = private_ns::getValueObjectMetaInfo(mt);
        if (meta)
        {
            try
            {
                QJsonValue value = json.value("value");
                if (value.isUndefined()) { throw CJsonException("Missing 'value'"); }

                CJsonScope scope("value");
                m_v = QVariant(mt, nullptr);
                meta->convertFromMemoizedJson(value.toObject(), data(), allowFallbackToJson);
            }
            catch (const private_ns::CVariantException &ex)
            {
                throw CJsonException(ex.what());
            }
        }
        else { convertFromJson(json); }
    }

    CStatusMessage CVariant::convertFromMemoizedJsonNoThrow(const QJsonObject &json, const CLogCategoryList &categories,
                                                            const QString &prefix)
    {
        try
        {
            convertFromMemoizedJson(json, false);
        }
        catch (const CJsonException &ex)
        {
            return CStatusMessage::fromJsonException(ex, categories, prefix);
        }
        return {};
    }

    size_t CVariant::getValueHash() const
    {
        switch (m_v.metaType().id())
        {
        case QMetaType::UnknownType: return 0;
        case QMetaType::Int: return qHash(m_v.toInt());
        case QMetaType::UInt: return qHash(m_v.toUInt());
        case QMetaType::Bool: return qHash(m_v.toUInt());
        case QMetaType::Double: return qHash(m_v.toUInt());
        case QMetaType::LongLong: return qHash(m_v.toLongLong());
        case QMetaType::ULongLong: return qHash(m_v.toULongLong());
        case QMetaType::QString: return qHash(m_v.toString());
        case QMetaType::Char: return qHash(m_v.toChar());
        case QMetaType::QByteArray: return qHash(m_v.toByteArray());
        default:
            try
            {
                auto *meta = getValueObjectMetaInfo();
                if (meta) { return meta->getValueHash(data()); }
                else if (m_v.canConvert<QString>()) { return qHash(m_v.toString()); }
                else
                {
                    CLogMessage(this).warning(u"Unsupported CVariant type for getValueHash: %1 (%2)")
                        << typeName() << userType();
                    return 0;
                }
            }
            catch (const private_ns::CVariantException &ex)
            {
                CLogMessage(this).debug() << ex.what();
                return 0;
            }
        }
    }

    void CVariant::marshallToDbus(QDBusArgument &arg) const
    {
        if (isValid()) { arg << QString(typeName()) << QDBusVariant(getQVariant()); }
        else { arg << QString() << QDBusVariant(QVariant(0)); }
    }

    //! @{
    /*!
     * 2 functions required for unmarshallFromDbus
     * \internal
     */
    QVariant fixQVariantFromDbusArgument(const QVariant &variant, QMetaType localUserType, const QString &typeName);
    QVariant complexQtTypeFromDbusArgument(const QDBusArgument &argument, int type);
    //! @}

    void CVariant::unmarshallFromDbus(const QDBusArgument &arg)
    {
        QString typeName;
        QDBusVariant dbusVar;
        arg >> typeName >> dbusVar;

        if (typeName.isEmpty()) { *this = CVariant(); }
        else
        {
            *this = fixQVariantFromDbusArgument(dbusVar.variant(), QMetaType::fromName(qPrintable(typeName)), typeName);
        }
    }

    void CVariant::marshalToDataStream(QDataStream &stream) const { stream << m_v; }

    void CVariant::unmarshalFromDataStream(QDataStream &stream) { stream >> m_v; }

    void CVariant::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            meta->setPropertyByIndex(data(), variant, index);
        }
        catch (const private_ns::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
        }
    }

    QVariant CVariant::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            QVariant result;
            meta->propertyByIndex(data(), result, index);
            return result;
        }
        catch (const private_ns::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return {};
        }
    }

    bool CVariant::equalsPropertyByIndex(const CVariant &compareValue, CPropertyIndexRef index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        try
        {
            return meta->equalsPropertyByIndex(data(), compareValue, index);
        }
        catch (const private_ns::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return false;
        }
    }

    CIcons::IconIndex CVariant::toIcon() const
    {
        auto *meta = getValueObjectMetaInfo();
        if (!meta) { return {}; }
        try
        {
            return static_cast<CIcons::IconIndex>(meta->toIcon(data()));
        }
        catch (const private_ns::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return CIcons::StandardIconError16;
        }
    }

    QPixmap CVariant::toPixmap() const
    {
        if (m_v.typeId() == QMetaType::QPixmap) { return m_v.value<QPixmap>(); }
        if (m_v.typeId() == QMetaType::QImage) { return QPixmap::fromImage(m_v.value<QImage>()); }
        if (m_v.typeId() == QMetaType::QIcon) { return iconToPixmap(m_v.value<QIcon>()); }
        return CIcon(toIcon()).toPixmap();
    }

    void CVariant::registerMetadata() { private_ns::MetaTypeHelper<CVariant>::maybeRegisterMetaType(); }

    int CVariant::getMetaTypeId() const { return private_ns::MetaTypeHelper<CVariant>::maybeGetMetaTypeId(); }

    QString CVariant::getClassName() const { return QMetaType(getMetaTypeId()).name(); }

    bool CVariant::isA(int metaTypeId) const
    {
        if (metaTypeId == QMetaType::UnknownType) { return false; }
        if (metaTypeId == getMetaTypeId()) { return true; }
        return false;
    }

    bool CVariant::matches(const CVariant &value) const
    {
        if (!isValid()) { return false; }
        auto *meta = getValueObjectMetaInfo();
        if (!meta)
        {
            CLogMessage(this).warning(u"Invalid type for CVariant::matches: %1") << typeName();
            return false;
        }
        try
        {
            return meta->matches(data(), value);
        }
        catch (const private_ns::CVariantException &ex)
        {
            CLogMessage(this).debug() << ex.what();
            return false;
        }
    }

    QVariant fixQVariantFromDbusArgument(const QVariant &variant, QMetaType localUserType, const QString &typeName)
    {
        if (localUserType.id() == static_cast<int>(QMetaType::UnknownType))
        {
            CLogMessage(&variant).warning(u"Invalid type for unmarshall: %1") << typeName;
        }
        // my business?
        if (!variant.canConvert<QDBusArgument>()) { return variant; }

        // complex, user type
        // it has to be made sure, that the cast works
        const auto arg = variant.value<QDBusArgument>();
        constexpr int userType = static_cast<int>(QMetaType::User);
        if (localUserType.id() < userType)
        {
            // complex Qt type, e.g. QDateTime
            return complexQtTypeFromDbusArgument(arg, localUserType.id());
        }
        else if (QMetaType(localUserType).flags() & QMetaType::IsEnumeration)
        {
            arg.beginStructure();
            int i {};
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
            QVariant valueVariant(QMetaType(localUserType), nullptr);
            auto *meta = private_ns::getValueObjectMetaInfo(valueVariant);
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
            const char *name = QMetaType(type).name();
            qFatal("Type cannot be resolved: %s (%d)", name ? name : "", type);
        }
        }
        return {}; // suppress compiler warning
    }

} // namespace swift::misc

//! \endcond
