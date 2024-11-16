// SPDX-FileCopyrightText: Copyright (C) 2023 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/simulation/settings/fgswiftbussettings.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::simulation::settings, CFGSwiftBusSettings)

namespace swift::misc::simulation::settings
{
    QVariant CFGSwiftBusSettings::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusServerAddress: return QVariant::fromValue(m_dBusServerAddress);
        default: break;
        }
        return CValueObject::propertyByIndex(index);
    }

    void CFGSwiftBusSettings::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CFGSwiftBusSettings>();
            return;
        }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDBusServerAddress: m_dBusServerAddress = variant.toString(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    QString CFGSwiftBusSettings::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        return "DBusServer: " + m_dBusServerAddress;
    }

} // namespace swift::misc::simulation::settings
