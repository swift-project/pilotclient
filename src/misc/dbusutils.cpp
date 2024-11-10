// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/dbusutils.h"
#include <QString>
#include <QStringBuilder>
#include <QVariant>

namespace swift::misc
{
    QString CDBusUtils::getQDBusArgumentSignature(const QDBusArgument &arg, int level)
    {
        arg.beginArray();
        QVariant qv;
        const QString indent(level * 2, ' ');
        QString out;

        while (!arg.atEnd())
        {
            const QString type = CDBusUtils::dbusTypeAsString(arg.currentType());
            const QString signature = arg.currentSignature();
            qv = arg.asVariant(); // this advances in the stream
            if (qv.canConvert<QDBusArgument>())
            {
                out += indent % type % u"signature " % signature % u'\n';
                out += CDBusUtils::getQDBusArgumentSignature(qv.value<QDBusArgument>(), level + 1) % u'\n';
            }
            else
            {
                out += indent % u"type: " % type % u"signature " % signature % u" value " % qv.toString() % u'\n';
            }
        }
        arg.endArray();
        return out;
    }

    QString CDBusUtils::dbusTypeAsString(QDBusArgument::ElementType type)
    {
        switch (type)
        {
        case QDBusArgument::BasicType: return QLatin1String("BasicType");
        case QDBusArgument::VariantType: return QLatin1String("VariantType");
        case QDBusArgument::ArrayType: return QLatin1String("ArrayType");
        case QDBusArgument::StructureType: return QLatin1String("StructureType");
        case QDBusArgument::MapType: return QLatin1String("MapType");
        case QDBusArgument::MapEntryType: return QLatin1String("MapEntryType");
        case QDBusArgument::UnknownType:
        default:
            return QLatin1String("Unknown type");
        }
    }
} // ns
