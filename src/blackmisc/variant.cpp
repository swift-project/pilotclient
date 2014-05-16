/* Copyright (C) 2014 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
            const CValueObject *s = CValueObject::fromQVariant(m_v); // FIXME this will return garbage if value is not a CValueObject
            if (s)
            {
                return s->toQString(i18n);
            }
        }
        return m_v.toString();
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