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

    void CVariant::registerMetadata()
    {
        qRegisterMetaType<CVariant>();
        qDBusRegisterMetaType<CVariant>();
    }

    QString CVariant::toString(bool i18n) const
    {
        if (type() == QVariant::UserType)
        {
            const CValueObject *s = CValueObject::convertFromQVariant(m_v); // FIXME this will return garbage if value is not a CValueObject
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
        uint h = 0;
        if (!this->isValid() || this->isNull())
        {
            QString hs;
            hs.sprintf("%p", this);
            h = ::qHash(hs);
        }
        else
        {
            QVariant qv = this->toQVariant();
            switch (qv.type())
            {
            case QVariant::Int:
                h = ::qHash(qv.toInt());
                break;
            case QVariant::UInt:
                h = ::qHash(qv.toUInt());
                break;
            case QVariant::Bool:
                h = ::qHash(qv.toUInt());
                break;
            case QVariant::Double:
                h = ::qHash(qv.toUInt());
                break;
            case QVariant::LongLong:
                h = ::qHash(qv.toLongLong());
                break;
            case QVariant::ULongLong:
                h = ::qHash(qv.toULongLong());
                break;
            case QVariant::String:
                h = ::qHash(qv.toString());
                break;
            case QVariant::Char:
                h = ::qHash(qv.toChar());
                break;
            case QVariant::StringList:
                h = ::qHash(qv.toString());
                break;
            case QVariant::ByteArray:
                h = ::qHash(qv.toByteArray());
                break;
            case QVariant::Date:
            case QVariant::Time:
            case QVariant::DateTime:
            case QVariant::Url:
            case QVariant::Locale:
            case QVariant::RegExp:
                h = ::qHash(qv.toString());
                break;
            case QVariant::Map:
            case QVariant::List:
            case QVariant::BitArray:
            case QVariant::Size:
            case QVariant::SizeF:
            case QVariant::Rect:
            case QVariant::LineF:
            case QVariant::Line:
            case QVariant::RectF:
            case QVariant::Point:
            case QVariant::PointF:
            case QVariant::UserType:
            case QVariant::Invalid:
                h = 2; // known, but not supported
                break;
            default:
                {
                    // value object?
                    const QVariant qv = this->toQVariant();
                    const CValueObject *cv = CValueObject::convertFromQVariant(qv);
                    if (cv)
                    {
                        h = cv->getValueHash();
                    }
                    else
                    {
                        // no value object
                        QString hs;
                        hs.sprintf("%p", this);
                        h = ::qHash(hs);
                    }
                }
                break;
            }
        }
        return h;
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
