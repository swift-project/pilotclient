/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "variant.h"
#include "blackmiscfreefunctions.h"
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDBusVariant>

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

    QString CVariant::convertToQString(bool i18n) const
    {
        auto *meta = getValueObjectMetaInfo();
        if (meta)
        {
            return meta->toQString(data(), i18n);
        }
        return m_v.toString();
    }

    int CVariant::compareImpl(const CValueObject &otherBase) const
    {
        const auto &other = static_cast<const CVariant &>(otherBase);

        if (userType() < other.userType()) { return -1; }
        if (userType() > other.userType()) { return 1; }
        auto *meta = getValueObjectMetaInfo();
        auto *otherMeta = other.getValueObjectMetaInfo();
        if (meta && otherMeta)
        {
            const void *casted = nullptr;
            if ((casted = meta->upCastTo(data(), otherMeta->getMetaTypeId())))
            {
                return otherMeta->compare(casted, other.data());
            }
            else if ((casted = otherMeta->upCastTo(other.data(), meta->getMetaTypeId())))
            {
                return meta->compare(data(), casted);
            }
            else
            {
                qWarning() << "Comparing two CVariants containing unrelated value objects";
                return 0;
            }
        }
        if (m_v < other.m_v) { return -1; }
        if (m_v > other.m_v) { return 1; }
        return 0;
    }

    QJsonObject CVariant::toJson() const
    {
        QJsonObject json;
        json.insert("type", this->typeName());

        switch (m_v.type())
        {
        case QVariant::Int:         json.insert("value", m_v.toInt());
        case QVariant::UInt:        json.insert("value", m_v.toInt());
        case QVariant::Bool:        json.insert("value", m_v.toBool());
        case QVariant::Double:      json.insert("value", m_v.toDouble());
        case QVariant::LongLong:    json.insert("value", m_v.toLongLong());
        case QVariant::ULongLong:   json.insert("value", m_v.toLongLong());
        case QVariant::String:      json.insert("value", m_v.toString());
        case QVariant::Char:        json.insert("value", m_v.toString());
        case QVariant::ByteArray:   json.insert("value", m_v.toString());
        default:
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
                    qWarning() << "Unsupported CVariant type for toJson";
                }
            }
        }
        return json;
    }

    void CVariant::convertFromJson(const QJsonObject &json)
    {
        QString typeName = json.value("type").toString();
        int typeId = QMetaType::type(qPrintable(typeName));

        switch (typeId)
        {
        case QVariant::Int:         m_v.setValue(json.value("value").toInt());
        case QVariant::UInt:        m_v.setValue<uint>(json.value("value").toInt());
        case QVariant::Bool:        m_v.setValue(json.value("value").toBool());
        case QVariant::Double:      m_v.setValue(json.value("value").toDouble());
        case QVariant::LongLong:    m_v.setValue<qlonglong>(json.value("value").toInt()); // QJsonValue has no toLongLong() method???
        case QVariant::ULongLong:   m_v.setValue<qulonglong>(json.value("value").toInt());
        case QVariant::String:      m_v.setValue(json.value("value").toString());
        case QVariant::Char:        m_v.setValue(json.value("value").toString().size() > 0 ? json.value("value").toString().at(0) : '\0');
        case QVariant::ByteArray:   m_v.setValue(json.value("value").toString().toLatin1());
        default:
            {
                auto *meta = Private::getValueObjectMetaInfo(typeId);
                if (meta)
                {
                    m_v = QVariant(typeId, nullptr);
                    meta->convertFromJson(json.value("value").toObject(), data());
                }
                else if (QMetaType::hasRegisteredConverterFunction(qMetaTypeId<QString>(), typeId))
                {
                    m_v.setValue(json.value("value").toString());
                    if (! m_v.convert(typeId))
                    {
                        qWarning() << "Failed to convert from JSON string";
                    }
                }
                else
                {
                    qWarning() << "Unsupported CVariant type for fromJson";
                }
            }
        }
    }

    uint CVariant::getValueHash() const
    {
        switch (m_v.type())
        {
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
                    qWarning() << "Unsupported CVariant type for getValueHash";
                    return 0;
                }
            }
        }
    }

    void CVariant::marshallToDbus(QDBusArgument &arg) const
    {
        arg << QString(typeName()) << QDBusVariant(getQVariant());
    }

    void CVariant::unmarshallFromDbus(const QDBusArgument &arg)
    {
        QString typeName;
        QDBusVariant dbusVar;
        arg >> typeName >> dbusVar;

        *this = fixQVariantFromDbusArgument(dbusVar.variant(), QMetaType::type(qPrintable(typeName)));
    }

    void CVariant::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        meta->setPropertyByIndex(data(), variant, index);
    }

    CVariant CVariant::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        CVariant result;
        meta->propertyByIndex(data(), result, index);
        return result;
    }

    QString CVariant::propertyByIndexAsString(const CPropertyIndex &index, bool i18n) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        return meta->propertyByIndexAsString(data(), index, i18n);
    }

    bool CVariant::equalsPropertyByIndex(const CVariant &compareValue, const CPropertyIndex &index) const
    {
        auto *meta = getValueObjectMetaInfo();
        Q_ASSERT(meta);
        return meta->equalsPropertyByIndex(data(), compareValue, index);
    }

} // namespace
