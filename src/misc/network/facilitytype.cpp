// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/facilitytype.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::network, CFacilityType)

namespace swift::misc::network
{
    QString CFacilityType::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n)

        switch (m_facilityType)
        {
        case OBS: return QStringLiteral("OBS");
        case FSS: return QStringLiteral("FSS");
        case DEL: return QStringLiteral("DEL");
        case GND: return QStringLiteral("GND");
        case TWR: return QStringLiteral("TWR");
        case APP: return QStringLiteral("APP");
        case CTR: return QStringLiteral("CTR");
        case Unknown: return QStringLiteral("Unknown");
        }

        Q_UNREACHABLE();
        return QStringLiteral("Unknown");
    }

} // namespace
