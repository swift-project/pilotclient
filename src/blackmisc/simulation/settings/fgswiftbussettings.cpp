/* Copyright (C) 2023
 * swift project community / contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/simulation/settings/fgswiftbussettings.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Simulation::Settings, CFGSwiftBusSettings)

namespace BlackMisc::Simulation::Settings
{
    QVariant CFGSwiftBusSettings::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusServerAddress:
            return QVariant::fromValue(m_dBusServerAddress);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CFGSwiftBusSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CFGSwiftBusSettings>(); return; }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusServerAddress:
            m_dBusServerAddress = variant.toString(); break;
        default:
            CValueObject::setPropertyByIndex(index, variant);
            break;
        }
    }

    QString CFGSwiftBusSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return "DBusServer: " + m_dBusServerAddress;
    }

} // ns
