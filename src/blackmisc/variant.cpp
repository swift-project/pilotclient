/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "variant.h"
#include "valueobject.h"
#include "blackmiscfreefunctions.h"
#include <QDBusArgument>
#include <QDBusMetaType>
#include <QDBusVariant>

namespace BlackMisc
{

    void CVariant::registerMetadata()
    {
        qRegisterMetaType<CVariant>();
        qDBusRegisterMetaType<CVariant>();
    }

    QString CVariant::toString(bool i18n) const
    {
        if (type() == QVariant::UserType)
        {
            const CValueObject *s = CValueObject::fromQVariant(m_v); // FIXME this will return garbage if value is not a CValueObject
            Q_ASSERT(s);
            if (s)
            {
                return s->toQString(i18n);
            }
            else
            {
                return "No CValueObject, no string conversion";
            }
        }
        return m_v.toString();
    }

    QJsonObject CVariant::toJson() const
    {
        QJsonObject json;
        json.insert("type", static_cast<int>(this->type())); // type
        json.insert("usertype", this->userType()); // user type
        json.insert("typename", this->typeName()); // as tring, mainly for debugging, readablity
        json.insert("value", this->toString(false));
        return json;
    }

    void CVariant::fromJson(const QJsonObject &json)
    {
        int type = json.value("type").toInt(-1);
        int userType = json.value("usertype").toInt(-1);
        QString typeName = json.value("typename").toString();
        QString value = json.value("value").toString();

        // KB: Not yet implemented, but would be possible IMHO
        Q_ASSERT(false);
        qDebug() << type << userType << typeName << value;
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
                const CValueObject *cv = CValueObject::fromQVariant(m_v);
                if (cv)
                {
                    return cv->getValueHash();
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

    QDBusArgument &operator <<(QDBusArgument &arg, const CVariant &var)
    {
        arg.beginStructure();
        arg << QString(var.typeName()) << QDBusVariant(var.toQVariant());
        arg.endStructure();
        return arg;
    }

    const QDBusArgument &operator >>(const QDBusArgument &arg, CVariant &var)
    {
        QString typeName;
        QDBusVariant dbusVar;
        arg.beginStructure();
        arg >> typeName >> dbusVar;
        arg.endStructure();

        var = fixQVariantFromDbusArgument(dbusVar.variant(), QMetaType::type(qPrintable(typeName)));
        return arg;
    }
} // namespace
