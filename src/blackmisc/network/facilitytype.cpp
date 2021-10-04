/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/facilitytype.h"

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Network, CFacilityType)

namespace BlackMisc::Network
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
